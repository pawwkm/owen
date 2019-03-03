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
        STATEMENT_CALL
    } type;

    union
    {
        Slice identifier;
        struct _FunctionDeclaration* function;;
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
