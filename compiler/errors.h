#pragma once
#include "parser.h"

void error(const char* format, ...);

void errorAt(Source* source, const char* format, ...);
