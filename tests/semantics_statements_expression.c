#include "runner.h"

static void unassigned_return_value(void)
{
    format_test_name("semantics/statements/expression/unassigned_return_value");

    format_source_path(0, "semantics/statements/expression/unassigned_return_value.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                           "namespace Abc\n"
                           "\n"
                           "function a() : I32\n"
                           "    return 0\n"
                           "end\n"
                           "\n"
                           "function b()\n"
                           "    a()\n"
                           "end");

    SET_COMMAND_LINE("semantics/statements/expression/unassigned_return_value.owen");

    format_expectation(&test.expected_error,
                       "%s:9:6: I32 is unassigned.\n",
                       test.source_paths[0]);
}

static void multiple_unassigned_return_values(void)
{
    format_test_name("semantics/statements/expression/multiple_unassigned_return_values");

    format_source_path(0, "semantics/statements/expression/multiple_unassigned_return_values.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                           "namespace Abc\n"
                           "\n"
                           "function a() : I32, I32\n"
                           "    return 0, 0\n"
                           "end\n"
                           "\n"
                           "function b()\n"
                           "    a()\n"
                           "end");

    SET_COMMAND_LINE("semantics/statements/expression/multiple_unassigned_return_values.owen");
    format_expectation(&test.expected_error,
                       "%s:9:6: I32, I32 are unassigned.\n",
                       test.source_paths[0]);
}

static void call_to_function_returning_no_values(void)
{
    format_test_name("semantics/statements/expression/call_to_function_returning_no_values");

    format_source_path(0, "semantics/statements/expression/call_to_function_returning_no_values.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                           "namespace Abc\n"
                           "\n"
                           "function a()\n"
                           "    a()\n"
                           "end\n"
                           "\n"
                           "function main() : I32\n"
                           "    return 0\n"
                           "end");

    SET_COMMAND_LINE("semantics/statements/expression/call_to_function_returning_no_values.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics,
                       "file\n"
                       "    path: semantics/statements/expression/call_to_function_returning_no_values.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        call_expression\n"
                       "            type: none\n"
                       "            reference_expression\n"
                       "                type: Function()\n"
                       "                name: a\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void monomorphic_call_in_polymorphic_function(void)
{
    format_test_name("semantics/statements/expression/monomorphic_call_in_polymorphic_function");

    format_source_path(0, "semantics/statements/expression/monomorphic_call_in_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "end\n"
                          "\n"
                          "function b[T]()\n"
                          "    a()\n"
                          "end\n"
                          "\n"
                          "function c()\n"
                          "    b[I32]()\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/expression/monomorphic_call_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics,
                       "file\n"
                       "    path: semantics/statements/expression/monomorphic_call_in_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "    function\n"
                       "        name: b\n"
                       "        formal_type_parameter: T\n"
                       "        call_expression\n"
                       "            reference_expression\n"
                       "                name: a\n"
                       "    function\n"
                       "        name: b\n"
                       "        actual_type_parameter: I32\n"
                       "        call_expression\n"
                       "            type: none\n"
                       "            reference_expression\n"
                       "                type: Function()\n"
                       "                name: a\n"
                       "    function\n"
                       "        name: c\n"
                       "        call_expression\n"
                       "            type: none\n"
                       "            reference_expression\n"
                       "                type: Function()\n"
                       "                name: b\n"
                       "                actual_type_parameter: I32\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void non_call_expression(void)
{
    format_test_name("semantics/statements/expression/non_call_expression");

    format_source_path(0, "semantics/statements/expression/non_call_expression.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                           "namespace Abc\n"
                           "\n"
                           "function a(I32 b)\n"
                           "    b\n"
                           "end");

    SET_COMMAND_LINE("semantics/statements/expression/non_call_expression.owen");
    format_expectation(&test.expected_error,
                       "%s:5:5: Function call expected.\n",
                       test.source_paths[0]);
}

static void polymorphic_call_in_polymorphic_function(void)
{
    format_test_name("semantics/statements/expression/polymorphic_call_in_polymorphic_function");

    format_source_path(0, "semantics/statements/expression/polymorphic_call_in_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T]()\n"
                          "end\n"
                          "\n"
                          "function b[T]()\n"
                          "    a[T]()\n"
                          "end\n"
                          "\n"
                          "function c()\n"
                          "    b[I32]()\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/expression/polymorphic_call_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics,
                       "file\n"
                       "    path: semantics/statements/expression/polymorphic_call_in_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "    function\n"
                       "        name: b\n"
                       "        formal_type_parameter: T\n"
                       "        call_expression\n"
                       "            reference_expression\n"
                       "                name: a\n"
                       "                actual_type_parameter: T\n"
                       "    function\n"
                       "        name: b\n"
                       "        actual_type_parameter: I32\n"
                       "        call_expression\n"
                       "            type: none\n"
                       "            reference_expression\n"
                       "                type: Function()\n"
                       "                name: a\n"
                       "                actual_type_parameter: I32\n"
                       "    function\n"
                       "        name: c\n"
                       "        call_expression\n"
                       "            type: none\n"
                       "            reference_expression\n"
                       "                type: Function()\n"
                       "                name: b\n"
                       "                actual_type_parameter: I32\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

bool semantics_statements_expression(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            unassigned_return_value();
            break;

        case 1:
            multiple_unassigned_return_values();
            break;

        case 2:
            call_to_function_returning_no_values();
            break;

        case 3:
            monomorphic_call_in_polymorphic_function();
            break;

        case 4:
            non_call_expression();
            break;

        case 5:
            polymorphic_call_in_polymorphic_function();
            break;

        default:
            return false;
    }

    return true;
}
