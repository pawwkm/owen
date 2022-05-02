#include "runner.h"

static void outside_of_loop(void)
{
    format_test_name("semantics/statements/break/outside_of_loop");

    format_source_path(0, "semantics/statements/break/outside_of_loop.owen");
    format_source_file(0, "// Generated by " __FILE__ " " __FUNCSIG__ "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    break\n"
                          "end\n");

    SET_COMMAND_LINE("semantics/statements/break/outside_of_loop.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:5: No enclosing loop of which to break out of." NEW_LINE,
                       test.source_paths[0]);
}

static void inside_while_loop(void)
{
    format_test_name("semantics/statements/break/inside_while_loop");

    format_source_path(0, "semantics/statements/break/inside_while_loop.owen");
    format_source_file(0, "// Generated by " __FILE__ " " __FUNCSIG__ "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main() : I32\n"
                          "    while true\n"
                          "        break\n"
                          "    end\n"
                          "\n"
                          "    return 0\n"
                          "end\n");

    SET_COMMAND_LINE("semantics/statements/break/inside_while_loop.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file" NEW_LINE
                       "    path: semantics/statements/break/inside_while_loop.owen" NEW_LINE
                       "    namespace: Abc" NEW_LINE
                       "    function" NEW_LINE
                       "        name: main" NEW_LINE
                       "        return_type: I32" NEW_LINE
                       "        loop_statement" NEW_LINE
                       "            boolean_literal" NEW_LINE
                       "                type: Bool" NEW_LINE
                       "                value: true" NEW_LINE
                       "            break_statement" NEW_LINE
                       "        return_statement" NEW_LINE
                       "            integer_literal" NEW_LINE
                       "                type: I32" NEW_LINE
                       "                value: 0" NEW_LINE,
                       test.source_paths[0]);
}

static void inside_if_statement(void)
{
    format_test_name("semantics/statements/break/inside_if_statement");

    format_source_path(0, "semantics/statements/break/inside_if_statement.owen");
    format_source_file(0, "// Generated by " __FILE__ " " __FUNCSIG__ "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    if true\n"
                          "        break\n"
                          "    end\n"
                          "end\n");

    SET_COMMAND_LINE("semantics/statements/break/inside_if_statement.owen");
    format_expectation(&test.expected_error, 
                       "%s:6:9: No enclosing loop of which to break out of." NEW_LINE,
                       test.source_paths[0]);
}

static void inside_for_loop(void)
{
    format_test_name("semantics/statements/break/inside_for_loop");

    format_source_path(0, "semantics/statements/break/inside_for_loop.owen");
    format_source_file(0, "// Generated by " __FILE__ " " __FUNCSIG__ "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main() : I32\n"
                          "    for I32 i = 0; i < 2; i += 1\n"
                          "        break\n"
                          "    end\n"
                          "\n"
                          "    return 0\n"
                          "end\n");

    SET_COMMAND_LINE("semantics/statements/break/inside_for_loop.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file" NEW_LINE
                       "    path: semantics/statements/break/inside_for_loop.owen" NEW_LINE
                       "    namespace: Abc" NEW_LINE
                       "    function" NEW_LINE
                       "        name: main" NEW_LINE
                       "        return_type: I32" NEW_LINE
                       "        loop_statement" NEW_LINE
                       "            declaration_statement" NEW_LINE
                       "                variable" NEW_LINE
                       "                    type: I32" NEW_LINE
                       "                    name: i" NEW_LINE
                       "                integer_literal" NEW_LINE
                       "                    type: I32" NEW_LINE
                       "                    value: 0" NEW_LINE
                       "            binary" NEW_LINE
                       "                type: Bool" NEW_LINE
                       "                operator: <" NEW_LINE
                       "                reference_expression" NEW_LINE
                       "                    type: I32" NEW_LINE
                       "                    name: i" NEW_LINE
                       "                integer_literal" NEW_LINE
                       "                    type: I32" NEW_LINE
                       "                    value: 2" NEW_LINE
                       "            assignment_statement" NEW_LINE
                       "                operator: +=" NEW_LINE
                       "                reference_expression" NEW_LINE
                       "                    type: I32" NEW_LINE
                       "                    name: i" NEW_LINE
                       "                integer_literal" NEW_LINE
                       "                    type: I32" NEW_LINE
                       "                    value: 1" NEW_LINE
                       "            break_statement" NEW_LINE
                       "        return_statement" NEW_LINE
                       "            integer_literal" NEW_LINE
                       "                type: I32" NEW_LINE
                       "                value: 0" NEW_LINE,
                       test.source_paths[0]);
}

bool semantics_statements_break(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            outside_of_loop();
            break;
        
        case 1:
            inside_while_loop();
            break;

        case 2:
            inside_if_statement();
            break;

        case 3:
            inside_for_loop();
            break;

        default:
            return false;
    }

    return true;
}
