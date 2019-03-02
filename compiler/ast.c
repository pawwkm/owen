#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "ast.h"

DECLARE_LIST_APPEND(Slice)
DECLARE_LIST_APPEND(SliceList)
DECLARE_LIST_APPEND(Statement)
DECLARE_LIST_APPEND(FunctionDeclaration)
DECLARE_LIST_APPEND(CompilationUnit)

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

Position positionOf(const Source* source)
{
    return (Position)
    {
        .path = source->path,
        .code = source->code,
        .index = source->current
    };
}
