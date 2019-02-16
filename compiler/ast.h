#ifndef AST_H
#define AST_H

#include <stdint.h>
#include <stdbool.h>
#include "list.h"
#include "diagnostics.h"

typedef struct
{
    const char* path;
    const char* code;
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
    bool isPublic;
    Slice identifier;
} FunctionSignature;

typedef union
{
    int i;
} Statement;

typedef struct
{
    Statement* elements;
    int32_t count;
    int32_t capacity;
} StatementList;

typedef struct
{
    FunctionSignature signature;
    StatementList body;
} FunctionDeclaration;

DECLARE_LIST(FunctionDeclaration)

typedef struct
{
    const Source* source;
    const SliceList* namespace;
    const SliceListList* uses;
    const FunctionDeclarationList* functions;
} CompilationUnit;

DECLARE_LIST(CompilationUnit)

typedef struct
{
    CompilationUnitList compilationUnits;
    DiagnosticList diagnostics;
} Program;

bool compareSlices(Slice* a, Slice* b);

#endif
