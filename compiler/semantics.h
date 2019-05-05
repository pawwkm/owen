#pragma once

#include "ast.h"

typedef enum
{
    TYPE_I8,
    TYPE_I16,
    TYPE_I32,
    TYPE_I64,
    TYPE_U8,
    TYPE_U16,
    TYPE_U32,
    TYPE_U64,
    TYPE_F32,
    TYPE_F64,
    TYPE_FUNCTION,
    TYPE_ENUMERATION,
    TYPE_STRUCTURE,
    TYPE_UNION
} Type;

typedef struct
{
    Type tag;
    union
    {
        Slice variable;
        struct
        {
            const char* namespace;
            FunctionDeclaration* declaration;
        } function;
    };
} Symbol;

DECLARE_LIST(Symbol)

SymbolList* findSymbolsWithName(SymbolList* symbols, Slice* name);

void semanticsTestSuite();
