#include "runner.h"

static void missing_rhs(void)
{
    format_test_name("parsing/expressions/binary/missing_rhs");

    format_source_path(0, "parsing/expressions/binary/missing_rhs.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    a +\n"
                          "end");

    SET_COMMAND_LINE("parsing/expressions/binary/missing_rhs.owen");
    format_expectation(&test.expected_error, "%s:6:1: Expression expected.\n", test.source_paths[0]);
}

bool parsing_expressions_binary(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            missing_rhs();
            break;

        default:
            return false;
    }

    return true;
}
