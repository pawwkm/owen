#include "runner.h"

static void missing_lhs_parenthesis(void)
{
    format_test_name("parsing/expressions/sizeof/missing_lhs_parenthesis");

    format_source_path(0, "parsing/expressions/sizeof/missing_lhs_parenthesis.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    sizeof b\n"
                          "end");

    SET_COMMAND_LINE("parsing/expressions/sizeof/missing_lhs_parenthesis.owen");
    format_expectation(&test.expected_error, "%s:5:12: ( expected.\n", test.source_paths[0]);
}

static void missing_rhs_parenthesis(void)
{
    format_test_name("parsing/expressions/sizeof/missing_rhs_parenthesis");

    format_source_path(0, "parsing/expressions/sizeof/missing_rhs_parenthesis.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    sizeof(T\n"
                          "end");

    SET_COMMAND_LINE("parsing/expressions/sizeof/missing_rhs_parenthesis.owen");
    format_expectation(&test.expected_error, "%s:6:1: ) expected.\n", test.source_paths[0]);
}

static void missing_type_or_expression(void)
{
    format_test_name("parsing/expressions/sizeof/missing_type_or_expression");

    format_source_path(0, "parsing/expressions/sizeof/missing_type_or_expression.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    sizeof(.)\n"
                          "end");

    SET_COMMAND_LINE("parsing/expressions/sizeof/missing_type_or_expression.owen");
    format_expectation(&test.expected_error, "%s:5:12: Type or expression expected.\n", test.source_paths[0]);
}

bool parsing_expressions_sizeof(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            missing_lhs_parenthesis();
            break;

        case 1:
            missing_rhs_parenthesis();
            break;

        case 2:
            missing_type_or_expression();
            break;

        default:
            return false;
    }

    return true;
}
