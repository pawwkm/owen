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
        NUMBER_INTEGER_TO_BE_INFERRED,
        NUMBER_F32,
        NUMBER_F64,
        NUMBER_FLOAT_TO_BE_INFERRED
    } tag;
    Slice value;
} Number;

typedef struct
{
    enum
    {
        EXPRESSION_NULL,
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

struct _FunctionDeclaration;
typedef struct
{
    Position declaredAt;
    enum
    {
        STATEMENT_UNRESOLVED_CALL,
        STATEMENT_CALL,
        STATEMENT_ASSIGNMENT
    } tag;

    union
    {
        Slice identifier;
        struct _FunctionDeclaration* function;
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
