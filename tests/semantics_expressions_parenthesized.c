#include "runner.h"

static void parenthesize_an_expression(void)
{
    format_test_name("semantics/expressions/parenthesized/parenthesize_an_expression");

    format_source_path(0, "semantics/expressions/parenthesized/parenthesize_an_expression.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b) : I32\n"
                          "    return (b)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/parenthesized/parenthesize_an_expression.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/parenthesized/parenthesize_an_expression.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: b\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            parenthesized\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: b\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

bool semantics_expressions_parenthesized(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            parenthesize_an_expression();
            break;

        default:
            return false;
    }

    return true;
}
