#include "runner.h"

static void actual_type_parameters_make_signature_match_monomorphic_function(void)
{
    format_test_name("semantics/expressions/call/implicit_actual_type_parameters/actual_type_parameters_make_signature_match_monomorphic_function");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b) : I32\n"
                          "    return b\n"
                          "end\n"
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

    SET_COMMAND_LINE("semantics/expressions/call/implicit_actual_type_parameters/actual_type_parameters_make_signature_match_monomorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: %s\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: b\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: b\n"
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
                       "                value: 0\n",
                       test.source_paths[0]);
}

static void call_reference_that_is_not_a_function(void)
{
    format_test_name("semantics/expressions/call/implicit_actual_type_parameters/call_reference_that_is_not_a_function");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b, I32 c)\n"
                          "    b(c)\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/implicit_actual_type_parameters/call_reference_that_is_not_a_function.owen");

    format_expectation(&test.expected_error, 
                       "%s:5:5: Function type expected but found I32.\n",
                       test.source_paths[0]);
}

static void error_nested_in_a_polymorphic_function_call(void)
{
    format_test_name("semantics/expressions/call/implicit_actual_type_parameters/error_nested_in_a_polymorphic_function_call");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration Direction : U8\n"
                          "    up\n"
                          "    right\n"
                          "    down\n"
                          "    left\n"
                          "end\n"
                          "\n"
                          "function do_stuff() : Direction\n"   
                          "    return do_stuff(Direction.up, Direction.down)\n"
                          "end\n"
                          "\n"
                          "function do_stuff[T](T a, T b) : T\n"
                          "    return a + b\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/implicit_actual_type_parameters/error_nested_in_a_polymorphic_function_call.owen");

    format_expectation(&test.expected_error, 
                       "semantics/expressions/call/implicit_actual_type_parameters/error_nested_in_a_polymorphic_function_call.owen:16:14: Operator not defined for Direction and Direction.\n"
                       "Polymorphic stack:\n"
                       "    semantics/expressions/call/implicit_actual_type_parameters/error_nested_in_a_polymorphic_function_call.owen:12:20: do_stuff[Direction](Direction a, Direction b) : Direction\n");
}

static void error_nested_in_multiple_polymorphic_function_calls(void)
{
    format_test_name("semantics/expressions/call/implicit_actual_type_parameters/error_nested_in_multiple_polymorphic_function_calls");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration Direction : U8\n"
                          "    up\n"
                          "    right\n"
                          "    down\n"
                          "    left\n"
                          "end\n"
                          "\n"
                          "function do_stuff() : Direction\n"
                          "    return do_stuff(Direction.up)\n"
                          "end\n"
                          "\n"
                          "function do_stuff[T](T a) : T\n"
                          "    return do_stuff(a, a)\n"
                          "end\n"
                          "\n"
                          "function do_stuff[T](T a, T b) : T\n"
                          "    return a + b\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/implicit_actual_type_parameters/error_nested_in_multiple_polymorphic_function_calls.owen");

    format_expectation(&test.expected_error, 
                       "semantics/expressions/call/implicit_actual_type_parameters/error_nested_in_multiple_polymorphic_function_calls.owen:20:14: Operator not defined for Direction and Direction.\n"
                       "Polymorphic stack:\n"
                       "    semantics/expressions/call/implicit_actual_type_parameters/error_nested_in_multiple_polymorphic_function_calls.owen:12:20: do_stuff[Direction](Direction a) : Direction\n"
                       "    semantics/expressions/call/implicit_actual_type_parameters/error_nested_in_multiple_polymorphic_function_calls.owen:16:20: do_stuff[Direction](Direction a, Direction b) : Direction\n");
}

static void formal_type_parameter_nested_in_compound(void)
{
    format_test_name("semantics/expressions/call/implicit_actual_type_parameters/formal_type_parameter_nested_in_compound");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A[T]\n"
                          "    T t\n"
                          "end\n"
                          "\n"
                          "function a[T](A[T] b) : T\n"
                          "    return b.t\n"
                          "end\n"
                          "\n"
                          "function c(A[I32] d) : I32\n"
                          "    return a(d)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/implicit_actual_type_parameters/formal_type_parameter_nested_in_compound.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/call/implicit_actual_type_parameters/formal_type_parameter_nested_in_compound.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        formal_type_parameter: T\n"
                       "        field\n"
                       "            type_reference: T\n"
                       "            name: t\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        actual_type_parameter: I32\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: t\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: A[T]\n"
                       "            name: b\n"
                       "        return_type_reference: T\n"
                       "        return_statement\n"
                       "            field_access\n"
                       "                reference_expression\n"
                       "                    name: b\n"
                       "                field: t\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: A[I32]\n"
                       "            name: b\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            field_access\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: A[I32]\n"
                       "                    name: b\n"
                       "                field: t\n"
                       "    function\n"
                       "        name: c\n"
                       "        formal_parameter\n"
                       "            type: A[I32]\n"
                       "            name: d\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: Function(A[I32]) : I32\n"
                       "                    name: a\n"
                       "                reference_expression\n"
                       "                    type: A[I32]\n"
                       "                    name: d\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void formal_type_parameter_not_specified_by_actual_parameters(void)
{
    format_test_name("semantics/expressions/call/implicit_actual_type_parameters/formal_type_parameter_not_specified_by_actual_parameters");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T1, T2](T1 b) : T2\n"
                          "    return b + 2\n"
                          "end\n"
                          "\n"
                          "function c(I32 d) : I32\n"
                          "    return a(d)\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/implicit_actual_type_parameters/formal_type_parameter_not_specified_by_actual_parameters.owen");

    format_expectation(&test.expected_error, 
                       "%s:9:13: T2 could not be inferred from the call site. Specify it explicitly.\n",
                       test.source_paths[0]);
}

static void formal_type_parameter_redefined_by_actual_parameter(void)
{
    format_test_name("semantics/expressions/call/implicit_actual_type_parameters/formal_type_parameter_redefined_by_actual_parameter");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T](T b, T c) : T\n"
                          "    return b + c\n"
                          "end\n"
                          "\n"
                          "function d(I32 e, F32 f) : I32\n"
                          "    return a(e, f)\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/implicit_actual_type_parameters/formal_type_parameter_redefined_by_actual_parameter.owen");

    format_expectation(&test.expected_error, 
                       "%s:9:17: T redefined as F32. It was originally I32.\n",
                       test.source_paths[0]);
}

static void formal_type_parameters_is_actual_type_parameter(void)
{
    format_test_name("semantics/expressions/call/implicit_actual_type_parameters/formal_type_parameters_is_actual_type_parameter");

    format_source_path(0, "%s.owen", test.name);
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

    SET_COMMAND_LINE("semantics/expressions/call/implicit_actual_type_parameters/formal_type_parameters_is_actual_type_parameter.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/call/implicit_actual_type_parameters/formal_type_parameters_is_actual_type_parameter.owen\n"
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

static bool literal_as_actual_parameter(void)
{
    static uint8_t index = 0;
    while (index < TYPES_LENGTH && !types[index].default_value_name)
        index++;

    if (index == TYPES_LENGTH)
        return false;

    format_test_name("semantics/expressions/call/implicit_actual_type_parameters/%s_literal_where_%s_typed_actual_parameter_expected", types[index].file_friendly_default_value_name, types[index].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T](T b) : T\n"
                          "    return b\n"
                          "end\n"
                          "\n"
                          "function c()\n"
                          "    a(%s)\n"
                          "end", 
                          types[index].default_value);

    format_command_line_options("%s", test.source_paths[0]);
    format_expectation(&test.expected_error, 
                       "%s:9:7: %s type cannot be inferred.\n",
                       test.source_paths[0],
                       types[index].default_value_name);

    index++;
    return true;
}

bool semantics_expressions_call_implicit_actual_type_parameters(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            actual_type_parameters_make_signature_match_monomorphic_function();
            state++;
            break;

        case 1:
            call_reference_that_is_not_a_function();
            state++;
            break;

        case 2:
            error_nested_in_a_polymorphic_function_call();
            state++;
            break;

        case 3:
            error_nested_in_multiple_polymorphic_function_calls();
            state++;
            break;

        case 4:
            formal_type_parameter_nested_in_compound();
            state++;
            break;

        case 5:
            formal_type_parameter_not_specified_by_actual_parameters();
            state++;
            break;

        case 6:
            formal_type_parameter_redefined_by_actual_parameter();
            state++;
            break;

        case 7:
            formal_type_parameters_is_actual_type_parameter();
            state++;
            break;

        case 8:
            if (literal_as_actual_parameter())
                break;

            state++;

        default:
            return false;
    }

    return true;
}
