#include "runner.h"

static void missing_variable_type(void)
{
    format_test_name("parsing/statements/declaration/missing_variable_type");

    format_source_path(0, "parsing/statements/declaration/missing_variable_type.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function some_function()\n"
                          "    I32 a, b\n"
                          "end");

    SET_COMMAND_LINE("parsing/statements/declaration/missing_variable_type.owen");
    format_expectation(&test.expected_error, "%s:5:12: Variable type expected.\n", test.source_paths[0]);
}

static void missing_variable_name(void)
{
    format_test_name("parsing/statements/declaration/missing_variable_name");

    format_source_path(0, "parsing/statements/declaration/missing_variable_name.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function some_function()\n"
                          "    I32 = 1\n"
                          "end");

    SET_COMMAND_LINE("parsing/statements/declaration/missing_variable_name.owen");
    format_expectation(&test.expected_error, "%s:5:9: Variable name expected.\n", test.source_paths[0]);
}

static void missing_expression(void)
{
    format_test_name("parsing/statements/declaration/missing_expression");

    format_source_path(0, "parsing/statements/declaration/missing_expression.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    I32 b =\n"
                          "end");

    SET_COMMAND_LINE("parsing/statements/declaration/missing_expression.owen");
    format_expectation(&test.expected_error, "%s:6:1: Expression expected.\n", test.source_paths[0]);
}

bool parsing_statements_declaration(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            missing_variable_type();
            break;

        case 1:
            missing_variable_name();
            break;

        case 2:
            missing_expression();
            break;

        default:
            return false;
    }

    return true;
}
