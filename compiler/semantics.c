#include <stdio.h>
#include "ast.h"
#include "errors.h"
#include "string.h"
#include "interpreter.h"

char* i8;
char* i16;
char* i32;
char* i64;
char* u8;
char* u16;
char* u32;
char* u64;
char* f32;
char* f64;
char* boolean;

DECLARE_LIST_APPEND(Symbol)

char* typeToString(Type* type)
{
    switch (type->tag)
    {
        case TYPE_I8:
            return i8;
        case TYPE_I16:
            return i16;
        case TYPE_I32:
            return i32;
        case TYPE_I64:
            return i64;
        case TYPE_U8:
            return u8;
        case TYPE_U16:
            return u16;
        case TYPE_U32:
            return u32;
        case TYPE_U64:
            return u64;
        case TYPE_F32:
            return f32;
        case TYPE_F64:
            return f64;
        case TYPE_BOOL:
            return boolean;
        case TYPE_FUNCTION:
            error("typeToString TYPE_FUNCTION\n");
        case TYPE_ENUMERATION:
            error("typeToString TYPE_ENUMERATION\n");
        case TYPE_STRUCTURE:
            error("typeToString TYPE_STRUCTURE\n");
        case TYPE_UNION:
            error("typeToString TYPE_UNION\n");
    }

    return NULL;
}

Symbol* resolve(Scope* scope, char* name)
{
    for (int32_t i = 0; i < scope->symbols.count; i++)
    {
        if (scope->symbols.elements[i].name == name)
            return &scope->symbols.elements[i];
    }

    if (scope->parent == NULL)
        return NULL;
    else
        return resolve(scope->parent, name);
}

bool compareSymbols(Symbol* a, Symbol* b)
{
    return a == NULL && b == NULL ||
           a != NULL && b != NULL &&
           a->type.tag == b->type.tag;
}

Source* positionOfExpression(Expression* expression)
{
    switch (expression->tag)
    {
        case EXPRESSION_IDENTIFIER:
        case EXPRESSION_VARIABLE_DECLARATION:
        case EXPRESSION_VARIABLE_REFERENCE:
            return &expression->identifier.position;
        case EXPRESSION_NUMBER:
            return &expression->number.position;
        default:
            error("Could not get position of expression.");
            return NULL;
    }
}

void putDeclarationsIntoScope(Program* program)
{
    void addPrimitive(SymbolList* list, char* name, int32_t tag)
    {
        appendSymbol(list, (Symbol)
        {
            .name = name,
            .type = (Type)
            {
                .tag = tag
            }
        });
    }

    Scope* primitives = calloc(1, sizeof(Scope));
    i8 = internString(&program->interned, "i8");
    i16 = internString(&program->interned, "i16");
    i32 = internString(&program->interned, "i32");
    i64 = internString(&program->interned, "i64");
    u8 = internString(&program->interned, "u8");
    u16 = internString(&program->interned, "u16");
    u32 = internString(&program->interned, "u32");
    u64 = internString(&program->interned, "u64");
    f32 = internString(&program->interned, "f32");
    f64 = internString(&program->interned, "f64");
    boolean = internString(&program->interned, "bool");

    addPrimitive(&primitives->symbols, i8, TYPE_I8);
    addPrimitive(&primitives->symbols, i16, TYPE_I16);
    addPrimitive(&primitives->symbols, i32, TYPE_I32);
    addPrimitive(&primitives->symbols, i64, TYPE_I64);
    addPrimitive(&primitives->symbols, u8, TYPE_I8);
    addPrimitive(&primitives->symbols, u16, TYPE_I16);
    addPrimitive(&primitives->symbols, u32, TYPE_I32);
    addPrimitive(&primitives->symbols, u64, TYPE_I64);
    addPrimitive(&primitives->symbols, f32, TYPE_F32);
    addPrimitive(&primitives->symbols, f64, TYPE_F64);
    addPrimitive(&primitives->symbols, boolean, TYPE_BOOL);

    for (int32_t a = 0; a < program->files.count; a++)
    {
        File* file = &program->files.elements[a];
        file->scope.parent = primitives;

        for (int32_t f = 0; f < file->functions.count; f++)
        {
            FunctionDeclaration* declaration = &file->functions.elements[f];
            Symbol symbol =
            {
                .name = declaration->name.value,
                .type.tag = TYPE_FUNCTION,
                .type.function.declaration = declaration,
                .type.function.input = (SymbolList) NEW_LIST(),
                .type.function.output = (SymbolList) NEW_LIST()
            };

            for (int32_t i = 0; i < declaration->output.count; i++)
            {
                Symbol* type = resolve(&file->scope, declaration->output.elements[i].value);
                if (type == NULL)
                    errorAt(&declaration->output.elements[i].position, "%s is undefined.", declaration->output.elements[i].value);
                else
                    appendSymbol(&symbol.type.function.output, *type);
            }

            appendSymbol(&file->scope.symbols, symbol);
        }
    }

    for (int32_t u = 0; u < program->files.count; u++)
    {
        for (int32_t o = 0; o < program->files.count; o++)
        {
            if (u != o)
            {
                File* file = &program->files.elements[u];
                File* other = &program->files.elements[o];
                if (file->namespace == other->namespace || stringListContains(&file->uses, other->namespace))
                {
                    for (int32_t s = 0; s < other->scope.symbols.count; s++)
                    {
                        Symbol* symbol = &other->scope.symbols.elements[s];
                        bool canAddSymbol = symbol->type.tag && symbol->type.function.declaration->isPublic;

                        if (canAddSymbol)
                            appendSymbol(&file->scope.symbols, *symbol);
                    }
                }
            }
        }
    }
}

void inferNumberType(Expression* expression, Symbol* expected)
{
    if (expression->number.tag == NUMBER_INTEGER_TO_BE_INFERRED)
    {
        switch (expected->type.tag)
        {
            case TYPE_I8:
            case TYPE_I16:
            case TYPE_I32:
            case TYPE_I64:
                expression->number.tag = expected->type.tag;
                break;
            default:
                errorAt(&expression->number.position, "%s expected.\n", expected->name);
                break;
        }
    }
    else if (expression->number.tag == NUMBER_FLOAT_TO_BE_INFERRED)
    {
        switch (expected->type.tag)
        {
            case TYPE_F32:
            case TYPE_F64:
                expression->number.tag = expected->type.tag;
                break;
            default:
                errorAt(&expression->number.position, "%s expected.\n", expected->name);
                break;
        }
    }
}

Symbol* analyzeExpression(Scope* scope, Expression* expression)
{
    if (expression->tag == EXPRESSION_IDENTIFIER)
    {
        Symbol* symbol = resolve(scope, expression->identifier.value);
        if (symbol == NULL)
            errorAt(&expression->identifier.position, "Undefined reference to %s.", expression->identifier.value);
        else
            return symbol;
    }
    else if (expression->tag == EXPRESSION_CALL)
    {
        // Remember that there can be overloads.
        // Pass a list of input types to the resolve
        // function to be able to resolve the function call.

        Symbol* callee = resolve(scope, expression->call.name.value);
        if (callee == NULL)
            errorAt(&expression->call.name.position, "Call to undefined function %s.", &expression->call.name.value);
        else if (callee->type.tag != TYPE_FUNCTION)
            errorAt(&expression->call.name.position, "%s is not a function.", &expression->call.name.value);

        if (callee->type.function.output.count == 0)
            return NULL;
        else
            return &callee->type.function.output.elements[0];
    }
    else
    {
        switch (expression->number.tag)
        {
            case NUMBER_I8:
                return resolve(scope, i8);
            case NUMBER_I16:
                return resolve(scope, i16);
            case NUMBER_I32:
                return resolve(scope, i32);
            case NUMBER_I64:
                return resolve(scope, i64);
            case NUMBER_U8:
                return resolve(scope, u8);
            case NUMBER_U16:
                return resolve(scope, u16);
            case NUMBER_U32:
                return resolve(scope, u32);
            case NUMBER_U64:
                return resolve(scope, u64);
            case NUMBER_F32:
                return resolve(scope, f32);
            case NUMBER_F64:
                return resolve(scope, f64);
            default:
                break;
        }
    }

    return NULL;
}

void checkExpectedExpressionType(Expression* expression, Symbol* expected, Symbol* actual)
{
    if (!compareSymbols(expected, actual))
        errorAt(positionOfExpression(expression), "Expected %s found %s.", typeToString(&expected->type), typeToString(&actual->type));
}

void analyzeBlock(Scope* file, Scope* local, StatementList* body, Symbol* functionSymbol)
{
    for (int32_t i = 0; i < body->count; i++)
    {
        Statement* statement = &body->elements[i];
        switch (statement->tag)
        {
            case STATEMENT_ASSIGNMENT:
                for (int32_t v = 0; v < statement->assignment.variables.count; v++)
                {
                    Expression* variable = &statement->assignment.variables.elements[v];
                    if (variable->tag != EXPRESSION_IDENTIFIER)
                        error("Can only assign to variables");

                    Expression* value = &statement->assignment.values.elements[v];
                    if (value->tag == EXPRESSION_CTFE)
                    {
                        // Call an interpreter. The interpreter returns an
                        // ExpressionList Then check if the return expressions
                        // match in length and types. If the list of returned
                        // expreessions are longer than there are variables left
                        // mark the ctfe expression as an error.

                        ExpressionList result = interpret(file, local, value->expression);
                        statement->assignment.values.elements[v] = result.elements[0];
                    }

                    Symbol* variableSymbol = resolve(local, variable->identifier.value);
                    if (variableSymbol)
                    {
                        if (value->tag == EXPRESSION_NUMBER && (value->number.tag == NUMBER_INTEGER_TO_BE_INFERRED || value->number.tag == NUMBER_FLOAT_TO_BE_INFERRED))
                            inferNumberType(value, variableSymbol);

                        checkExpectedExpressionType(value, variableSymbol, analyzeExpression(local, value));
                        variable->tag = EXPRESSION_VARIABLE_REFERENCE;
                    }
                    else
                    {
                        Type typeOfValue;
                        if (value->tag == EXPRESSION_NUMBER)
                        {
                            if (value->number.tag == NUMBER_INTEGER_TO_BE_INFERRED)
                                errorAt(&value->number.position, "Integer type cannot be infered.");
                            else if (value->number.tag == NUMBER_FLOAT_TO_BE_INFERRED)
                                errorAt(&value->number.position, "Float type cannot be infered.");
                            else
                                typeOfValue = analyzeExpression(local, value)->type;
                        }
                        else if (value->tag == EXPRESSION_IDENTIFIER)
                        {
                            value->tag = EXPRESSION_VARIABLE_REFERENCE;
                            typeOfValue = resolve(local, value->identifier.value)->type;
                        }
                        else if (value->tag == EXPRESSION_CALL)
                        {
                            Symbol* symbol = analyzeExpression(local, value);
                            if (symbol == NULL)
                                errorAt(&value->call.name.position, "%s doesn't return a value.", value->call.name.value);

                            typeOfValue = analyzeExpression(local, value)->type;
                        }

                        variable->tag = EXPRESSION_VARIABLE_DECLARATION;
                        appendSymbol(&local->symbols, (Symbol)
                        {
                            .name = variable->identifier.value,
                            .type = typeOfValue
                        });
                    }
                }
                break;
            case STATEMENT_RETURN:
                for (int32_t x = 0; x < functionSymbol->type.function.output.count; x++)
                {
                    Symbol* expected = &functionSymbol->type.function.output.elements[x];
                    inferNumberType(&statement->value, expected);

                    checkExpectedExpressionType(&statement->value, expected, analyzeExpression(local, &statement->value));
                }
                break;
            case STATEMENT_EXPRESSION:
                analyzeExpression(local, &statement->value);
                break;
            default:
                error("Unsupported Statement type");
        }
    }
}

void analyzeFunction(Scope* file, FunctionDeclaration* declaration)
{
    Symbol* functionSymbol = resolve(file, declaration->name.value);
    declaration->scope.parent = file;

    analyzeBlock(file, &declaration->scope, &declaration->body, functionSymbol);
    declaration->analyzed = true;
}

void analyze(Program* program)
{
    putDeclarationsIntoScope(program);
    for (int32_t i = 0; i < program->files.count; i++)
    {
        File* file = &program->files.elements[i];
        for (int32_t f = 0; f < file->functions.count; f++)
            analyzeFunction(&file->scope, &file->functions.elements[f]);
    }
}
