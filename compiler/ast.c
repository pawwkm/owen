#include <stdlib.h>
#include <stdint.h>
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
