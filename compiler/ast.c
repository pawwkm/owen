#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "ast.h"

void appendCompilationUnit(CompilationUnitList* compilationUnits, CompilationUnit compilationUnit)
{
    if (compilationUnits->capacity < compilationUnits->count + 1)
    {
        int32_t oldCapacity = compilationUnits->capacity;
        compilationUnits->capacity = compilationUnits->capacity < 4 ? 4 : compilationUnits->capacity * 2;
        compilationUnits->elements = (CompilationUnit*)realloc(compilationUnits->elements, compilationUnits->capacity);
    }

    compilationUnits->elements[compilationUnits->count] = compilationUnit;
    compilationUnits->count++;
}

void appendIdentifier(QualifiedIdentifier* qualifiedIdentifier, Slice identifier)
{
    if (qualifiedIdentifier->capacity < qualifiedIdentifier->count + 1)
    {
        int32_t oldCapacity = qualifiedIdentifier->capacity;
        qualifiedIdentifier->capacity = qualifiedIdentifier->capacity < 4 ? 4 : qualifiedIdentifier->capacity * 2;
        qualifiedIdentifier->elements = (Slice*)realloc(qualifiedIdentifier->elements, qualifiedIdentifier->capacity);
    }

    qualifiedIdentifier->elements[qualifiedIdentifier->count] = identifier;
    qualifiedIdentifier->count++;
}

void appendQualifiedIdentifier(QualifiedIdentifierList* list, QualifiedIdentifier qualifiedIdentifier)
{
    if (list->capacity < list->count + 1)
    {
        int32_t oldCapacity = list->capacity;
        list->capacity = list->capacity < 4 ? 4 : list->capacity * 2;
        list->elements = (QualifiedIdentifier*)realloc(list->elements, list->capacity);
    }

    list->elements[list->count] = qualifiedIdentifier;
    list->count++;
}

void appendFunctionDeclaration(FunctionDeclarationList* functions, FunctionDeclaration function)
{
    if (functions->capacity < functions->count + 1)
    {
        int32_t oldCapacity = functions->capacity;
        functions->capacity = functions->capacity < 4 ? 4 : functions->capacity * 2;
        functions->elements = (FunctionDeclaration*)realloc(functions->elements, functions->capacity);
    }

    functions->elements[functions->count] = function;
    functions->count++;
}

bool compareSlices(Slice* a, Slice* b)
{
    if (a->length != b->length)
        return false;

    for (int32_t i = 0; i < a->length; i++)
    {
        if (a->start[i] != b->start[i])
            return false;
    }

    return true;
}
