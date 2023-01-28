#include "runner.h"

static void no_parameters_or_return_types(void)
{
    format_test_name("semantics/expressions/call/function_typed/no_parameters_or_return_types");

    format_source_path(0, "semantics/expressions/call/function_typed/no_parameters_or_return_types.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(Function() b)\n"
                          "    b()\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/function_typed/no_parameters_or_return_types.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/call/function_typed/no_parameters_or_return_types.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_parameter\n"
                       "            type: Function()\n"
                       "            name: b\n"
                       "        call_expression\n"
                       "            type: none\n"
                       "            reference_expression\n"
                       "                type: Function()\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void matching_number_of_parameters_but_wrong_type(void)
{
    format_test_name("semantics/expressions/call/function_typed/matching_number_of_parameters_but_wrong_type");

    format_source_path(0, "semantics/expressions/call/function_typed/matching_number_of_parameters_but_wrong_type.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(Function(I8) b, I32 c)\n"
                          "    b(c)\n"
                          "end\n");

    SET_COMMAND_LINE("semantics/expressions/call/function_typed/matching_number_of_parameters_but_wrong_type.owen");

    format_expectation(&test.expected_error, 
                       "%s:5:7: I8 expected but found I32.\n",
                       test.source_paths[0]);
}

static void matching_number_of_parameters_and_types(void)
{
    format_test_name("semantics/expressions/call/function_typed/matching_number_of_parameters_and_types");

    format_source_path(0, "semantics/expressions/call/function_typed/matching_number_of_parameters_and_types.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(Function(I32) b, I32 c)\n"
                          "    b(c)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/function_typed/matching_number_of_parameters_and_types.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/call/function_typed/matching_number_of_parameters_and_types.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_parameter\n"
                       "            type: Function(I32)\n"
                       "            name: b\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: c\n"
                       "        call_expression\n"
                       "            type: none\n"
                       "            reference_expression\n"
                       "                type: Function(I32)\n"
                       "                name: b\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: c\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

bool semantics_expressions_call_function_typed(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            no_parameters_or_return_types();
            break;

        case 1:
            matching_number_of_parameters_but_wrong_type();
            break;

        case 2:
            matching_number_of_parameters_and_types();
            break;

        default:
            return false;
    }

    return true;
}
