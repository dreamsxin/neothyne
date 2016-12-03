#include <string.h>
#include <stdlib.h> // atoi

#include "s_parser.h"
#include "s_gen.h"
#include "s_object.h"

#include "u_new.h"
#include "u_log.h"
#include "u_assert.h"
#include "u_vector.h"

namespace s {

SourceRecord *SourceRecord::m_record = nullptr;

void SourceRecord::registerSource(SourceRange source,
                                  const char *name,
                                  int rowBegin,
                                  int colBegin)
{
    SourceRecord *record = new SourceRecord;
    record->m_prev = m_record;
    record->m_source = source;
    record->m_name = name;
    record->m_rowBegin = rowBegin;
    record->m_colBegin = colBegin;
    m_record = record;
}

bool SourceRecord::findSourcePosition(char *source,
                                      const char **name,
                                      SourceRange *line,
                                      int *rowBegin,
                                      int *colBegin)
{
    SourceRecord *record = m_record;
    while (record) {
        if (source >= record->m_source.m_begin && source < record->m_source.m_end) {
            *name = record->m_name;
            int rowCount = 0;
            SourceRange lineSearch = { record->m_source.m_begin, record->m_source.m_begin };
            while (lineSearch.m_begin < record->m_source.m_end) {
                while (lineSearch.m_end < record->m_source.m_end && *lineSearch.m_end != '\n')
                    lineSearch.m_end++;
                if (lineSearch.m_end < record->m_source.m_end)
                    lineSearch.m_end++;
                if (source >= lineSearch.m_begin && source < lineSearch.m_end) {
                    int colCount = source - lineSearch.m_begin;
                    *line = lineSearch;
                    *rowBegin = rowCount + record->m_rowBegin;
                    *colBegin = colCount + ((rowCount == 0) ? record->m_colBegin : 0);
                    return true;
                }
                lineSearch.m_begin = lineSearch.m_end;
                rowCount++;
            }
            U_ASSERT(0 && "text in range but not in any line");
        }
        record = record->m_prev;
    }
    return false;
}

static void logParseError(char *location, const char *format, ...) {
    SourceRange line;
    const char *file;
    int row;
    int col;

    auto utf8len = [](const char *ptr, size_t length) {
        size_t len = 0;
        for (const char *end = ptr + length; ptr != end; ptr++)
            if ((*ptr & 0xC0) != 0x80) len++;
        return len;
    };

    va_list va;
    va_start(va, format);
    if (SourceRecord::findSourcePosition(location, &file, &line, &row, &col)) {
        u::Log::err("%s:%i:%i: error: %s\n", file, row + 1, col + 1, u::formatProcess(format, va));
        u::Log::err("%.*s", (int)(line.m_end - line.m_begin), line.m_begin);
        int u8col = utf8len(line.m_begin, col);
        int u8len = utf8len(line.m_begin, line.m_end - line.m_begin);
        for (int i = 0; i < u8len; i++) {
            if (i < u8col)
                u::Log::err(" ");
            else if (i == u8col)
                u::Log::err("^");
        }
        u::Log::err("\n");
    } else {
        u::Log::err("error: %.*s %s\n", 20, location, u::formatProcess(format, va));
    }
    va_end(va);
}

static bool startsWith(char **contents, const char *compare) {
    char *text = *contents;
    while (*compare) {
        if (!*compare)
            return false;
        if (*text != *compare)
            return false;
        text++;
        compare++;
    }
    *contents = text;
    return true;
}

static inline bool isAlpha(int ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

static inline bool isDigit (int ch) {
    return ch >= '0' && ch <= '9';
}

void Parser::consumeFiller(char **contents) {
    size_t commentDepth = 0;
    while (**contents) {
        if (commentDepth) {
            if (startsWith(contents, "*/")) {
                commentDepth--;
            } else {
                (*contents)++;
            }
        } else {
            if (startsWith(contents, "/*")) {
                commentDepth++;
            } else if (startsWith(contents, "//")) {
                while (**contents && **contents != '\n')
                    (*contents)++;
            } else if (**contents == ' ' || **contents == '\n') {
                (*contents)++;
            } else {
                break;
            }
        }
    }
}

bool Parser::consumeString(char **contents, const char *identifier) {
    char *text = *contents;
    consumeFiller(&text);
    if (startsWith(&text, identifier)) {
        *contents = text;
        return true;
    }
    return false;
}

// returns memory which can leak
const char *Parser::parseIdentifierAll(char **contents) {
    char *text = *contents;
    consumeFiller(&text);
    char *start = text;
    if (*text && (isAlpha(*text) || *text == '_'))
        text++;
    else return nullptr;
    while (*text && (isAlpha(*text) || isDigit(*text) || *text == '_'))
        text++;
    size_t length = text - start;
    char *result = (char *)neoMalloc(length + 1);
    memcpy(result, start, length);
    result[length] = '\0';
    *contents = text;
    return result;
}

const char *Parser::parseIdentifier(char **contents) {
    char *text = *contents;
    const char *result = parseIdentifierAll(&text);
    if (!result)
        return nullptr;
    if (!strncmp(result, "fn", 2) || !strncmp(result, "method", 6) || !strcmp(result, "new")) {
        // reserved identifier
        neoFree((char *)result);
        return nullptr;
    }
    *contents = text;
    return result;
}

bool Parser::parseInteger(char **contents, int *out) {
    char *text = *contents;
    consumeFiller(&text);
    char *start = text;
    int base = 10;
    if (*text && *text == '-')
        text++;
    if (text[0] == '0' && text[1] == 'x') {
        base = 16;
        text += 2;
    }
    while (*text) {
        if (base >= 10 && isDigit(*text))
            text++;
        else if (base >= 16 && isAlpha(*text))
            text++;
        else break;
    }
    if (text == start)
        return false;
    *contents = text;
    size_t length = text - start;
    char *result = (char *)neoMalloc(length + 1);
    memcpy(result, start, length);
    result[length] = '\0';
    *out = strtol(result, nullptr, base);
    neoFree(result);
    return true;
}

bool Parser::parseFloat(char **contents, float *out) {
    char *text = *contents;
    consumeFiller(&text);
    char *start = text;
    if (*text && *text == '-')
        text++;
    while (*text && isDigit(*text))
        text++;
    if (!*text || *text != '.')
        return false;
    text++;
    while (*text && isDigit(*text))
        text++;
    *contents = text;
    size_t length = text - start;
    char *result = (char *)neoMalloc(length + 1);
    memcpy(result, start, length);
    *out = atof(result);
    neoFree(result);
    return true;
}

ParseResult Parser::parseString(char **contents, char **out) {
    char *text = *contents;
    consumeFiller(&text);
    char *start = text;
    if (*text != '"')
        return kParseNone;
    text++;
    while (*text && *text != '"') // TODO(daleweiler): string escaping
        text++;
    if (!*text) {
        logParseError(text, "expected closing quote mark");
        return kParseError;
    }
    text++;
    *contents = text;
    size_t length = text - start;
    char *result = (char *)neoMalloc(length - 2 + 1);
    memcpy(result, start + 1, length - 2);
    result[length - 2] = '\0';
    *out = result;
    return kParseOk;
}

bool Parser::consumeKeyword(char **contents, const char *keyword) {
    char *text = *contents;
    const char *compare = parseIdentifierAll(&text);
    if (!compare || strcmp(compare, keyword) != 0)
        return false;
    *contents = text;
    return kParseOk;
}

///! Parser::Reference
Slot Parser::Reference::access(Gen *gen, Reference reference) {
    // during speculative parsing there is no gen
    if (gen) {
        if (reference.m_key != NoSlot)
            return Gen::addAccess(gen, reference.m_base, reference.m_key);
        return reference.m_base;
    }
    return 0;
}

void Parser::Reference::assignPlain(Gen *gen, Reference reference, Slot value) {
    U_ASSERT(reference.m_key != NoSlot);
    Gen::addAssign(gen, reference.m_base, reference.m_key, value, kAssignPlain);
}

void Parser::Reference::assignExisting(Gen *gen, Reference reference, Slot value) {
    U_ASSERT(reference.m_key != NoSlot);
    Gen::addAssign(gen, reference.m_base, reference.m_key, value, kAssignExisting);
}

void Parser::Reference::assignShadowing(Gen *gen, Reference reference, Slot value) {
    U_ASSERT(reference.m_key != NoSlot);
    Gen::addAssign(gen, reference.m_base, reference.m_key, value, kAssignShadowing);
}

Parser::Reference Parser::Reference::getScope(Gen *gen, const char *name) {
    // during speculative parsing there is no gen
    if (gen) {
        Slot nameSlot = Gen::addNewStringObject(gen, gen->m_scope, name);
        return { gen->m_scope, nameSlot, kVariable };
    }
    return { 0, NoSlot, kVariable };
}

///! Parser
ParseResult Parser::parseObjectLiteral(char **contents, Gen *gen, Slot objectSlot) {
    char *text = *contents;
    while (!consumeString(&text, "}")) {
        char *keyName = (char *)parseIdentifier(&text);
        if (!keyName) {
            ParseResult result = parseString(&text, &keyName);
            if (result != kParseOk) {
                logParseError(text, "expected identifier");
                return kParseError;
            }
        }
        if (!consumeString(&text, "=")) {
            logParseError(text, "object literal expects 'name = value'");
            return kParseError;
        }

        Reference value;
        ParseResult result = parseExpression(&text, gen, 0, &value);
        if (result == kParseError)
            return result;
        U_ASSERT(result == kParseOk);

        if (gen) {
            Slot keySlot = Gen::addNewStringObject(gen, gen->m_scope, keyName);
            Slot valueSlot = Reference::access(gen, value);
            Gen::addAssign(gen, objectSlot, keySlot, valueSlot, kAssignPlain);
        }

        if (consumeString(&text, ","))
            continue;
        if (consumeString(&text, "}"))
            break;

        logParseError(text, "expected comma or closing brace");
        return kParseError;
    }
    *contents = text;
    return kParseOk;
}

ParseResult Parser::parseObjectLiteral(char **contents, Gen *gen, Reference *reference) {
    char *text = *contents;

    if (!consumeString(&text, "{"))
        return kParseNone;

    Slot objectSlot = 0;
    if (gen)
        objectSlot = Gen::addNewObject(gen, gen->m_slot++);

    *contents = text;
    *reference = { objectSlot, Reference::NoSlot, Reference::kNone };

    return parseObjectLiteral(contents, gen, objectSlot);
}

ParseResult Parser::parseArrayLiteral(char **contents, Gen *gen, Slot objectSlot) {
    char *text = *contents;
    u::vector<Reference> values;
    while (!consumeString(&text, "]")) {
        Reference value;
        ParseResult result = parseExpression(&text, gen, 0, &value);
        if (result == kParseError)
            return result;
        U_ASSERT(result == kParseOk);

        if (gen)
            values.push_back(value);
        if (consumeString(&text, ","))
            continue;
        if (consumeString(&text, "]"))
            break;

        logParseError(text, "expected comma or closing bracket");
        return kParseError;
    }

    *contents = text;
    if (gen) {
        Slot keySlot1 = Gen::addNewStringObject(gen, gen->m_scope, "resize");
        Slot keySlot2 = Gen::addNewStringObject(gen, gen->m_scope, "[]=");
        Slot resizeFunction = Reference::access(gen, { objectSlot, keySlot1, Reference::kObject });
        Slot assignFunction = Reference::access(gen, { objectSlot, keySlot2, Reference::kObject });
        Slot resizeSlot = Gen::addNewIntObject(gen, gen->m_scope, values.size());
        objectSlot = Gen::addCall(gen, resizeFunction, objectSlot, resizeSlot);
        for (size_t i = 0; i < values.size(); i++) {
            Slot indexSlot = Gen::addNewIntObject(gen, gen->m_scope, i);
            Gen::addCall(gen, assignFunction, objectSlot, indexSlot, Reference::access(gen, values[i]));
        }
    }

    return kParseOk;
}

ParseResult Parser::parseArrayLiteral(char **contents, Gen *gen, Reference *reference) {
    char *text = *contents;
    if (!consumeString(&text, "["))
        return kParseNone;

    Slot objectSlot = 0;
    if (gen)
        objectSlot = Gen::addNewArrayObject(gen, gen->m_scope);

    *contents = text;
    *reference = { objectSlot, Reference::NoSlot, Reference::kNone };

    return parseArrayLiteral(contents, gen, objectSlot);
}

ParseResult Parser::parseExpressionStem(char **contents, Gen *gen, Reference *reference) {
    char *text = *contents;
    const char *identifier = parseIdentifier(&text);
    if (identifier) {
        *contents = text;
        *reference = Reference::getScope(gen, identifier);
        return kParseOk;
    }

    // float?
    {
        float value = 0.0f;
        if (parseFloat(&text, &value)) {
            *contents = text;
            if (gen) {
                Slot slot = Gen::addNewFloatObject(gen, gen->m_scope, value);
                *reference = { slot, Reference::NoSlot, Reference::kNone };
            }
            return kParseOk;
        }
    }
    // int?
    {
        int value = 0;
        if (parseInteger(&text, &value)) {
            *contents = text;
            if (gen) {
                Slot slot = Gen::addNewIntObject(gen, gen->m_scope, value);
                *reference = { slot, Reference::NoSlot, Reference::kNone };
            }
            return kParseOk;
        }
    }
    // string?
    {
        ParseResult result;
        char *value = nullptr;
        if ((result = parseString(&text, &value)) != kParseNone) {
            if (result == kParseOk) {
                *contents = text;
                if (gen) {
                    Slot slot = Gen::addNewStringObject(gen, gen->m_scope, value);
                    *reference = { slot, Reference::NoSlot, Reference::kNone };
                }
            }
            return result;
        }
    }

    ParseResult result;

    // object literal
    if ((result = parseObjectLiteral(&text, gen, reference)) != kParseNone) {
        if (result == kParseOk)
            *contents = text;
        return result;
    }

    // array literal
    if ((result = parseArrayLiteral(&text, gen, reference)) != kParseNone) {
        if (result == kParseOk)
            *contents = text;
        return result;
    }

    if (consumeString(&text, "(")) {
        result = parseExpression(&text, gen, 0, reference);
        if (result == kParseError)
            return result;
        U_ASSERT(result == kParseOk);

        if (!consumeString(&text, ")")) {
            logParseError(text, "'()' expected closing paren");
            return kParseError;
        }

        *contents = text;
        return kParseOk;
    }

    bool isMethod = false;
    if (consumeKeyword(&text, "fn") || (consumeKeyword(&text, "method") && (isMethod = true))) {
        UserFunction *function;
        ParseResult result = parseFunctionExpression(&text, &function);
        if (result == kParseError)
            return result;
        U_ASSERT(result == kParseOk);

        *contents = text;

        if (gen) {
            function->m_isMethod = isMethod;
            Slot slot = Gen::addNewClosureObject(gen, gen->m_scope, function);
            *reference = { slot, Reference::NoSlot, Reference::kNone };
        }
        return kParseOk;
    }

    if (consumeKeyword(&text, "new")) {
        Reference parentVariable;
        ParseResult result = parseExpression(&text, gen, 0, &parentVariable);
        if (result == kParseError)
            return result;
        U_ASSERT(result == kParseOk);

        Slot parentSlot = Reference::access(gen, parentVariable);
        Slot objectSlot = 0;
        if (gen)
            objectSlot = Gen::addNewObject(gen, parentSlot);

        *contents = text;
        if (consumeString(contents, "{")) {
            ParseResult result = parseObjectLiteral(contents, gen, objectSlot);
            if (result == kParseError)
                return result;
            U_ASSERT(result == kParseOk);
        }

        *reference = { objectSlot, Reference::NoSlot, Reference::kNone };
        return kParseOk;
    }

    logParseError(text, "expected expression");
    return kParseError;
}

ParseResult Parser::parseCall(char **contents, Gen *gen, Reference *expression) {
    char *text = *contents;
    if (!consumeString(&text, "("))
        return kParseNone;

    *contents = text;
    Slot *arguments = nullptr;
    size_t length = 0;

    while (!consumeString(&text, ")")) {
        if (length && !consumeString(&text, ",")) {
            logParseError(text, "expected comma or closing parenthesis");
            return kParseError;
        }

        Reference argument;
        ParseResult result = parseExpression(&text, gen, 0, &argument);
        if (result == kParseError)
            return result;
        U_ASSERT(result == kParseOk);

        Slot slot = Reference::access(gen, argument);
        arguments = (Slot *)neoRealloc(arguments, sizeof(Slot) * ++length);
        arguments[length - 1] = slot;
    }

    *contents = text;

    if (gen) {
        Slot thisSlot;
        if (expression->m_key)
            thisSlot = expression->m_base;
        else
            thisSlot = gen->m_slot++;

        *expression = {
            Gen::addCall(gen, Reference::access(gen, *expression), thisSlot, arguments, length),
            Reference::NoSlot,
            Reference::kNone
        };
    }

    return kParseOk;
}

ParseResult Parser::parseArrayAccess(char **contents, Gen *gen, Reference *expression) {
    char *text = *contents;
    if (!consumeString(&text, "["))
        return kParseNone;

    Reference key;
    ParseResult result = parseExpression(&text, gen, 0, &key);
    if (result == kParseError)
        return result;
    U_ASSERT(result == kParseOk);

    if (!consumeString(&text, "]")) {
        logParseError(*contents, "expected closing ']'");
        return kParseError;
    }

    Slot keySlot = Reference::access(gen, key);

    *contents = text;

    *expression = {
        Reference::access(gen, *expression),
        keySlot,
        Reference::kIndex
    };

    return kParseOk;
}

ParseResult Parser::parsePropertyAccess(char **contents, Gen *gen, Reference *expression) {
    char *text = *contents;
    if (!consumeString(&text, "."))
        return kParseNone;

    const char *keyName = parseIdentifier(&text);
    *contents = text;

    Slot keySlot = 0;
    if (gen)
        keySlot = Gen::addNewStringObject(gen, gen->m_scope, keyName);

    *expression = {
        Reference::access(gen, *expression),
        keySlot,
        Reference::kObject
    };

    return kParseOk;
}

ParseResult Parser::parseExpression(char **contents, Gen *gen, Reference *reference) {
    ParseResult result = parseExpressionStem(contents, gen, reference);
    if (result == kParseError)
        return result;
    U_ASSERT(result == kParseOk);

    for (;;) {
        if ((result = parseCall(contents, gen, reference)) == kParseOk)
            continue;
        if (result == kParseError)
            return result;
        if ((result = parsePropertyAccess(contents, gen, reference)) == kParseOk)
            continue;
        if (result == kParseError)
            return result;
        if ((result = parseArrayAccess(contents, gen, reference)) == kParseOk)
            continue;
        if (result == kParseError)
            return result;
        break;
    }

    return kParseOk;
}

void Parser::buildOperation(Gen *gen, const char *op, Reference *lhs, Reference rhs) {
    if (gen) {
        Slot lhsSlot = Reference::access(gen, *lhs);
        Slot rhsSlot = Reference::access(gen, rhs);
        Slot function = Gen::addAccess(gen, lhsSlot, Gen::addNewStringObject(gen, gen->m_scope, op));
        *lhs = { Gen::addCall(gen, function, lhsSlot, rhsSlot), Reference::NoSlot, Reference::kNone };
    }
}

ParseResult Parser::parseExpression(char **contents, Gen *gen, int level, Reference *reference) {
    char *text = *contents;

    ParseResult result = parseExpression(&text, gen, reference);
    if (result == kParseError)
        return result;
    U_ASSERT(result == kParseOk);

    if (level > 2) {
        *contents = text;
        return kParseOk;
    }

    Reference expression;
    for (;;) {
        if (consumeString(&text, "*")) {
            result = parseExpression(&text, gen, 3, &expression);
            if (result == kParseError)
                return result;
            U_ASSERT(result == kParseOk);
            buildOperation(gen, "*", reference, expression);
            continue;
        }
        if (consumeString(&text, "/")) {
            result = parseExpression(&text, gen, 3, &expression);
            if (result == kParseError)
                return result;
            U_ASSERT(result == kParseOk);
            buildOperation(gen, "/", reference, expression);
            continue;
        }
        break;
    }

    if (level > 1) {
        *contents = text;
        return kParseOk;
    }

    for (;;) {
        if (consumeString(&text, "+")) {
            result = parseExpression(&text, gen, 2, &expression);
            if (result == kParseError)
                return result;
            U_ASSERT(result == kParseOk);
            buildOperation(gen, "+", reference, expression);
            continue;
        }
        if (consumeString(&text, "-")) {
            result = parseExpression(&text, gen, 2, &expression);
            if (result == kParseError)
                return result;
            U_ASSERT(result == kParseOk);
            buildOperation(gen, "-", reference, expression);
            continue;
        }
        break;
    }

    if (level > 0) {
        *contents = text;
        return kParseOk;
    }

    bool negated = false;

    if (consumeString(&text, "==")) {
        result = parseExpression(&text, gen, 1, &expression);
        if (result == kParseError)
            return result;
        U_ASSERT(result == kParseOk);
        buildOperation(gen, "==", reference, expression);
    } else if (consumeString(&text, "!=")) {
        // the same code except result is negated
        result = parseExpression(&text, gen, 1, &expression);
        if (result == kParseError)
            return result;
        U_ASSERT(result == kParseOk);
        buildOperation(gen, "==", reference, expression);
        negated = true;
    } else {
        // this allows for operators like !<, !>, !<=, !>=
        if (consumeString(&text, "!"))
            negated = true;
        if (consumeString(&text, "<=")) {
            result = parseExpression(&text, gen, 1, &expression);
            if (result == kParseError)
                return result;
            U_ASSERT(result == kParseOk);
            buildOperation(gen, "<=", reference, expression);
        } else if (consumeString(&text, ">=")) {
            result = parseExpression(&text, gen, 1, &expression);
            if (result == kParseError)
                return result;
            U_ASSERT(result == kParseOk);
            buildOperation(gen, ">=", reference, expression);
        } else if (consumeString(&text, "<")) {
            result = parseExpression(&text, gen, 1, &expression);
            if (result == kParseError)
                return result;
            U_ASSERT(result == kParseOk);
            buildOperation(gen, "<", reference, expression);
        } else if (consumeString(&text, ">")) {
            result = parseExpression(&text, gen, 1, &expression);
            if (result == kParseError)
                return result;
            U_ASSERT(result == kParseOk);
            buildOperation(gen, ">", reference, expression);
        } else if (negated) {
            logParseError(text, "expected comparison operator");
            return kParseError;
        }
    }

    if (negated && gen) {
        Slot lhs = Reference::access(gen, *reference);
        Slot bnot = Gen::addAccess(gen, lhs, Gen::addNewStringObject(gen, gen->m_scope, "!"));
        *reference = { Gen::addCall(gen, bnot, lhs), Reference::NoSlot, Reference::kNone };
    }

    *contents = text;
    return kParseOk;
}

ParseResult Parser::parseIfStatement(char **contents, Gen *gen) {
    char *text = *contents;
    if (!consumeString(&text, "(")) {
        logParseError(text, "expected opening paren after 'if'");
        return kParseError;
    }

    Reference testExpression;
    ParseResult result = parseExpression(&text, gen, 0, &testExpression);
    if (result == kParseError)
        return result;
    U_ASSERT(result == kParseOk);

    Slot testSlot = Reference::access(gen, testExpression);
    if (!consumeString(&text, ")")) {
        logParseError(text, "expected closing paren after 'if'");
        return kParseError;
    }

    BlockRef trueBlock;
    BlockRef falseBlock;
    BlockRef endBlock;
    Gen::addTestBranch(gen, testSlot, &trueBlock, &falseBlock);

    Gen::setBlockRef(gen, trueBlock, Gen::newBlock(gen));

    result = parseBlock(&text, gen);
    if (result == kParseError)
        return kParseError;
    U_ASSERT(result == kParseOk);

    Gen::addBranch(gen, &endBlock);

    size_t falseBlockIndex = Gen::newBlock(gen);
    Gen::setBlockRef(gen, falseBlock, falseBlockIndex);
    if (consumeKeyword(&text, "else")) {
        result = parseBlock(&text, gen);
        if (result == kParseError)
            return kParseError;
        U_ASSERT(result == kParseOk);
        BlockRef elseEndBlock;
        Gen::addBranch(gen, &elseEndBlock);

        size_t blockIndex = Gen::newBlock(gen);
        Gen::setBlockRef(gen, elseEndBlock, blockIndex);
        Gen::setBlockRef(gen, endBlock, blockIndex);
    } else {
        Gen::setBlockRef(gen, endBlock, falseBlockIndex);
    }

    *contents = text;
    return kParseOk;
}

ParseResult Parser::parseWhile(char **contents, Gen *gen) {
    char *text = *contents;
    if (!consumeString(&text, "(")) {
        logParseError(text, "expected opening paren after 'while'");
        return kParseError;
    }


    BlockRef testBlock;
    Gen::addBranch(gen, &testBlock);
    size_t testBlockIndex = Gen::newBlock(gen);
    Gen::setBlockRef(gen, testBlock, testBlockIndex);

    BlockRef loopBlock;
    BlockRef endBlock;

    Reference testExpression;
    ParseResult result = parseExpression(&text, gen, 0, &testExpression);
    if (result == kParseError)
        return result;
    U_ASSERT(result == kParseOk);

    Slot testSlot = Reference::access(gen, testExpression);
    if (!consumeString(&text, ")")) {
        logParseError(text, "expected closing paren after 'while'");
        return kParseError;
    }

    Gen::addTestBranch(gen, testSlot, &loopBlock, &endBlock);

    Gen::setBlockRef(gen, loopBlock, Gen::newBlock(gen));
    result = parseBlock(&text, gen);
    if (result == kParseError)
        return result;
    U_ASSERT(result == kParseOk);

    BlockRef testBlock2;
    Gen::addBranch(gen, &testBlock2);

    Gen::setBlockRef(gen, testBlock2, testBlockIndex);
    Gen::setBlockRef(gen, endBlock, Gen::newBlock(gen));

    *contents = text;
    return kParseOk;
}

ParseResult Parser::parseReturnStatement(char **contents, Gen *gen) {
    Reference returnValue;
    ParseResult result = parseExpression(contents, gen, 0, &returnValue);
    if (result == kParseError)
        return result;
    U_ASSERT(result == kParseOk);

    Slot value = Reference::access(gen, returnValue);
    if (!consumeString(contents, ";")) {
        logParseError(*contents, "expected semicolon");
        return kParseError;
    }

    Gen::addReturn(gen, value);
    Gen::newBlock(gen);

    return kParseOk;
}

ParseResult Parser::parseLetDeclaration(char **contents, Gen *gen) {
    char *text = *contents;

    // allocate a new scope immediately to allow recursion for closures
    // i.e allow: let foo = fn() { foo(); };
    gen->m_scope = Gen::addNewObject(gen, gen->m_scope);

    const char *variableName = parseIdentifier(&text);
    Slot value;
    Slot variableNameSlot = Gen::addNewStringObject(gen, gen->m_scope, variableName);
    if (!consumeString(&text, "=")) {
        value = gen->m_slot++;
    } else {
        Reference reference;
        ParseResult result = parseExpression(&text, gen, 0, &reference);
        if (result == kParseError)
            return result;
        U_ASSERT(result == kParseOk);

        value = Reference::access(gen, reference);
    }

    Gen::addAssign(gen, gen->m_scope, variableNameSlot, value, kAssignPlain);
    Gen::addCloseObject(gen, gen->m_scope);

    // let a, b
    if (consumeString(&text, ",")) {
        *contents = text;
        return parseLetDeclaration(contents, gen);
    }

    if (!consumeString(&text, ";")) {
        logParseError(text, "expected ';' to close let declaration");
        return kParseError;
    }

    *contents = text;
    return kParseOk;
}

ParseResult Parser::parseFunctionDeclaration(char **contents, Gen *gen) {
    gen->m_scope = Gen::addNewObject(gen, gen->m_scope);

    UserFunction *function;
    ParseResult result = parseFunctionExpression(contents, &function);
    if (result == kParseError)
        return result;
    U_ASSERT(result == kParseOk);

    Slot nameSlot = Gen::addNewStringObject(gen, gen->m_scope, function->m_name);
    Slot slot = Gen::addNewClosureObject(gen, gen->m_scope, function);
    Gen::addAssign(gen, gen->m_scope, nameSlot, slot, kAssignPlain);
    Gen::addCloseObject(gen, gen->m_scope);

    return kParseOk;
}

ParseResult Parser::parseStatement(char **contents, Gen *gen) {
    char *text = *contents;
    if (consumeKeyword(&text, "if")) {
        *contents = text;
        return parseIfStatement(contents, gen);
    }
    if (consumeKeyword(&text, "return")) {
        *contents = text;
        return parseReturnStatement(contents, gen);
    }
    if (consumeKeyword(&text, "let")) {
        *contents = text;
        return parseLetDeclaration(contents, gen);
    }
    if (consumeKeyword(&text, "fn")) {
        *contents = text;
        return parseFunctionDeclaration(contents, gen);
    }
    if (consumeKeyword(&text, "while")) {
        *contents = text;
        return parseWhile(contents, gen);
    }

    // variable assignment
    {
        char *next = text;
        Reference reference;
        ParseResult result = parseExpression(&next, nullptr, &reference); // speculative parse
        if (result == kParseError)
            return result;

        if (result == kParseOk && consumeString(&next, "=")) {
            result = parseExpression(&text, gen, &reference);
            U_ASSERT(result == kParseOk);

            if (!consumeString(&text, "=")) {
                U_ASSERT(0 && "internal compiler error");
            }

            Reference valueExpression;
            result = parseExpression(&text, gen, 0, &valueExpression);
            if (result == kParseError)
                return result;
            U_ASSERT(result == kParseOk);

            Slot value = Reference::access(gen, valueExpression);

            switch (reference.m_mode) {
            case Reference::kNone:
                logParseError(text, "cannot assign to non-reference expression");
                return kParseError;
            case Reference::kVariable:
                Reference::assignExisting(gen, reference, value);
                break;
            case Reference::kObject:
                Reference::assignShadowing(gen, reference, value);
                break;
            case Reference::kIndex:
                Reference::assignPlain(gen, reference, value);
                break;
            default:
                U_ASSERT(0 && "internal compiler error");
            }

            if (!consumeString(&text, ";")) {
                logParseError(text, "expected ';' to terminate assignment statement");
                return kParseError;
            }

            *contents = text;
            return kParseOk;
        }
    }

    // expression as statement
    {
        Reference reference;
        ParseResult result = parseExpression(&text, gen, &reference);
        if (result == kParseError)
            return result;
        U_ASSERT(result == kParseOk);

        if (!consumeString(&text, ";")) {
            logParseError(text, "expected ';' to terminate expression");
            return kParseError;
        }

        *contents = text;
        return kParseOk;
    }

    logParseError(text, "unknown statement");
    return kParseError;
}

ParseResult Parser::parseBlock(char **contents, Gen *gen) {
    char *text = *contents;

    // Note: blocks don't actually open new scopes
    Slot currentScope = gen->m_scope;

    ParseResult result;
    if (consumeString(&text, "{")) {
        while (!consumeString(&text, "}")) {
            result = parseStatement(&text, gen);
            if (result == kParseError)
                return result;
            U_ASSERT(result == kParseOk);
        }
    } else {
        result = parseStatement(&text, gen);
        if (result == kParseError)
            return result;
        U_ASSERT(result == kParseOk);
    }

    *contents = text;
    gen->m_scope = currentScope;
    return kParseOk;
}

ParseResult Parser::parseFunctionExpression(char **contents, UserFunction **function) {
    char *text = *contents;
    const char *functionName = parseIdentifier(&text);

    if (!consumeString(&text, "(")) {
        logParseError(text, "expected opening paren for parameter list");
        return kParseError;
    }

    const char **arguments = nullptr;
    size_t length = 0;
    while (!consumeString(&text, ")")) {
        if (length && !consumeString(&text, ",")) {
            logParseError(text, "expected comma in parameter list");
            return kParseError;
        }
        const char *argument = parseIdentifier(&text);
        if (!argument) {
            logParseError(text, "expected identifier for parameter in parameter list");
            return kParseError;
        }
        arguments = (const char **)neoRealloc(arguments, sizeof(const char *) * ++length);
        arguments[length - 1] = argument;
    }

    *contents = text;

    Gen *gen = (Gen *)neoCalloc(sizeof *gen, 1);
    gen->m_arguments = arguments;
    gen->m_count = length;
    gen->m_slot = length;
    gen->m_name = functionName;
    gen->m_blockTerminated = true;

    // generate lexical scope
    Gen::newBlock(gen);
    Slot contextSlot = Gen::addGetContext(gen);
    gen->m_scope = Gen::addNewObject(gen, contextSlot);
    for (size_t i = 0; i < length; i++) {
        Slot argumentSlot = Gen::addNewStringObject(gen, gen->m_scope, arguments[i]);
        Gen::addAssign(gen, gen->m_scope, argumentSlot, i, kAssignPlain);
    }
    Gen::addCloseObject(gen, gen->m_scope);

    ParseResult result = parseBlock(contents, gen);
    if (result == kParseError)
        return result;
    U_ASSERT(result == kParseOk);

    Gen::terminate(gen);

    *function = Gen::optimize(Gen::buildFunction(gen));
    neoFree(gen);
    return kParseOk;
}

ParseResult Parser::parseModule(char **contents, UserFunction **function) {
    Gen *gen = (Gen *)neoCalloc(sizeof *gen, 1);
    gen->m_slot = 0;
    gen->m_name = nullptr;
    gen->m_blockTerminated = true;

    Gen::newBlock(gen);
    gen->m_scope = Gen::addGetContext(gen);

    for (;;) {
        consumeFiller(contents);
        if ((*contents)[0] == '\0')
            break;
        ParseResult result = parseStatement(contents, gen);
        if (result == kParseError)
            return result;
        U_ASSERT(result == kParseOk);
    }

    Gen::addReturn(gen, gen->m_scope);
    *function = Gen::optimize(Gen::buildFunction(gen));
    neoFree(gen);
    return kParseOk;
}

}
