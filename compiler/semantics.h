#pragma once

#include "ast.h"

typedef enum
{
    TYPE_FUNCTION
} Type;

typedef struct
{
    const char* namespace;
    Type type;
    union
    {
        FunctionDeclaration* function;
    };
} Symbol;

typedef struct SymbolList
{
    struct SymbolList* parent;
    Symbol* elements;
    int32_t count;
    int32_t capacity;
} SymbolList;

void appendSymbol(SymbolList* list, Symbol element);

void semanticsTestSuite();
