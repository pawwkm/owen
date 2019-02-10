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
    QualifiedIdentifier* elements;
    int32_t count;
    int32_t capacity;
} QualifiedIdentifierList;

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

typedef struct
{
    FunctionDeclaration* elements;
    int32_t count;
    int32_t capacity;
} FunctionDeclarationList;

typedef struct
{
    const Source* source;
    const QualifiedIdentifier* namespace;
    const QualifiedIdentifierList* uses;
    const FunctionDeclarationList* functions;
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

void appendQualifiedIdentifier(QualifiedIdentifierList* list, QualifiedIdentifier qualifiedIdentifier);

void appendCompilationUnit(CompilationUnitList* compilationUnits, CompilationUnit compilationUnit);

void appendFunctionDeclaration(FunctionDeclarationList* functions, FunctionDeclaration function);

#endif
