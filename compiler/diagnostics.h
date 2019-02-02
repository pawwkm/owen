#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

typedef struct
{
    char* description;
    int32_t index;
} Diagnostic;

typedef struct
{
    Diagnostic* elements;
    int32_t count;
    int32_t capacity;
} DiagnosticList;

void appendDiagnostic(DiagnosticList* diagnostics, Diagnostic diagnostic);

#endif
