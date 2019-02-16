#include <stdint.h>
#include <stdlib.h>

#include "diagnostics.h"
#include "list.h"

DECLARE_LIST_APPEND(Diagnostic)

DiagnosticList initDiagnosticList()
{
    return (DiagnosticList)
    {
        .elements = NULL,
        .count = 0,
        .capacity = 0
    };
}
