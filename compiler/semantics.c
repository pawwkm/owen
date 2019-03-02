#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "list.h"
#include "parser.h"
#include "semantics.h"
#include "callBinding.h"

DECLARE_LIST_APPEND(Symbol)

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

    return (Symbol)
    {
        .namespace = namespace,
        .type = TYPE_FUNCTION,
        .function = function
    };
}

SymbolList* symbolizeDeclarations(Program* program)
{
    SymbolList* symbols = calloc(1, sizeof(SymbolList));
    for (int32_t u = 0; u < program->compilationUnits.count; u++)
    {
        CompilationUnit* unit = &program->compilationUnits.elements[u];
        for (int32_t f = 0; f < unit->functions->count; f++)
            appendSymbol(symbols, symbolizeFunction(unit->namespace, &unit->functions->elements[f]));
    }

    return symbols;
}

SymbolList* findAvailableReferences(SymbolList* master, CompilationUnit* unit)
{
    SymbolList* symbols = calloc(1, sizeof(SymbolList));
    for (int32_t s = 0; s < master->count; s++)
    {
        Symbol* symbol = &master->elements[s];
        if (!strcmp(symbol->namespace, unit->namespace))
            appendSymbol(symbols, *symbol);
        else if (unit->uses)
        {
            for (int32_t u = 0; u < unit->uses->count; u++)
            {
                if (symbol->function->signature.isPublic && !strcmp(symbol->namespace, unit->uses->elements[u]))
                    appendSymbol(symbols, *symbol);
            }
        }
    }

    return symbols;
}

void analyze(Program* program)
{
    if (program->diagnostics.count)
        return;

    SymbolList* master = symbolizeDeclarations(program);
    for (int32_t u = 0; u < program->compilationUnits.count; u++)
    {
        CompilationUnit* unit = &program->compilationUnits.elements[u];
        SymbolList* unitSymbols = findAvailableReferences(master, unit);

        bindFunctionCalls(unit, unitSymbols, &program->diagnostics);
    }
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
    assert(!strcmp("a can be any of:\nfirst.owen:2:1\nsecond.owen:2:1", diagnostic->description));
    assert(37 == diagnostic->occurredAt.index - callingCode);
}

void semanticsTestSuite()
{
    ambiguousCallIssuesDiagnostic();
}
