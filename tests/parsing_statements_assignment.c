#include "runner.h"

static void missing_first_rhs_expression(void)
{
    format_test_name("parsing/statements/assignment/missing_first_rhs_expression");

    format_source_path(0, "parsing/statements/assignment/missing_first_rhs_expression.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function some_function()\n"
                          "    a, b =\n"
                          "end");

    SET_COMMAND_LINE("parsing/statements/assignment/missing_first_rhs_expression.owen");
    format_expectation(&test.expected_error, "%s:6:1: Expression expected.\n", test.source_paths[0]);
}

static void missing_second_lhs_expression(void)
{
    format_test_name("parsing/statements/assignment/missing_second_lhs_expression");

    format_source_path(0, "parsing/statements/assignment/missing_second_lhs_expression.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function some_function()\n"
                          "    a, = 1, 2\n"
                          "end");

    SET_COMMAND_LINE("parsing/statements/assignment/missing_second_lhs_expression.owen");
    format_expectation(&test.expected_error, "%s:5:8: Expression expected.\n", test.source_paths[0]);
}

static void missing_second_rhs_expression(void)
{
    format_test_name("parsing/statements/assignment/missing_second_rhs_expression");

    format_source_path(0, "parsing/statements/assignment/missing_second_rhs_expression.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function some_function()\n"
                          "    a, b = 1,\n"
                          "end");

    SET_COMMAND_LINE("parsing/statements/assignment/missing_second_rhs_expression.owen");
    format_expectation(&test.expected_error, "%s:6:1: Expression expected.\n", test.source_paths[0]);
}

static void invalid_operator(void)
{
    format_test_name("parsing/statements/assignment/invalid_operator");

    format_source_path(0, "parsing/statements/assignment/invalid_operator.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function some_function()\n"
                          "    a, b B 1,\n"
                          "end");

    SET_COMMAND_LINE("parsing/statements/assignment/invalid_operator.owen");
    format_expectation(&test.expected_error, "%s:5:10: =, +=, -=, *=, /=, &=, |=, ^=, %%=, <<= or >>= expected.\n", test.source_paths[0]);
}

bool parsing_statements_assignment(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            missing_first_rhs_expression();
            break;

        case 1:
            missing_second_lhs_expression();
            break;

        case 2:
            missing_second_rhs_expression();
            break;

        case 3:
            invalid_operator();
            break;

        default:
            return false;
    }

    return true;
}
