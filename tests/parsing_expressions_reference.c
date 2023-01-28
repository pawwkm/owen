#include "runner.h"

static void missing_type(void)
{
    format_test_name("parsing/expressions/reference/missing_type");

    format_source_path(0, "parsing/expressions/reference/missing_type.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    b[T,\n"
                          "end");

    SET_COMMAND_LINE("parsing/expressions/reference/missing_type.owen");
    format_expectation(&test.expected_error, "%s:6:1: Type expected.\n", test.source_paths[0]);
}

static void missing_rhs_square_bracket(void)
{
    format_test_name("parsing/expressions/reference/missing_rhs_square_bracket");

    format_source_path(0, "parsing/expressions/reference/missing_rhs_square_bracket.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    b[T\n"
                          "end");

    SET_COMMAND_LINE("parsing/expressions/reference/missing_rhs_square_bracket.owen");
    format_expectation(&test.expected_error, "%s:6:1: ] expected.\n", test.source_paths[0]);
}


bool parsing_expressions_reference(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            missing_type();
            break;

        case 1:
            missing_rhs_square_bracket();
            break;

        default:
            return false;
    }

    return true;
}
