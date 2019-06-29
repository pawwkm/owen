#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define DECLARE_LIST(type)                        \
typedef struct                                    \
{                                                 \
    type* elements;                               \
    int32_t count;                                \
    int32_t capacity;                             \
} type##List;                                     \
void append##type(type##List* list, type element);

#define DECLARE_LIST_APPEND(type)                                                        \
void append##type(type##List* list, type element)                                        \
{                                                                                        \
    if (list->count == list->capacity)                                                   \
    {                                                                                    \
        list->capacity = list->capacity < 4 ? 4 : list->capacity * 2;                    \
        list->elements = (type *)realloc(list->elements, list->capacity * sizeof(type)); \
    }                                                                                    \
                                                                                         \
    list->elements[list->count++] = element;                                             \
}

#define NEW_LIST() { NULL, 0, 0 }

typedef struct
{
    char** elements;
    int32_t count;
    int32_t capacity;
} StringList;

void appendString(StringList* strings, char* string);

bool stringListContains(StringList* list, char* string);
