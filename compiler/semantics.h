#pragma once
#include <stdint.h>
#include "ast.h"

Symbol* resolve(Scope* scope, char* name);

char* typeToString(Type* type);

void analyze(Program* program);

void analyzeFunction(Scope* file, FunctionDeclaration* declaration);
