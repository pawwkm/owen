#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "diagnostics.h"

void appendDiagnostic(DiagnosticList* diagnostics, Diagnostic diagnostic)
{
    if (diagnostics->capacity < diagnostics->count + 1)
    {
        int32_t oldCapacity = diagnostics->capacity;
        diagnostics->capacity = diagnostics->capacity < 4 ? 4 : diagnostics->capacity * 2;
        diagnostics->elements = (Diagnostic*)realloc(diagnostics->elements, diagnostics->capacity);
    }

    diagnostics->elements[diagnostics->count] = diagnostic;
    diagnostics->count++;
}

DiagnosticList initDiagnosticList()
{
    return (DiagnosticList)
    {
        .elements = NULL,
        .count = 0,
        .capacity = 0
    };
}
