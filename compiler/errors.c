#include <stdio.h>
#include <stdarg.h>
#include "parser.h"

void error(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("\n");
    exit(EXIT_FAILURE);
}

void errorAt(Source* source, const char* format, ...)
{
    int32_t line = 1;
    int32_t column = 1;

    for (char* c = source->contents; c < source->current; c++)
    {
        if (*c == '\n')
        {
            line++;
            column = 1;
        }
        else
            column++;
    }

    printf("%s:%i:%i: ", source->path, line, column);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("\n");
    exit(EXIT_FAILURE);
}
