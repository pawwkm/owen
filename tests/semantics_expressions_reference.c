#include "runner.h"

static void monomorphic_reference_in_polymorphic_function(void)
{
    format_test_name("semantics/expressions/reference/monomorphic_reference_in_polymorphic_function");

    format_source_path(0, "semantics/expressions/reference/monomorphic_reference_in_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T](I32 b) : I32\n"
                          "    return b\n"
                          "end\n"
                          "\n"
                          "function c(I32 d) : I32\n"
                          "    return a[I32](d)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/reference/monomorphic_reference_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/reference/monomorphic_reference_in_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: I32\n"
                       "            name: b\n"
                       "        return_type_reference: I32\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: b\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: c\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: d\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: Function(I32) : I32\n"
                       "                    name: a\n"
                       "                    actual_type_parameter: I32\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: d\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void polymorphic_reference_in_polymorphic_function(void)
{
    format_test_name("semantics/expressions/reference/polymorphic_reference_in_polymorphic_function");

    format_source_path(0, "semantics/expressions/reference/polymorphic_reference_in_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T](T b) : T\n"
                          "    return b\n"
                          "end\n"
                          "\n"
                          "function c(I32 d) : I32\n"
                          "    return a(d)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/reference/polymorphic_reference_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/reference/polymorphic_reference_in_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: T\n"
                       "            name: b\n"
                       "        return_type_reference: T\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: b\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: c\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: d\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: Function(I32) : I32\n"
                       "                    name: a\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: d\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void reference_a_parameter(void)
{
    format_test_name("semantics/expressions/reference/reference_a_parameter");

    format_source_path(0, "semantics/expressions/reference/reference_a_parameter.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(Function() b) : Function()\n"
                          "    return b\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/reference/reference_a_parameter.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/reference/reference_a_parameter.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_parameter\n"
                       "            type: Function()\n"
                       "            name: b\n"
                       "        return_type: Function()\n"
                       "        return_statement\n"
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

static void reference_a_variable(void)
{
    format_test_name("semantics/expressions/reference/reference_a_variable");

    format_source_path(0, "semantics/expressions/reference/reference_a_variable.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a() : I32\n"
                          "    I32 b = 0\n"
                          "    return b\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/reference/reference_a_variable.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/reference/reference_a_variable.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        return_type: I32\n"
                       "        declaration_statement\n"
                       "            variable\n"
                       "                type: I32\n"
                       "                name: b\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void undefined_reference(void)
{
    format_test_name("semantics/expressions/reference/undefined_reference");

    format_source_path(0, "semantics/expressions/reference/undefined_reference.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a() : I32\n"
                          "    return b\n"
                          "end\n");

    SET_COMMAND_LINE("semantics/expressions/reference/undefined_reference.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:12: b is undefined.\n",
                       test.source_paths[0]);
}

bool semantics_expressions_reference(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            monomorphic_reference_in_polymorphic_function();
            break;

        case 1:
            polymorphic_reference_in_polymorphic_function();
            break;

        case 2:
            reference_a_parameter();
            break;

        case 3:
            reference_a_variable();
            break;

        case 4:
            undefined_reference();
            break;

        default:
            return false;
    }

    return true;
}
