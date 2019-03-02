#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H
#include <stdbool.h>
#include "list.h"

typedef struct
{
    char* path;
    char* code;
    char* index;
} Position;

typedef struct
{
    const char* description;
    Position occurredAt;
} Diagnostic;

DECLARE_LIST(Diagnostic)

DiagnosticList initDiagnosticList();

#endif
