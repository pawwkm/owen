#include "runner.h"

static void missing_expression(void)
{
    format_test_name("parsing/expressions/field_initializer/missing_expression");

    format_source_path(0, "parsing/expressions/field_initializer/missing_expression.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    { a = }\n"
                          "end");

    SET_COMMAND_LINE("parsing/expressions/field_initializer/missing_expression.owen");
    format_expectation(&test.expected_error, "%s:5:11: Expression expected.\n", test.source_paths[0]);
}

static void missing_initializer(void)
{
    format_test_name("parsing/expressions/field_initializer/missing_initializer");

    format_source_path(0, "parsing/expressions/field_initializer/missing_initializer.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    { a = a, }\n"
                          "end");

    SET_COMMAND_LINE("parsing/expressions/field_initializer/missing_initializer.owen");
    format_expectation(&test.expected_error, "%s:5:14: Field initializer expected.\n", test.source_paths[0]);
}

bool parsing_expressions_field_initializer(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            missing_expression();
            break;

        case 1:
            missing_initializer();
            break;

        default:
            return false;
    }

    return true;
}
