#include "runner.h"

static void outside_of_loop(void)
{
    format_test_name("semantics/statements/break/outside_of_loop");

    format_source_path(0, "semantics/statements/break/outside_of_loop.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    break\n"
                          "end\n");

    SET_COMMAND_LINE("semantics/statements/break/outside_of_loop.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:5: No enclosing loop of which to break out of.\n",
                       test.source_paths[0]);
}

static void inside_while_loop(void)
{
    format_test_name("semantics/statements/break/inside_while_loop");

    format_source_path(0, "semantics/statements/break/inside_while_loop.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
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
                       "file\n"
                       "    path: semantics/statements/break/inside_while_loop.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        loop_statement\n"
                       "            boolean_literal\n"
                       "                type: Bool\n"
                       "                value: true\n"
                       "            break_statement\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n",
                       test.source_paths[0]);
}

static void inside_if_statement(void)
{
    format_test_name("semantics/statements/break/inside_if_statement");

    format_source_path(0, "semantics/statements/break/inside_if_statement.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    if true\n"
                          "        break\n"
                          "    end\n"
                          "end\n");

    SET_COMMAND_LINE("semantics/statements/break/inside_if_statement.owen");
    format_expectation(&test.expected_error, 
                       "%s:6:9: No enclosing loop of which to break out of.\n",
                       test.source_paths[0]);
}

static void inside_for_loop(void)
{
    format_test_name("semantics/statements/break/inside_for_loop");

    format_source_path(0, "semantics/statements/break/inside_for_loop.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
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
                       "file\n"
                       "    path: semantics/statements/break/inside_for_loop.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        loop_statement\n"
                       "            declaration_statement\n"
                       "                variable\n"
                       "                    type: I32\n"
                       "                    name: i\n"
                       "                integer_literal\n"
                       "                    type: I32\n"
                       "                    value: 0\n"
                       "            binary\n"
                       "                type: Bool\n"
                       "                operator: <\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: i\n"
                       "                integer_literal\n"
                       "                    type: I32\n"
                       "                    value: 2\n"
                       "            assignment_statement\n"
                       "                operator: +=\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: i\n"
                       "                integer_literal\n"
                       "                    type: I32\n"
                       "                    value: 1\n"
                       "            break_statement\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n",
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
