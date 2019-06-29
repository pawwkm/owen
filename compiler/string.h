#pragma once
#include <stdint.h>
#include "ast.h"

typedef struct
{
    char* string;
    int32_t count;
    int32_t capacity;
} StringBuilder;

void buildFromSlice(StringBuilder* builder, Slice* part);

void buildFromString(StringBuilder* builder, char* string);

char* terminateBuilder(StringBuilder* builder);

char* strndup(const char *s, size_t n);

char* internString(StringList* list, char* string);

char* internSlice(StringList* list, Slice* slice);
