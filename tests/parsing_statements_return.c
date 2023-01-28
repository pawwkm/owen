#include "runner.h"

static void invalid_expression(void)
{
    format_test_name("parsing/statements/return/invalid_expression");

    format_source_path(0, "parsing/statements/return/invalid_expression.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a() : I32\n"
                          "    return )\n"
                          "end");

    SET_COMMAND_LINE("parsing/statements/return/invalid_expression.owen");
    format_expectation(&test.expected_error, "%s:5:12: Missing end keyword in function.\n", test.source_paths[0]);
}

bool parsing_statements_return(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            invalid_expression();
            break;

        default:
            return false;
    }

    return true;
}
