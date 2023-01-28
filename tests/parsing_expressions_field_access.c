#include "runner.h"

static void missing_field(void)
{
    format_test_name("parsing/expressions/field_access/missing_field");

    format_source_path(0, "parsing/expressions/field_access/missing_field.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    I32 a\n"
                          "end\n"
                          "\n"
                          "function a(A b)\n"
                          "    b.\n"
                          "end");

    SET_COMMAND_LINE("parsing/expressions/field_access/missing_field.owen");
    format_expectation(&test.expected_error, "%s:10:1: Field expected.\n", test.source_paths[0]);
}

bool parsing_expressions_field_access(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            missing_field();
            break;

        default:
            return false;
    }

    return true;
}
