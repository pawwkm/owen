#include "runner.h"

static void missing_explicit_index(void)
{
    format_test_name("parsing/expressions/array_initializer/missing_explicit_index");

    format_source_path(0, "parsing/expressions/array_initializer/missing_explicit_index.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    { [] }\n"
                          "end");

    SET_COMMAND_LINE("parsing/expressions/array_initializer/missing_explicit_index.owen");
    format_expectation(&test.expected_error, "%s:5:8: Expression expected.\n", test.source_paths[0]);
}

static void missing_equal(void)
{
    format_test_name("parsing/expressions/array_initializer/missing_equal");

    format_source_path(0, "parsing/expressions/array_initializer/missing_equal.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    { [0] }\n"
                          "end");

    SET_COMMAND_LINE("parsing/expressions/array_initializer/missing_equal.owen");
    format_expectation(&test.expected_error, "%s:5:11: = expected in element initializer.\n", test.source_paths[0]);
}

static void missing_expression(void)
{
    format_test_name("parsing/expressions/array_initializer/missing_expression");

    format_source_path(0, "parsing/expressions/array_initializer/missing_expression.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    { [0] = }\n"
                          "end");

    SET_COMMAND_LINE("parsing/expressions/array_initializer/missing_expression.owen");
    format_expectation(&test.expected_error, "%s:5:13: Expression expected.\n", test.source_paths[0]);
}

static void missing_initializer(void)
{
    format_test_name("parsing/expressions/array_initializer/missing_initializer");

    format_source_path(0, "parsing/expressions/array_initializer/missing_initializer.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    { a, }\n"
                          "end");

    SET_COMMAND_LINE("parsing/expressions/array_initializer/missing_initializer.owen");
    format_expectation(&test.expected_error, "%s:5:10: Element initializer expected.\n", test.source_paths[0]);
}

bool parsing_expressions_array_initializer(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            missing_explicit_index();
            break;
        
        case 1:
            missing_equal();
            break;

        case 2:
            missing_expression();
            break;

        case 3:
            missing_initializer();
            break;

        default:
            return false;
    }

    return true;
}
