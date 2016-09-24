#include <string.h>

#include "s_object.h"
#include "s_instr.h"

#include "u_assert.h"
#include "u_new.h"

#include "engine.h" // neoFatal

namespace s {

Object *equals(Object *context, Object *function, Object **args, size_t length) {
    (void)function;
    U_ASSERT(length == 2);
    Object *root = context;
    while (root->m_parent) root = root->m_parent;
    Object *intBase = root->m_table.lookup("int");
    Object *object1 = args[0];
    Object *object2 = args[1];
    if (object1->m_parent == intBase && object2->m_parent == intBase) {
        bool test = ((IntObject *)object1)->m_value == ((IntObject *)object2)->m_value;
        return Object::newBoolean(context, test);
    }
    U_UNREACHABLE();
}

#define MATHOP(NAME, OP) \
Object *NAME(Object *context, Object *function, Object **args, size_t length) { \
    (void)function; \
    U_ASSERT(length == 2); \
    Object *root = context; \
    while (root->m_parent) root = root->m_parent; \
    Object *intBase = root->m_table.lookup("int"); \
    Object *object1 = args[0]; \
    Object *object2 = args[1]; \
    if (object1->m_parent == intBase && object2->m_parent == intBase) { \
        int result = ((IntObject *)object1)->m_value OP ((IntObject *)object2)->m_value; \
        return Object::newInt(context, result); \
    } \
    U_UNREACHABLE(); \
}

MATHOP(add, +)
MATHOP(sub, -)
MATHOP(mul, *)

Object *call(Object *context, UserFunction *function, Object **args, size_t length) {
    // allocate slots for arguments and locals
    size_t numSlots = function->m_arity + function->m_slots;
    Object **slots = allocate<Object *>(numSlots);

    // ensure this call is valid
    U_ASSERT(length == function->m_arity);

    // copy the arguments into the slots
    for (size_t i = 0; i < length; ++i)
        slots[i] = args[i];

    // ensure this function has code
    U_ASSERT(function->m_body.m_length);

    InstrBlock *block = &function->m_body.m_blocks[0];
    size_t offset = 0;
    for (;;) {
        if (!(offset < block->m_length))
            neoFatal("reached end of block without a branch");
        Instr *instr = block->m_instrs[offset++];

        switch (instr->m_type) {
        case Instr::kGetRoot: {
            auto *getRoot = (GetRootInstr *)instr;
            size_t slot = getRoot->m_slot;
            Object *root = context;
            while (root->m_parent) root = root->m_parent;
            U_ASSERT(slot < numSlots && !slots[slot]);
            slots[getRoot->m_slot] = root;
        } break;

        case Instr::kGetContext: {
            auto *getContext = (GetContextInstr *)instr;
            size_t slot = getContext->m_slot;
            U_ASSERT(slot < numSlots && !slots[slot]);
            slots[slot] = context;
        } break;

        case Instr::kAccess: {
            auto *access = (AccessInstr *)instr;
            size_t targetSlot = access->m_targetSlot;
            size_t objectSlot = access->m_objectSlot;
            const char *key = access->m_key;
            U_ASSERT(targetSlot < numSlots && !slots[targetSlot]);
            U_ASSERT(objectSlot < numSlots);
            for (Object *object = slots[objectSlot]; object; object = object->m_parent) {
                Object *value = object->m_table.lookup(key);
                if (value) {
                    slots[targetSlot] = value;
                    break;
                }
            }
        } break;

        case Instr::kAssign: {
            AssignInstr *i = (AssignInstr *)instr;
            Slot objectSlot = i->m_objectSlot;
            Slot valueSlot = i->m_valueSlot;
            const char *key = i->m_key;
            U_ASSERT(objectSlot < numSlots);
            U_ASSERT(valueSlot < numSlots);
            Object *object = slots[objectSlot];
            object->set(key, slots[valueSlot]);
        } break;

        case Instr::kAllocObject: {
            AllocObjectInstr *i = (AllocObjectInstr *)instr;
            Slot targetSlot = i->m_targetSlot;
            Slot parentSlot = i->m_parentSlot;
            U_ASSERT(targetSlot < numSlots && !slots[targetSlot]);
            U_ASSERT(parentSlot < numSlots);
            slots[targetSlot] = Object::newObject(slots[parentSlot]);
        } break;

        case Instr::kAllocIntObject: {
            AllocIntObjectInstr *i = (AllocIntObjectInstr *)instr;
            Slot target = i->m_targetSlot;
            int value = i->m_value;
            U_ASSERT(target < numSlots && !slots[target]);
            slots[target] = Object::newInt(context, value);
        } break;

        case Instr::kCall: {
            auto *call = (CallInstr *)instr;
            size_t targetSlot = call->m_targetSlot;
            size_t functionSlot = call->m_functionSlot;
            size_t argsLength = call->m_argsLength;
            U_ASSERT(targetSlot < numSlots && !slots[targetSlot]);
            U_ASSERT(functionSlot < numSlots);
            Object *functionObject = slots[functionSlot];
            Object *root = context;
            while (root->m_parent) root = root->m_parent;
            // validate function type
            Object *functionBase = root->m_table.lookup("function");
            Object *functionType = functionObject->m_parent;
            while (functionType->m_parent) functionType = functionType->m_parent;
            U_ASSERT(functionType == functionBase);
            FunctionObject *function = (FunctionObject *)functionObject;
            // form the argument array from slots
            Object **args = (Object **)neoMalloc(sizeof *args * argsLength);
            for (size_t i = 0; i < argsLength; i++) {
                size_t argSlot = call->m_argsData[i];
                U_ASSERT(argSlot < numSlots);
                args[i] = slots[argSlot];
            }
            // now call
            slots[targetSlot] = function->m_function(context, functionObject, args, argsLength);
            neoFree(args);
        } break;

        case Instr::kReturn: {
            auto *ret = (ReturnInstr *)instr;
            size_t returnSlot = ret->m_returnSlot;
            U_ASSERT(returnSlot < numSlots);
            Object *result = slots[returnSlot];
            neoFree(slots);
            return result;
        } break;

        case Instr::kBranch: {
            auto *branch = (BranchInstr *)instr;
            size_t blockIndex = branch->m_block;
            U_ASSERT(blockIndex < function->m_body.m_length);
            block = &function->m_body.m_blocks[blockIndex];
            offset = 0;
        } break;

        case Instr::kTestBranch: {
            auto *testBranch = (TestBranchInstr *)instr;
            size_t testSlot = testBranch->m_testSlot;
            size_t trueBlock = testBranch->m_trueBlock;
            size_t falseBlock = testBranch->m_falseBlock;
            U_ASSERT(testSlot < numSlots);
            Object *testValue = slots[testSlot];

            Object *root = context;
            while (root->m_parent) root = root->m_parent;
            Object *booleanBase = root->m_table.lookup("boolean");
            Object *intBase = root->m_table.lookup("int");

            bool test = false;
            if (testValue && testValue->m_parent == booleanBase) {
                if (((BooleanObject *)testValue)->m_value == true)
                    test = true;
            } else if (testValue && testValue->m_parent == intBase) {
                if (((IntObject *)testValue)->m_value != 0)
                    test = true;
            } else {
                test = !!testValue;
            }

            size_t targetBlock = test ? trueBlock : falseBlock;
            U_ASSERT(targetBlock < function->m_body.m_length);
            block = &function->m_body.m_blocks[targetBlock];
            offset = 0;
        } break;

        }
    }
}

Object *handler(Object *context, Object *function, Object **args, size_t length) {
    UserFunctionObject *functionObject = (UserFunctionObject *)function;
    return call(context, &functionObject->m_userFunction, args, length);
}

Object *createRoot() {
    Object *root = Object::newObject(nullptr);
    root->set("int", Object::newObject(nullptr));
    root->set("boolean", Object::newObject(nullptr));
    root->set("function", Object::newObject(nullptr));
    root->set("=", Object::newFunction(root, equals));
    root->set("+", Object::newFunction(root, add));
    root->set("-", Object::newFunction(root, sub));
    root->set("*", Object::newFunction(root, mul));
    return root;
}

}
