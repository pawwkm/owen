#pragma once
#include "semantics.h"

//void bindFunctionCalls(CompilationUnit* unit, SymbolList* symbols, DiagnosticList* diagnostics);

void bindCall(Call* call, SymbolList* symbols, DiagnosticList* diagnostics);
