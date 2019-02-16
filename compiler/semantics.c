#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "parser.h"
#include "semantics.h"

Symbol* findSymbol(SymbolList* list, char* name)
{
    for (int32_t i = 0; i < list->count; i++)
    {
        Symbol* symbol = &list->elements[i];
        if (!strcmp(symbol->name, name))
            return symbol;
    }

    if (list->parent != NULL)
        return findSymbol(list->parent, name);
    else
        return NULL;
}

void analyze(Program* program)
{
    SymbolList* symbols = calloc(1, sizeof(SymbolList));
    for (int32_t u = 0; u < program->compilationUnits.count; u++)
    {
        CompilationUnit* unit = &program->compilationUnits.elements[u];
        for (int32_t f = 0; f < unit->functions->count; f++)
        {

        }
    }
}

void lol()
{
    char* code = "function a\n"
                  "end";

    Source sources[] =
    {
        {
            .path = "first.owen",
            .code = code,
            .current = code
        },
        {
            .path = "second.owen",
            .code = code,
            .current = code
        }
    };

    Program program = parse(sources, 2);
    analyze(&program);
}

void semanticsTestSuite()
{
    lol();
}
