#include <stdint.h>
#include <string.h>
#include "semantics.h"
#include "diagnostics.h"

char* itoa(int32_t number)
{
    char buffer[12];
    memset(buffer, 0, 12);

    int8_t index = 0;
    while (number)
    {
        buffer[index++] = number % 10 + '0';
        number /= 10;
    }

    size_t length = strlen(buffer);
    for (uint8_t i = 0, j = length - 1; i < j; i++, j--)
    {
        char c = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = c;
    }

    char* string = malloc(length);
    strcpy(string, buffer);

    return string;
}

char* concatenate(char* a, char* b)
{
    size_t sizeOfA = strlen(a);
    size_t sizeOfB = strlen(b);

    char* string = malloc(sizeOfA + sizeOfB + 1);
    string[sizeOfA + sizeOfB] = 0;

    strncpy(string, a, sizeOfA);
    strncpy(string + sizeOfA, b, sizeOfB);

    return string;
}

char* positionAsString(const Position* position)
{
    int32_t line = 1;
    int32_t column = 1;

    for (char* c = position->code; c < position->index; c++)
    {
        if (*c == '\n')
        {
            line++;
            column = 1;
        }
        else
            column++;
    }

    char* string = NULL;
    string = concatenate(position->path, ":");
    string = concatenate(string, itoa(line));
    string = concatenate(string, ":");
    string = concatenate(string, itoa(column));

    return string;
}

void bindCall(Call* call, SymbolList* symbols, DiagnosticList* diagnostics)
{
    SymbolList* matches = findSymbolsWithName(symbols, &call->identifier);
    if (matches->count == 0)
    {
        char* description = malloc(call->identifier.length + 1);
        description[call->identifier.length] = 0;
        strncpy(description, call->identifier.start, call->identifier.length);
        description = concatenate(description, " is undefined.");

        appendDiagnostic(diagnostics, (Diagnostic)
        {
            .description = description,
            .occurredAt = call->declaredAt
        });
    }
    else if (matches->count > 1)
    {
        char* description = malloc(call->identifier.length + 1);
        description[call->identifier.length] = 0;
        strncpy(description, call->identifier.start, call->identifier.length);

        description = concatenate(description, " can be any of:");
        for (size_t i = 0; i < matches->count; i++)
        {
            Symbol* symbol = &matches->elements[i];

            description = concatenate(description, "\n");
            description = concatenate(description, positionAsString(&symbol->function.declaration->declaredAt));
        }

        appendDiagnostic(diagnostics, (Diagnostic)
        {
            .description = description,
            .occurredAt = call->declaredAt
        });
    }
    else
    {
        call->callee = matches->elements[0].function.declaration;
    }
}
