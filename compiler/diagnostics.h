#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H
#include "list.h"

typedef struct
{
    const char* description;
    const char* code;
    const char* index;
} Diagnostic;

DECLARE_LIST(Diagnostic)

DiagnosticList initDiagnosticList();

#endif
