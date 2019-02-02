#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

Program parse(Source* sources, int length);

void parserTestSuite();

#endif
