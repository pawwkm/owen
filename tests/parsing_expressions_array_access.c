#include "runner.h"

static void missing_expression(void)
{
    format_test_name("parsing/expressions/array_access/missing_expression");

    format_source_path(0, "parsing/expressions/array_access/missing_expression.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    b[\n"
                          "end");

    SET_COMMAND_LINE("parsing/expressions/array_access/missing_expression.owen");
    format_expectation(&test.expected_error, "%s:6:1: Expression expected.\n", test.source_paths[0]);
}

static void missing_rhs_square_bracket(void)
{
    format_test_name("parsing/expressions/array_access/missing_rhs_square_bracket");

    format_source_path(0, "parsing/expressions/array_access/missing_rhs_square_bracket.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    b[c\n"
                          "end");

    SET_COMMAND_LINE("parsing/expressions/array_access/missing_rhs_square_bracket.owen");
    format_expectation(&test.expected_error, "%s:6:1: ] expected.\n", test.source_paths[0]);
}

bool parsing_expressions_array_access(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            missing_expression();
            break;

        case 1:
            missing_rhs_square_bracket();
            break;
        default:
            return false;
    }

    return true;
}
