#include <stdint.h>
#include <string.h>
#include "string.h"
#include "ast.h"

void expandBuilder(StringBuilder* builder, int32_t minimum)
{
    if (builder->count + minimum > builder->capacity)
    {
        while (builder->count + minimum > builder->capacity)
            builder->capacity = builder->capacity < 4 ? 4 : builder->capacity * 2;

        builder->string = (char*)realloc(builder->string, builder->capacity);
    }
}

void buildFromSlice(StringBuilder* builder, Slice* part)
{
    expandBuilder(builder, part->length);

    strncpy(builder->string + builder->count, part->start, part->length);
    builder->count += part->length;
}

void buildFromString(StringBuilder* builder, char* string)
{
    size_t length = strlen(string);
    expandBuilder(builder, length);

    strncpy(builder->string + builder->count, string, length);
    builder->count += length;
}

char* terminateBuilder(StringBuilder* builder)
{
    expandBuilder(builder, 1);
    builder->string[builder->count] = '\0';

    return builder->string;
}

char* internString(StringList* list, char* string)
{
    for (int32_t i = 0; i < list->count; i++)
    {
        if (!strcmp(list->elements[i], string))
            return list->elements[i];
    }

    appendString(list, string);

    return string;
}

char* internSlice(StringList* list, Slice* slice)
{
    for (int32_t i = 0; i < list->count; i++)
    {
        if (strlen(list->elements[i]) == slice->length && !strncmp(list->elements[i], slice->start, slice->length))
            return list->elements[i];
    }

    char* string = strndup(slice->start, slice->length);
    appendString(list, string);

    return string;
}

char* strndup(const char *s, size_t n)
{
    char* p = memchr(s, '\0', n);
    if (p != NULL)
        n = p - s;

    p = malloc(n + 1);
    if (p != NULL)
    {
        memcpy(p, s, n);
        p[n] = '\0';
    }

    return p;
}
