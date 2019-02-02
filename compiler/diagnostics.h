#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

typedef struct
{
    const char* description;
    const char* code;
    const char* index;
} Diagnostic;

typedef struct
{
    Diagnostic* elements;
    int32_t count;
    int32_t capacity;
} DiagnosticList;

DiagnosticList initDiagnosticList();

void appendDiagnostic(DiagnosticList* diagnostics, Diagnostic diagnostic);

#endif
