#include "runner.h"

static void missing_rhs_parenthesis(void)
{
    format_test_name("parsing/expressions/call/missing_rhs_parenthesis");

    format_source_path(0, "parsing/expressions/call/missing_rhs_parenthesis.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    a(b\n"
                          "end");

    SET_COMMAND_LINE("parsing/expressions/call/missing_rhs_parenthesis.owen");
    format_expectation(&test.expected_error, "%s:6:1: ) expected.\n", test.source_paths[0]);
}

bool parsing_expressions_call(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            missing_rhs_parenthesis();
            break;

        default:
            return false;
    }

    return true;
}
