#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "ast.h"

typedef enum
{
    TYPE_FUNCTION
} Type;

typedef struct
{
    char* name;
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

void semanticsTestSuite();

#endif
