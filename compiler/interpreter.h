#pragma once

#include "ast.h"

ExpressionList interpret(Scope* file, Scope* local, Expression* expression);
