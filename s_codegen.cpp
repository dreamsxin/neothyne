#include <string.h>

#include "s_object.h"
#include "s_codegen.h"

namespace s {

FunctionCodegen::FunctionCodegen(const char **arguments, size_t length, const char *name)
    : m_arguments(arguments)
    , m_length(length)
    , m_name(name)
    , m_scope(0)
    , m_slotBase(length)
{
    memset(&m_body, 0, sizeof m_body);
}

size_t FunctionCodegen::newBlock() {
    FunctionBody *body = &m_body;
    body->m_length++;
    body->m_blocks = (InstrBlock *)neoRealloc(body->m_blocks, body->m_length * sizeof(InstrBlock));
    body->m_blocks[body->m_length - 1].m_instrs = nullptr;
    body->m_blocks[body->m_length - 1].m_length = 0;
    return body->m_length - 1;
}

void FunctionCodegen::addInstr(Instr *instruction) {
    FunctionBody *body = &m_body;
    InstrBlock *block = &body->m_blocks[body->m_length - 1];
    block->m_length++;
    block->m_instrs = (Instr **)neoRealloc(block->m_instrs, block->m_length * sizeof(Instr *));
    block->m_instrs[block->m_length - 1] = instruction;
}

Slot FunctionCodegen::addAccess(Slot objectSlot, const char *identifier) {
    auto *instruction = allocate<AccessInstr>();
    instruction->m_type = Instr::kAccess;
    instruction->m_targetSlot = m_slotBase++;
    instruction->m_objectSlot = objectSlot;
    instruction->m_key = identifier;
    addInstr((Instr *)instruction);
    return instruction->m_targetSlot;
}

void FunctionCodegen::addAssign(Slot object, const char *name, Slot slot) {
    auto *instruction = allocate<AssignInstr>();
    instruction->m_type = Instr::kAssign;
    instruction->m_objectSlot = object;
    instruction->m_valueSlot = slot;
    instruction->m_key = name;
    addInstr((Instr *)instruction);
}

void FunctionCodegen::addCloseObject(Slot object) {
    auto *instruction = allocate<CloseObjectInstr>();
    instruction->m_type = Instr::kCloseObject;
    instruction->m_slot = object;
    addInstr((Instr *)instruction);
}

Slot FunctionCodegen::addGetContext() {
    auto *instruction = allocate<GetContextInstr>();
    instruction->m_type = Instr::kGetContext;
    instruction->m_slot = m_slotBase++;
    addInstr((Instr *)instruction);
    return instruction->m_slot;
}

Slot FunctionCodegen::addAllocObject(Slot parent) {
    auto *instruction = allocate<AllocObjectInstr>();
    instruction->m_type = Instr::kAllocObject;
    instruction->m_targetSlot = m_slotBase++;
    instruction->m_parentSlot = parent;
    addInstr((Instr *)instruction);
    return instruction->m_targetSlot;
}

Slot FunctionCodegen::addAllocIntObject(int value) {
    auto *instruction = allocate<AllocIntObjectInstr>();
    instruction->m_type = Instr::kAllocIntObject;
    instruction->m_targetSlot = m_slotBase++;
    instruction->m_value = value;
    addInstr((Instr *)instruction);
    return instruction->m_targetSlot;
}

Slot FunctionCodegen::addCall(Slot function, Slot *arguments, size_t length) {
    auto *instruction = allocate<CallInstr>();
    instruction->m_type = Instr::kCall;
    instruction->m_targetSlot = m_slotBase++;
    instruction->m_functionSlot = function;
    instruction->m_length = length;
    instruction->m_arguments = arguments;
    addInstr((Instr *)instruction);
    return instruction->m_targetSlot;
}

Slot FunctionCodegen::addCall(Slot function, Slot arg0, Slot arg1) {
    Slot *arguments = (Slot *)neoMalloc(sizeof *arguments * 2);
    arguments[0] = arg0;
    arguments[1] = arg1;
    return addCall(function, arguments, 2);
}

void FunctionCodegen::addTestBranch(Slot test, size_t **trueBranch, size_t **falseBranch) {
    auto *instruction = allocate<TestBranchInstr>();
    instruction->m_type = Instr::kTestBranch;
    instruction->m_testSlot = test;
    *trueBranch = &instruction->m_trueBlock;
    *falseBranch = &instruction->m_falseBlock;
    addInstr((Instr *)instruction);
}

void FunctionCodegen::addBranch(size_t **branch) {
    auto *instruction = allocate<BranchInstr>();
    instruction->m_type = Instr::kBranch;
    *branch = &instruction->m_block;
    addInstr((Instr *)instruction);
}

void FunctionCodegen::addReturn(Slot slot) {
    auto *instruction = allocate<ReturnInstr>();
    instruction->m_type = Instr::kReturn;
    instruction->m_returnSlot = slot;
    addInstr((Instr *)instruction);
}

UserFunction *FunctionCodegen::build() {
    auto *function = allocate<UserFunction>();
    function->m_arity = m_length;
    function->m_slots = m_slotBase;
    function->m_name = m_name;
    function->m_body = m_body;
    return function;
}

}
