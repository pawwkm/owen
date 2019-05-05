#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "list.h"
#include "diagnostics.h"

typedef struct
{
    char* path;
    char* code;
    char* current;
} Source;

typedef struct
{
    char* start;
    int32_t length;
} Slice;

DECLARE_LIST(Slice)
DECLARE_LIST(SliceList)

typedef struct
{
    enum
    {
        NUMBER_I8,
        NUMBER_I16,
        NUMBER_I32,
        NUMBER_I64,
        NUMBER_U8,
        NUMBER_U16,
        NUMBER_U32,
        NUMBER_U64,
        NUMBER_F32,
        NUMBER_F64,
        NUMBER_FLOAT_TO_BE_INFERRED,
        NUMBER_INTEGER_TO_BE_INFERRED,
        NUMBER_POISONED
    } tag;
    Slice value;
} Number;

typedef struct
{
    enum
    {
        EXPRESSION_NULL,
        EXPRESSION_CALL,
        EXPRESSION_NUMBER,
        EXPRESSION_IDENTIFIER,
        EXPRESSION_VARIABLE_DECLARATION,
        EXPRESSION_VARIABLE_REFERENCE
    } tag;
    union
    {
        Slice identifier;
        Number number;
    };
} Expression;

DECLARE_LIST(Expression)
struct _FunctionDeclaration;

typedef struct
{
    Position declaredAt;
    Slice identifier;
    ExpressionList input;

    struct _FunctionDeclaration* callee;
} Call;

typedef enum
{
    ACCESS_PUBLIC,
    ACCESS_PRIVATE
} Access;

typedef struct
{
    Access access;
    Slice identifier;
} FunctionSignature;

typedef struct
{
    enum
    {
        STATEMENT_CALL,
        STATEMENT_ASSIGNMENT
    } tag;

    union
    {
        Call call;
        struct
        {
            ExpressionList variables;
            ExpressionList values;
        } assignment;
    };
} Statement;

DECLARE_LIST(Statement)

typedef struct _FunctionDeclaration
{
    Position declaredAt;
    FunctionSignature signature;
    StatementList body;
} FunctionDeclaration;

DECLARE_LIST(FunctionDeclaration)

typedef struct
{
    const Source* source;
    const char* namespace;
    StringList uses;
    FunctionDeclarationList functions;
} CompilationUnit;

DECLARE_LIST(CompilationUnit)

typedef struct
{
    CompilationUnitList compilationUnits;
    DiagnosticList diagnostics;
} Program;

bool compareSlices(Slice* a, Slice* b);

Position positionOf(const Source* source);
