#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "list.h"
#include "parser.h"
#include "semantics.h"
#include "callBinding.h"

DECLARE_LIST_APPEND(Symbol)

SymbolList* findSymbolsWithName(SymbolList* symbols, Slice* name)
{
    SymbolList* matches = calloc(1, sizeof(SymbolList));
    for (int32_t s = 0; s < symbols->count; s++)
    {
        bool isMatch = false;
        Symbol* symbol = &symbols->elements[s];

        switch (symbol->tag)
        {
            case TYPE_FUNCTION:
                isMatch = compareSlices(&symbol->function.declaration->signature.identifier, name);
                break;
            default:
                isMatch = compareSlices(&symbol->variable, name);
                break;
        }

        if (isMatch)
            appendSymbol(matches, *symbol);
    }

    return matches;
}

int32_t lengthOfQualifiedIdentifier(const SliceList* list)
{
    if (list == NULL)
        return 0;

    int32_t length = list->count;
    for (int32_t i = 0; i < list->count; i++)
        length += list->elements[i].length;

    return length;
}

Symbol symbolizeFunction(const char* namespace, FunctionDeclaration* function)
{
    int32_t sizeOfName = strlen(namespace) + function->signature.identifier.length + 1;
    char* name = malloc(sizeOfName);

    Slice* identifier = &function->signature.identifier;
    strncpy(name + sizeOfName, identifier->start, identifier->length);
    name[sizeOfName + identifier->length] = 0;

    Symbol symbol;
    symbol.tag = TYPE_FUNCTION;
    symbol.function.namespace = namespace;
    symbol.function.declaration = function;

    return symbol;
}

SymbolList* symbolizeDeclarations(Program* program)
{
    SymbolList* symbols = calloc(1, sizeof(SymbolList));
    for (int32_t u = 0; u < program->compilationUnits.count; u++)
    {
        CompilationUnit* unit = &program->compilationUnits.elements[u];
        for (int32_t f = 0; f < unit->functions.count; f++)
            appendSymbol(symbols, symbolizeFunction(unit->namespace, &unit->functions.elements[f]));
    }

    return symbols;
}

SymbolList* findAvailableReferences(SymbolList* master, CompilationUnit* unit)
{
    SymbolList* symbols = calloc(1, sizeof(SymbolList));
    for (int32_t s = 0; s < master->count; s++)
    {
        Symbol* symbol = &master->elements[s];
        switch (symbol->tag)
        {
            case TYPE_FUNCTION:
                if (!strcmp(symbol->function.namespace, unit->namespace))
                    appendSymbol(symbols, *symbol);
                else
                {
                    for (int32_t u = 0; u < unit->uses.count; u++)
                    {
                        if (symbol->function.declaration->signature.access == ACCESS_PUBLIC && !strcmp(symbol->function.namespace, unit->uses.elements[u]))
                            appendSymbol(symbols, *symbol);
                    }
                }
                break;
        }
    }

    return symbols;
}

void analyzeExpression(Expression* expression, SymbolList* symbols, SymbolList* context, DiagnosticList* diagnostics)
{
    switch (expression->tag)
    {
        case EXPRESSION_NUMBER:
            if (expression->number.tag == NUMBER_INTEGER_TO_BE_INFERRED)
            {
                if (context == NULL)
                {
                    // error cannot infer integer type
                    expression->number.tag = NUMBER_POISONED;
                }
                else if (context->count > 1)
                {
                    // error there are more than one
                }
                else if (context->count == 1)
                    expression->number.tag = context->elements[0].tag;
                else
                {
                    // error cannot infer integer type
                    expression->number.tag = NUMBER_POISONED;
                }
            }
            break;
    }
}

void analyzeBlock(StatementList* block, SymbolList* symbols, DiagnosticList* diagnostics)
{
    for (int32_t s = 0; s < block->count; s++)
    {
        Statement* statement = &block->elements[s];
        switch (statement->tag)
        {
            case STATEMENT_ASSIGNMENT:
                for (int32_t v = 0; v < statement->assignment.values.count; v++)
                {
                    // context is a really bad name since the symbols are also context?
                    SymbolList* context = NULL;
                    Expression* variable = NULL;
                    Expression* value = &statement->assignment.values.elements[v];

                    if (statement->assignment.variables.count >= v && statement->assignment.variables.elements[v].tag == EXPRESSION_IDENTIFIER)
                    {
                        variable = &statement->assignment.variables.elements[v];
                        context = findSymbolsWithName(symbols, &variable->identifier);
                    }

                    analyzeExpression(value, symbols, context, diagnostics);
                    if (variable != NULL)
                    {
                        if (context->count == 0)
                        {
                            variable->tag = EXPRESSION_VARIABLE_DECLARATION;
                            appendSymbol(symbols, (Symbol)
                            {
                                .tag = value->number.tag,
                                .variable = variable->identifier
                            });
                        }
                        else
                            variable->tag = EXPRESSION_VARIABLE_REFERENCE;
                    }
                }
                break;
            case STATEMENT_CALL:
                bindCall(&statement->call, symbols, diagnostics);
                break;
        }
    }
}

void analyze(Program* program)
{
    SymbolList* master = symbolizeDeclarations(program);
    for (int32_t u = 0; u < program->compilationUnits.count; u++)
    {
        CompilationUnit* unit = &program->compilationUnits.elements[u];
        SymbolList* unitSymbols = findAvailableReferences(master, unit);

        for (int32_t f = 0; f < unit->functions.count; f++)
            analyzeBlock(&unit->functions.elements[f].body, unitSymbols, &program->diagnostics);
    }
}

void callToUndefinedFunctionIssuesDiagnostic()
{
    char* code = "namespace Test\n"
                 "public function a\n"
                 "    b()\n"
                 "end";

    Source source =
    {
        .path = "main.owen",
        .code = code,
        .current = code
    };

    Program program = parse(&source, 1);
    analyze(&program);

    assert(program.diagnostics.count == 1);

    Diagnostic* diagnostic = &program.diagnostics.elements[0];
    assert(!strcmp("b is undefined.", diagnostic->description));
    assert(37 == diagnostic->occurredAt.index - code);
}

void ambiguousCallIssuesDiagnostic()
{
    char* callingCode = "namespace Test\n"
                        "public function a\n"
                        "    a()\n"
                        "end";

    char* code = "namespace Test\n"
                 "public function a\n"
                 "end";

    Source sources[] =
    {
        {
            .path = "first.owen",
            .code = callingCode,
            .current = callingCode
        },
        {
            .path = "second.owen",
            .code = code,
            .current = code
        }
    };

    Program program = parse(sources, 2);
    analyze(&program);

    assert(program.diagnostics.count == 1);

    Diagnostic* diagnostic = &program.diagnostics.elements[0];
    assert(!strcmp("a can be any of:\nfirst.owen:2:17\nsecond.owen:2:17", diagnostic->description));
    assert(37 == diagnostic->occurredAt.index - callingCode);
}

void unambiguousCallBindsToFunction()
{
    char* code = "namespace Test\n"
                 "public function a\n"
                 "    a()\n"
                 "end";

    Source source =
    {
        .path = "main.owen",
        .code = code,
        .current = code
    };

    Program program = parse(&source, 1);
    analyze(&program);

    assert(program.compilationUnits.count == 1);
    assert(program.compilationUnits.elements[0].functions.count == 1);

    FunctionDeclaration callee = program.compilationUnits.elements[0].functions.elements[0];
    Statement statement = callee.body.elements[0];

    assert(statement.tag == STATEMENT_CALL);
    assert(compareSlices(&callee.signature.identifier, &statement.call.callee->signature.identifier));
}

void cannotDeclareAnIntegerVariableWithoutSize()
{
    char* code = "namespace Abc\n"
                 "function main\n"
                 "    a = 0\n"
                 "end";

    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    Program program = parse(&source, 1);
    analyze(&program);

    assert(program.compilationUnits.count == 1);
    assert(program.compilationUnits.elements[0].functions.count == 1);

    FunctionDeclaration function = program.compilationUnits.elements[0].functions.elements[0];
    Statement declaration = function.body.elements[0];

    assert(declaration.tag == STATEMENT_ASSIGNMENT);
    assert(declaration.assignment.variables.count == 1);
    assert(declaration.assignment.values.count == 1);

    Expression variable = declaration.assignment.variables.elements[0];
    assert(variable.tag == EXPRESSION_VARIABLE_DECLARATION);

    Expression value = declaration.assignment.values.elements[0];
    assert(value.tag == EXPRESSION_NUMBER);
    assert(value.number.tag == NUMBER_POISONED);
}

void canAssignAnIntegerVariableWithoutSize()
{
    char* code = "namespace Abc\n"
                 "function main\n"
                 "    a = 0i32\n"
                 "    a = 3\n"
                 "end";

    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    Program program = parse(&source, 1);
    analyze(&program);

    assert(program.compilationUnits.count == 1);
    assert(program.compilationUnits.elements[0].functions.count == 1);

    FunctionDeclaration function = program.compilationUnits.elements[0].functions.elements[0];
    Statement declaration = function.body.elements[0];
    assert(declaration.tag == STATEMENT_ASSIGNMENT);
    assert(declaration.assignment.variables.count == 1);
    assert(declaration.assignment.values.count == 1);

    Expression variable = declaration.assignment.variables.elements[0];
    assert(variable.tag == EXPRESSION_VARIABLE_DECLARATION);

    Expression value = declaration.assignment.values.elements[0];
    assert(value.tag == EXPRESSION_NUMBER);
    assert(value.number.tag == NUMBER_I32);

    Statement assignment = function.body.elements[1];
    variable = assignment.assignment.variables.elements[0];
    assert(variable.tag == EXPRESSION_VARIABLE_REFERENCE);

    value = assignment.assignment.values.elements[0];
    assert(value.tag == EXPRESSION_NUMBER);
    assert(value.number.tag == NUMBER_I32);
}

void semanticsTestSuite()
{
    callToUndefinedFunctionIssuesDiagnostic();
    ambiguousCallIssuesDiagnostic();
    unambiguousCallBindsToFunction();

    cannotDeclareAnIntegerVariableWithoutSize();
    canAssignAnIntegerVariableWithoutSize();
}
