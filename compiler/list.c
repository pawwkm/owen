#include <stdlib.h>
#include "list.h"

void appendString(StringList* strings, char* string)
{
    if (strings->count == strings->capacity)
    {
        strings->capacity = strings->capacity < 4 ? 4 : strings->capacity * 2;
        strings->elements = (char**)realloc(strings->elements, strings->capacity * sizeof(char*));
    }

    strings->elements[strings->count++] = string;
}
