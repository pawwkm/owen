#ifndef AST_H
#define AST_H

#include <stdint.h>
#include <stdbool.h>
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

typedef struct
{
    Slice* elements;
    int32_t count;
    int32_t capacity;
} QualifiedIdentifier;

typedef struct
{
    const Source* source;
    const QualifiedIdentifier* namespace;
} CompilationUnit;

typedef struct
{
    CompilationUnit* elements;
    int32_t count;
    int32_t capacity;
} CompilationUnitList;

typedef struct
{
    CompilationUnitList compilationUnits;
    DiagnosticList diagnostics;
} Program;

bool compareSlices(Slice* a, Slice* b);

void appendIdentifier(QualifiedIdentifier* qualifiedIdentifier, Slice identifier);

void appendCompilationUnit(CompilationUnitList* compilationUnits, CompilationUnit compilationUnit);

#endif
