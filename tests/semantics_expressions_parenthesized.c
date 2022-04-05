#include "runner.h"

static void parenthesize_an_expression(void)
{
    format_test_name("semantics/expressions/parenthesized/parenthesize_an_expression");

    format_source_path(0, "semantics/expressions/parenthesized/parenthesize_an_expression.owen");
    format_source_file(0, "// Generated by " __FILE__ " " __FUNCSIG__ "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b) : I32\n"
                          "    return (b)\n"
                          "end\n");

    SET_COMMAND_LINE("semantics/expressions/parenthesized/parenthesize_an_expression.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file" NEW_LINE
                       "    path: semantics/expressions/parenthesized/parenthesize_an_expression.owen" NEW_LINE
                       "    namespace: Abc" NEW_LINE
                       "    function" NEW_LINE
                       "        name: a" NEW_LINE
                       "        formal_parameter" NEW_LINE
                       "            type: I32" NEW_LINE
                       "            name: b" NEW_LINE
                       "        return_type: I32" NEW_LINE
                       "        return_statement" NEW_LINE
                       "            parenthesized" NEW_LINE
                       "                type: I32" NEW_LINE
                       "                reference_expression" NEW_LINE
                       "                    type: I32" NEW_LINE
                       "                    name: b" NEW_LINE);
}

bool semantics_expressions_parenthesized(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            parenthesize_an_expression();
            break;

        default:
            return false;
    }

    return true;
}