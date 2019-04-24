#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "ast.h"

DECLARE_LIST_APPEND(Slice)
DECLARE_LIST_APPEND(SliceList)
DECLARE_LIST_APPEND(Statement)
DECLARE_LIST_APPEND(Expression)
DECLARE_LIST_APPEND(FunctionDeclaration)
DECLARE_LIST_APPEND(CompilationUnit)

bool compareSlices(Slice* a, Slice* b)
{
    return a->length == b->length &&
           !strncmp(a->start, b->start, a->length);
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
