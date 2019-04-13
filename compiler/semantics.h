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

DECLARE_LIST(Symbol)

void semanticsTestSuite();
