#include "runner.h"

static void call_reference_that_is_not_a_function(void)
{
    format_test_name("semantics/expressions/call/explicit_actual_type_parameters/call_reference_that_is_not_a_function");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b)\n"
                          "    b[Bool]()\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/explicit_actual_type_parameters/call_reference_that_is_not_a_function.owen");

    format_expectation(&test.expected_error, 
                       "%s:5:5: b is not a function.\n",
                       test.source_paths[0]);
}

static void call_undefined_function(void)
{
    format_test_name("semantics/expressions/call/explicit_actual_type_parameters/call_undefined_function");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    b[I32]()\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/explicit_actual_type_parameters/call_undefined_function.owen");

    format_expectation(&test.expected_error, 
                       "%s:5:5: b is undefined.\n",
                       test.source_paths[0]);
}

static void error_nested_in_a_polymorphic_function_call(void)
{
    format_test_name("semantics/expressions/call/explicit_actual_type_parameters/error_nested_in_a_polymorphic_function_call");

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
                          "    return do_stuff[Direction](Direction.up, Direction.down)\n"
                          "end\n"
                          "\n"
                          "function do_stuff[T](T a, T b) : T\n"
                          "    return a + b\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/explicit_actual_type_parameters/error_nested_in_a_polymorphic_function_call.owen");

    format_expectation(&test.expected_error, 
                       "semantics/expressions/call/explicit_actual_type_parameters/error_nested_in_a_polymorphic_function_call.owen:16:14: Operator not defined for Direction and Direction.\n"
                       "Polymorphic stack:\n"
                       "    semantics/expressions/call/explicit_actual_type_parameters/error_nested_in_a_polymorphic_function_call.owen:12:12: do_stuff[Direction](Direction a, Direction b) : Direction\n");
}

static void error_nested_in_multiple_polymorphic_function_calls(void)
{
    format_test_name("semantics/expressions/call/explicit_actual_type_parameters/error_nested_in_multiple_polymorphic_function_calls");

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
                          "    return do_stuff[Direction](Direction.up)\n"
                          "end\n"
                          "\n"
                          "function do_stuff[T](T a) : T\n"
                          "    return do_stuff(a, a)\n"
                          "end\n"
                          "\n"
                          "function do_stuff[T](T a, T b) : T\n"
                          "    return a + b\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/explicit_actual_type_parameters/error_nested_in_multiple_polymorphic_function_calls.owen");

    format_expectation(&test.expected_error, 
                       "semantics/expressions/call/explicit_actual_type_parameters/error_nested_in_multiple_polymorphic_function_calls.owen:20:14: Operator not defined for Direction and Direction.\n"
                       "Polymorphic stack:\n"
                       "    semantics/expressions/call/explicit_actual_type_parameters/error_nested_in_multiple_polymorphic_function_calls.owen:12:12: do_stuff[Direction](Direction a) : Direction\n"
                       "    semantics/expressions/call/explicit_actual_type_parameters/error_nested_in_multiple_polymorphic_function_calls.owen:16:20: do_stuff[Direction](Direction a, Direction b) : Direction\n");
}

static bool literal_as_actual_parameter(void)
{
    static uint8_t index = 0;
    while (index < TYPES_LENGTH && !types[index].default_value_name)
        index++;

    if (index == TYPES_LENGTH)
        return false;

    format_test_name("semantics/expressions/call/explicit_actual_type_parameters/%s_literal_where_%s_typed_actual_parameter_expected", types[index].file_friendly_default_value_name, types[index].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure Structure\n"
                          "    I32 a\n"
                          "end\n"
                          "\n"
                          "function a[T](T b) : T\n"
                          "    return b\n"
                          "end\n"
                          "\n"
                          "function c()\n"
                          "    a[%s](%s)\n"
                          "end", 
                          types[index].name, types[index].default_value);

    format_command_line_options("%s", test.source_paths[0]);
    format_expectation(&test.expected_error, 
                       "%s:13:%u: %s type cannot be inferred.\n",
                       test.source_paths[0],
                       9 + strlen(types[index].name),
                       types[index].default_value_name);

    index++;
    return true;
}

static void missing_multiple_actual_type_parameters(void)
{
    format_test_name("semantics/expressions/call/explicit_actual_type_parameters/missing_multiple_actual_type_parameters");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T1, T2]() : T1\n"
                          "    return 0\n"
                          "end\n"
                          "\n"
                          "function c() : I32\n"
                          "    return a()\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/explicit_actual_type_parameters/missing_multiple_actual_type_parameters.owen");

    format_expectation(&test.expected_error, 
                       "%s:9:13: T1 could not be inferred from the call site. Specify it explicitly.\n",
                       test.source_paths[0]);
}

static void missing_one_actual_type_parameter(void)
{
    format_test_name("semantics/expressions/call/explicit_actual_type_parameters/missing_one_actual_type_parameter");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T]() : T\n"
                          "    return 0\n"
                          "end\n"
                          "\n"
                          "function b() : I32\n"
                          "    return a()\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/explicit_actual_type_parameters/missing_one_actual_type_parameter.owen");

    format_expectation(&test.expected_error, 
                       "%s:9:13: T could not be inferred from the call site. Specify it explicitly.\n",
                       test.source_paths[0]);
}

static void monomorphic_name_typed_formal_parameter_in_polymorphic_function(void)
{
    format_test_name("semantics/expressions/call/explicit_actual_type_parameters/monomorphic_name_typed_formal_parameter_in_polymorphic_function");

    format_source_path(0, "semantics/expressions/call/explicit_actual_type_parameters/monomorphic_name_typed_formal_parameter_in_polymorphic_function.owen");
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

    SET_COMMAND_LINE("semantics/expressions/call/explicit_actual_type_parameters/monomorphic_name_typed_formal_parameter_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/call/explicit_actual_type_parameters/monomorphic_name_typed_formal_parameter_in_polymorphic_function.owen\n"
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

static void polymorphic_name_typed_formal_parameter_in_polymorphic_function(void)
{
    format_test_name("semantics/expressions/call/explicit_actual_type_parameters/polymorphic_name_typed_formal_parameter_in_polymorphic_function");

    format_source_path(0, "semantics/expressions/call/explicit_actual_type_parameters/polymorphic_name_typed_formal_parameter_in_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T](T b) : T\n"
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

    SET_COMMAND_LINE("semantics/expressions/call/explicit_actual_type_parameters/polymorphic_name_typed_formal_parameter_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/call/explicit_actual_type_parameters/polymorphic_name_typed_formal_parameter_in_polymorphic_function.owen\n"
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

static void monomorphic_pointer_typed_formal_parameter_in_polymorphic_function(void)
{
    format_test_name("semantics/expressions/call/explicit_actual_type_parameters/monomorphic_pointer_typed_formal_parameter_in_polymorphic_function");

    format_source_path(0, "semantics/expressions/call/explicit_actual_type_parameters/monomorphic_pointer_typed_formal_parameter_in_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T](#I32 b) : #I32\n"
                          "    return b\n"
                          "end\n"
                          "\n"
                          "function c(#I32 d) : #I32\n"
                          "    return a[I32](d)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/explicit_actual_type_parameters/monomorphic_pointer_typed_formal_parameter_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/call/explicit_actual_type_parameters/monomorphic_pointer_typed_formal_parameter_in_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: #I32\n"
                       "            name: b\n"
                       "        return_type_reference: #I32\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: #I32\n"
                       "            name: b\n"
                       "        return_type: #I32\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: #I32\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: c\n"
                       "        formal_parameter\n"
                       "            type: #I32\n"
                       "            name: d\n"
                       "        return_type: #I32\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: #I32\n"
                       "                reference_expression\n"
                       "                    type: Function(#I32) : #I32\n"
                       "                    name: a\n"
                       "                    actual_type_parameter: I32\n"
                       "                reference_expression\n"
                       "                    type: #I32\n"
                       "                    name: d\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void polymorphic_pointer_typed_formal_parameter_in_polymorphic_function(void)
{
    format_test_name("semantics/expressions/call/explicit_actual_type_parameters/polymorphic_pointer_typed_formal_parameter_in_polymorphic_function");

    format_source_path(0, "semantics/expressions/call/explicit_actual_type_parameters/polymorphic_pointer_typed_formal_parameter_in_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T](#T b) : #T\n"
                          "    return b\n"
                          "end\n"
                          "\n"
                          "function c(#I32 d) : #I32\n"
                          "    return a[I32](d)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/explicit_actual_type_parameters/polymorphic_pointer_typed_formal_parameter_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/call/explicit_actual_type_parameters/polymorphic_pointer_typed_formal_parameter_in_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: #T\n"
                       "            name: b\n"
                       "        return_type_reference: #T\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: #I32\n"
                       "            name: b\n"
                       "        return_type: #I32\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: #I32\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: c\n"
                       "        formal_parameter\n"
                       "            type: #I32\n"
                       "            name: d\n"
                       "        return_type: #I32\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: #I32\n"
                       "                reference_expression\n"
                       "                    type: Function(#I32) : #I32\n"
                       "                    name: a\n"
                       "                    actual_type_parameter: I32\n"
                       "                reference_expression\n"
                       "                    type: #I32\n"
                       "                    name: d\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void monomorphic_dynamic_array_typed_formal_parameter_in_polymorphic_function(void)
{
    format_test_name("semantics/expressions/call/explicit_actual_type_parameters/monomorphic_dynamic_array_typed_formal_parameter_in_polymorphic_function");

    format_source_path(0, "semantics/expressions/call/explicit_actual_type_parameters/monomorphic_dynamic_array_typed_formal_parameter_in_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T]([]I32 b) : I32\n"
                          "    return b[0]\n"
                          "end\n"
                          "\n"
                          "function c([]I32 d) : I32\n"
                          "    return a[I32](d)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/explicit_actual_type_parameters/monomorphic_dynamic_array_typed_formal_parameter_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/call/explicit_actual_type_parameters/monomorphic_dynamic_array_typed_formal_parameter_in_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: []I32\n"
                       "            name: b\n"
                       "        return_type_reference: I32\n"
                       "        return_statement\n"
                       "            array_access\n"
                       "                reference_expression\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    value: 0\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: []I32\n"
                       "            name: b\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            array_access\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: []I32\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    type: U32\n"
                       "                    value: 0\n"
                       "    function\n"
                       "        name: c\n"
                       "        formal_parameter\n"
                       "            type: []I32\n"
                       "            name: d\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: Function([]I32) : I32\n"
                       "                    name: a\n"
                       "                    actual_type_parameter: I32\n"
                       "                reference_expression\n"
                       "                    type: []I32\n"
                       "                    name: d\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void polymorphic_dynamic_array_typed_formal_parameter_in_polymorphic_function(void)
{
    format_test_name("semantics/expressions/call/explicit_actual_type_parameters/polymorphic_dynamic_array_typed_formal_parameter_in_polymorphic_function");

    format_source_path(0, "semantics/expressions/call/explicit_actual_type_parameters/polymorphic_dynamic_array_typed_formal_parameter_in_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T]([]T b) : T\n"
                          "    return b[0]\n"
                          "end\n"
                          "\n"
                          "function c([]I32 d) : I32\n"
                          "    return a[I32](d)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/explicit_actual_type_parameters/polymorphic_dynamic_array_typed_formal_parameter_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/call/explicit_actual_type_parameters/polymorphic_dynamic_array_typed_formal_parameter_in_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: []T\n"
                       "            name: b\n"
                       "        return_type_reference: T\n"
                       "        return_statement\n"
                       "            array_access\n"
                       "                reference_expression\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    value: 0\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: []I32\n"
                       "            name: b\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            array_access\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: []I32\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    type: U32\n"
                       "                    value: 0\n"
                       "    function\n"
                       "        name: c\n"
                       "        formal_parameter\n"
                       "            type: []I32\n"
                       "            name: d\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: Function([]I32) : I32\n"
                       "                    name: a\n"
                       "                    actual_type_parameter: I32\n"
                       "                reference_expression\n"
                       "                    type: []I32\n"
                       "                    name: d\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void monomorphic_fixed_array_typed_formal_parameter_in_polymorphic_function(void)
{
    format_test_name("semantics/expressions/call/explicit_actual_type_parameters/monomorphic_fixed_array_typed_formal_parameter_in_polymorphic_function");

    format_source_path(0, "semantics/expressions/call/explicit_actual_type_parameters/monomorphic_fixed_array_typed_formal_parameter_in_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T]([4]I32 b) : I32\n"
                          "    return b[0]\n"
                          "end\n"
                          "\n"
                          "function c([4]I32 d) : I32\n"
                          "    return a[I32](d)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/explicit_actual_type_parameters/monomorphic_fixed_array_typed_formal_parameter_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/call/explicit_actual_type_parameters/monomorphic_fixed_array_typed_formal_parameter_in_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: [4]I32\n"
                       "            name: b\n"
                       "        return_type_reference: I32\n"
                       "        return_statement\n"
                       "            array_access\n"
                       "                reference_expression\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    value: 0\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: [4]I32\n"
                       "            name: b\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            array_access\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: [4]I32\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    type: U32\n"
                       "                    value: 0\n"
                       "    function\n"
                       "        name: c\n"
                       "        formal_parameter\n"
                       "            type: [4]I32\n"
                       "            name: d\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: Function([4]I32) : I32\n"
                       "                    name: a\n"
                       "                    actual_type_parameter: I32\n"
                       "                reference_expression\n"
                       "                    type: [4]I32\n"
                       "                    name: d\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void polymorphic_fixed_array_typed_formal_parameter_in_polymorphic_function(void)
{
    format_test_name("semantics/expressions/call/explicit_actual_type_parameters/polymorphic_fixed_array_typed_formal_parameter_in_polymorphic_function");

    format_source_path(0, "semantics/expressions/call/explicit_actual_type_parameters/polymorphic_fixed_array_typed_formal_parameter_in_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T]([4]T b) : T\n"
                          "    return b[0]\n"
                          "end\n"
                          "\n"
                          "function c([4]I32 d) : I32\n"
                          "    return a[I32](d)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/explicit_actual_type_parameters/polymorphic_fixed_array_typed_formal_parameter_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/call/explicit_actual_type_parameters/polymorphic_fixed_array_typed_formal_parameter_in_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: [4]T\n"
                       "            name: b\n"
                       "        return_type_reference: T\n"
                       "        return_statement\n"
                       "            array_access\n"
                       "                reference_expression\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    value: 0\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: [4]I32\n"
                       "            name: b\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            array_access\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: [4]I32\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    type: U32\n"
                       "                    value: 0\n"
                       "    function\n"
                       "        name: c\n"
                       "        formal_parameter\n"
                       "            type: [4]I32\n"
                       "            name: d\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: Function([4]I32) : I32\n"
                       "                    name: a\n"
                       "                    actual_type_parameter: I32\n"
                       "                reference_expression\n"
                       "                    type: [4]I32\n"
                       "                    name: d\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void monomorphic_function_typed_formal_parameter_in_polymorphic_function(void)
{
    format_test_name("semantics/expressions/call/explicit_actual_type_parameters/monomorphic_function_typed_formal_parameter_in_polymorphic_function");

    format_source_path(0, "semantics/expressions/call/explicit_actual_type_parameters/monomorphic_function_typed_formal_parameter_in_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T](Function(I32) b) : Function(I32)\n"
                          "    return b\n"
                          "end\n"
                          "\n"
                          "function c(Function(I32) d) : Function(I32)\n"
                          "    return a[I32](d)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/explicit_actual_type_parameters/monomorphic_function_typed_formal_parameter_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/call/explicit_actual_type_parameters/monomorphic_function_typed_formal_parameter_in_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: Function(I32)\n"
                       "            name: b\n"
                       "        return_type_reference: Function(I32)\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: Function(I32)\n"
                       "            name: b\n"
                       "        return_type: Function(I32)\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: Function(I32)\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: c\n"
                       "        formal_parameter\n"
                       "            type: Function(I32)\n"
                       "            name: d\n"
                       "        return_type: Function(I32)\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: Function(I32)\n"
                       "                reference_expression\n"
                       "                    type: Function(Function(I32)) : Function(I32)\n"
                       "                    name: a\n"
                       "                    actual_type_parameter: I32\n"
                       "                reference_expression\n"
                       "                    type: Function(I32)\n"
                       "                    name: d\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void polymorphic_function_typed_formal_parameter_in_polymorphic_function(void)
{
    format_test_name("semantics/expressions/call/explicit_actual_type_parameters/polymorphic_function_typed_formal_parameter_in_polymorphic_function");

    format_source_path(0, "semantics/expressions/call/explicit_actual_type_parameters/polymorphic_function_typed_formal_parameter_in_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T](Function(T) b) : Function(T)\n"
                          "    return b\n"
                          "end\n"
                          "\n"
                          "function c(Function(I32) d) : Function(I32)\n"
                          "    return a[I32](d)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/explicit_actual_type_parameters/polymorphic_function_typed_formal_parameter_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/call/explicit_actual_type_parameters/polymorphic_function_typed_formal_parameter_in_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: Function(T)\n"
                       "            name: b\n"
                       "        return_type_reference: Function(T)\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: Function(I32)\n"
                       "            name: b\n"
                       "        return_type: Function(I32)\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: Function(I32)\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: c\n"
                       "        formal_parameter\n"
                       "            type: Function(I32)\n"
                       "            name: d\n"
                       "        return_type: Function(I32)\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: Function(I32)\n"
                       "                reference_expression\n"
                       "                    type: Function(Function(I32)) : Function(I32)\n"
                       "                    name: a\n"
                       "                    actual_type_parameter: I32\n"
                       "                reference_expression\n"
                       "                    type: Function(I32)\n"
                       "                    name: d\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void monomorphic_actual_type_parameter_for_polymorphic_compound_typed_formal_parameter_in_polymorphic_function(void)
{
    format_test_name("semantics/expressions/call/explicit_actual_type_parameters/monomorphic_actual_type_parameter_for_polymorphic_compound_typed_formal_parameter_in_polymorphic_function");

    format_source_path(0, "semantics/expressions/call/explicit_actual_type_parameters/monomorphic_actual_type_parameter_for_polymorphic_compound_typed_formal_parameter_in_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A[T]\n"
                          "    T a\n"
                          "end\n"
                          "\n"
                          "function a[T](A[I32] b) : A[I32]\n"
                          "    return b\n"
                          "end\n"
                          "\n"
                          "function c(A[I32] d) : A[I32]\n"
                          "    return a[I32](d)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/explicit_actual_type_parameters/monomorphic_actual_type_parameter_for_polymorphic_compound_typed_formal_parameter_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/call/explicit_actual_type_parameters/monomorphic_actual_type_parameter_for_polymorphic_compound_typed_formal_parameter_in_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        formal_type_parameter: T\n"
                       "        field\n"
                       "            type_reference: T\n"
                       "            name: a\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        actual_type_parameter: I32\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: a\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: A[I32]\n"
                       "            name: b\n"
                       "        return_type_reference: A[I32]\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: A[I32]\n"
                       "            name: b\n"
                       "        return_type: A[I32]\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: A[I32]\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: c\n"
                       "        formal_parameter\n"
                       "            type: A[I32]\n"
                       "            name: d\n"
                       "        return_type: A[I32]\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: A[I32]\n"
                       "                reference_expression\n"
                       "                    type: Function(A[I32]) : A[I32]\n"
                       "                    name: a\n"
                       "                    actual_type_parameter: I32\n"
                       "                reference_expression\n"
                       "                    type: A[I32]\n"
                       "                    name: d\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n" );
}

static void polymorphic_actual_type_parameter_for_polymorphic_compound_typed_formal_parameter_in_polymorphic_function(void)
{
    format_test_name("semantics/expressions/call/explicit_actual_type_parameters/polymorphic_actual_type_parameter_for_polymorphic_compound_typed_formal_parameter_in_polymorphic_function");

    format_source_path(0, "semantics/expressions/call/explicit_actual_type_parameters/polymorphic_actual_type_parameter_for_polymorphic_compound_typed_formal_parameter_in_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A[T]\n"
                          "    T a\n"
                          "end\n"
                          "\n"
                          "function a[T](A[T] b) : A[T]\n"
                          "    return b\n"
                          "end\n"
                          "\n"
                          "function c(A[I32] d) : A[I32]\n"
                          "    return a[I32](d)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/explicit_actual_type_parameters/polymorphic_actual_type_parameter_for_polymorphic_compound_typed_formal_parameter_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/call/explicit_actual_type_parameters/polymorphic_actual_type_parameter_for_polymorphic_compound_typed_formal_parameter_in_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        formal_type_parameter: T\n"
                       "        field\n"
                       "            type_reference: T\n"
                       "            name: a\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        actual_type_parameter: I32\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: a\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: A[T]\n"
                       "            name: b\n"
                       "        return_type_reference: A[T]\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: A[I32]\n"
                       "            name: b\n"
                       "        return_type: A[I32]\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: A[I32]\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: c\n"
                       "        formal_parameter\n"
                       "            type: A[I32]\n"
                       "            name: d\n"
                       "        return_type: A[I32]\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: A[I32]\n"
                       "                reference_expression\n"
                       "                    type: Function(A[I32]) : A[I32]\n"
                       "                    name: a\n"
                       "                    actual_type_parameter: I32\n"
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

static void polymorphic_formal_type_parameter_used_as_explicit_type_parameter(void)
{
    format_test_name("semantics/expressions/call/explicit_actual_type_parameters/polymorphic_formal_type_parameter_used_as_explicit_type_parameter");

    format_source_path(0, "semantics/expressions/call/explicit_actual_type_parameters/polymorphic_formal_type_parameter_used_as_explicit_type_parameter.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T]() : T\n"
                          "    return 0\n"
                          "end\n"
                          "\n"
                          "function b[T]() : T\n"
                          "    return a[T]()\n"
                          "end\n"
                          "\n"
                          "function c() : I32\n"
                          "    return b[I32]()\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/explicit_actual_type_parameters/polymorphic_formal_type_parameter_used_as_explicit_type_parameter.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/call/explicit_actual_type_parameters/polymorphic_formal_type_parameter_used_as_explicit_type_parameter.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        return_type_reference: T\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                value: 0\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n"
                       "    function\n"
                       "        name: b\n"
                       "        formal_type_parameter: T\n"
                       "        return_type_reference: T\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                reference_expression\n"
                       "                    name: a\n"
                       "                    actual_type_parameter: T\n"
                       "    function\n"
                       "        name: b\n"
                       "        actual_type_parameter: I32\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: Function() : I32\n"
                       "                    name: a\n"
                       "                    actual_type_parameter: I32\n"
                       "    function\n"
                       "        name: c\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: Function() : I32\n"
                       "                    name: b\n"
                       "                    actual_type_parameter: I32\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static bool pass_x_typed_expression(void)
{
    static uint8_t actual_type_parameter = 0;
    static uint8_t formal_type_parameter = 0;

    if (actual_type_parameter == TYPES_LENGTH)
        return false;

    format_test_name("semantics/expressions/call/explicit_actual_type_parameters/pass_%s_typed_expression_to_%s_typed_formal_parameter", types[actual_type_parameter].file_friendly_name, types[formal_type_parameter].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration Enumeration : I32\n"
                          "    a\n"
                          "end\n"
                          "\n"
                          "structure Structure\n"
                          "    I32 a\n"
                          "end\n"
                          "\n"
                          "structure Structure[T]\n"
                          "    T a\n"
                          "end\n"
                          "\n"
                          "union Union\n"
                          "    I32 a\n"
                          "end\n"
                          "\n"
                          "union Union[T]\n"
                          "    T a\n"
                          "end\n"
                          "\n"
                          "function a[T](T b) : T, T\n"
                          "    return b, b\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    %s d\n"
                          "    _, _ = a[%s](d)\n"
                          "    return 0\n"
                          "end", 
                          types[actual_type_parameter].name, 
                          types[formal_type_parameter].name);

    format_command_line_options("%s -print-semantics", test.source_paths[0]);
    if (is_noalias_type(types[actual_type_parameter].name))
    {
        format_expectation(&test.expected_error,
                           "%s:29:5: Only Pointerless formal parameter types can be noalias qualified.\n",
                           test.source_paths[0],
                           test.source_paths[0],
                           types[actual_type_parameter].name);
    }
    else if (compatible_types(types[formal_type_parameter].name, types[actual_type_parameter].name))
    {
        if (is_noalias_type(types[formal_type_parameter].name))
        {
            format_expectation(&test.expected_error, 
                               "%s:24:22: Only Pointerless formal parameter types can be noalias qualified.\n"
                               "Polymorphic stack:\n"
                               "    %s:30:12: a[%s]()\n",
                               test.source_paths[0],
                               test.source_paths[0],
                               types[formal_type_parameter].name);
        
        }
        else if (actual_type_parameter == POLYMORPHIC_STRUCTURE_TYPE_INDEX)
        {
            format_expectation(&test.expected_semantics, 
                               "file\n"
                               "    path: %s\n"
                               "    namespace: Abc\n"
                               "    enumeration\n"
                               "        name: Enumeration\n"
                               "        underlying_type: I32\n"
                               "        is_public: false\n"
                               "        constant\n"
                               "            name: a\n"
                               "            value: 0\n"
                               "    structure\n"
                               "        name: Structure\n"
                               "        is_public: false\n"
                               "        field\n"
                               "            type: I32\n"
                               "            name: a\n"
                               "    structure\n"
                               "        name: Structure\n"
                               "        is_public: false\n"
                               "        formal_type_parameter: T\n"
                               "        field\n"
                               "            type_reference: T\n"
                               "            name: a\n"
                               "    structure\n"
                               "        name: Structure\n"
                               "        is_public: false\n"
                               "        actual_type_parameter: I32\n"
                               "        field\n"
                               "            type: I32\n"
                               "            name: a\n"
                               "    union\n"
                               "        name: Union\n"
                               "        is_public: false\n"
                               "        field\n"
                               "            type: I32\n"
                               "            name: a\n"
                               "    union\n"
                               "        name: Union\n"
                               "        is_public: false\n"
                               "        formal_type_parameter: T\n"
                               "        field\n"
                               "            type_reference: T\n"
                               "            name: a\n"
                               "    function\n"
                               "        name: a\n"
                               "        formal_type_parameter: T\n"
                               "        formal_parameter\n"
                               "            type_reference: T\n"
                               "            name: b\n"
                               "        return_type_reference: T\n"
                               "        return_type_reference: T\n"
                               "        return_statement\n"
                               "            reference_expression\n"
                               "                name: b\n"
                               "            reference_expression\n"
                               "                name: b\n"
                               "    function\n"
                               "        name: a\n"
                               "        actual_type_parameter: Structure[I32]\n"
                               "        formal_parameter\n"
                               "            type: Structure[I32]\n"
                               "            name: b\n"
                               "        return_type: Structure[I32]\n"
                               "        return_type: Structure[I32]\n"
                               "        return_statement\n"
                               "            reference_expression\n"
                               "                type: Structure[I32]\n"
                               "                name: b\n"
                               "            reference_expression\n"
                               "                type: Structure[I32]\n"
                               "                name: b\n"
                               "    function\n"
                               "        name: main\n"
                               "        return_type: I32\n"
                               "        declaration_statement\n"
                               "            variable\n"
                               "                type: Structure[I32]\n"
                               "                name: d\n"
                               "        assignment_statement\n"
                               "            operator: =\n"
                               "            blank_identifier\n"
                               "                type: Structure[I32]\n"
                               "            blank_identifier\n"
                               "                type: Structure[I32]\n"
                               "            call_expression\n"
                               "                type: Structure[I32], Structure[I32]\n"
                               "                reference_expression\n"
                               "                    type: Function(Structure[I32]) : Structure[I32], Structure[I32]\n"
                               "                    name: a\n"
                               "                    actual_type_parameter: Structure[I32]\n"
                               "                reference_expression\n"
                               "                    type: Structure[I32]\n"
                               "                    name: d\n"
                               "        return_statement\n"
                               "            integer_literal\n"
                               "                type: I32\n"
                               "                value: 0\n",
                               test.source_paths[0]);
        }
        else if (actual_type_parameter == POLYMORPHIC_UNION_TYPE_INDEX)
        {
            format_expectation(&test.expected_semantics, 
                               "file\n"
                               "    path: %s\n"
                               "    namespace: Abc\n"
                               "    enumeration\n"
                               "        name: Enumeration\n"
                               "        underlying_type: I32\n"
                               "        is_public: false\n"
                               "        constant\n"
                               "            name: a\n"
                               "            value: 0\n"
                               "    structure\n"
                               "        name: Structure\n"
                               "        is_public: false\n"
                               "        field\n"
                               "            type: I32\n"
                               "            name: a\n"
                               "    structure\n"
                               "        name: Structure\n"
                               "        is_public: false\n"
                               "        formal_type_parameter: T\n"
                               "        field\n"
                               "            type_reference: T\n"
                               "            name: a\n"
                               "    union\n"
                               "        name: Union\n"
                               "        is_public: false\n"
                               "        field\n"
                               "            type: I32\n"
                               "            name: a\n"
                               "    union\n"
                               "        name: Union\n"
                               "        is_public: false\n"
                               "        formal_type_parameter: T\n"
                               "        field\n"
                               "            type_reference: T\n"
                               "            name: a\n"
                               "    union\n"
                               "        name: Union\n"
                               "        is_public: false\n"
                               "        actual_type_parameter: I32\n"
                               "        field\n"
                               "            type: I32\n"
                               "            name: a\n"
                               "    function\n"
                               "        name: a\n"
                               "        formal_type_parameter: T\n"
                               "        formal_parameter\n"
                               "            type_reference: T\n"
                               "            name: b\n"
                               "        return_type_reference: T\n"
                               "        return_type_reference: T\n"
                               "        return_statement\n"
                               "            reference_expression\n"
                               "                name: b\n"
                               "            reference_expression\n"
                               "                name: b\n"
                               "    function\n"
                               "        name: a\n"
                               "        actual_type_parameter: Union[I32]\n"
                               "        formal_parameter\n"
                               "            type: Union[I32]\n"
                               "            name: b\n"
                               "        return_type: Union[I32]\n"
                               "        return_type: Union[I32]\n"
                               "        return_statement\n"
                               "            reference_expression\n"
                               "                type: Union[I32]\n"
                               "                name: b\n"
                               "            reference_expression\n"
                               "                type: Union[I32]\n"
                               "                name: b\n"
                               "    function\n"
                               "        name: main\n"
                               "        return_type: I32\n"
                               "        declaration_statement\n"
                               "            variable\n"
                               "                type: Union[I32]\n"
                               "                name: d\n"
                               "        assignment_statement\n"
                               "            operator: =\n"
                               "            blank_identifier\n"
                               "                type: Union[I32]\n"
                               "            blank_identifier\n"
                               "                type: Union[I32]\n"
                               "            call_expression\n"
                               "                type: Union[I32], Union[I32]\n"
                               "                reference_expression\n"
                               "                    type: Function(Union[I32]) : Union[I32], Union[I32]\n"
                               "                    name: a\n"
                               "                    actual_type_parameter: Union[I32]\n"
                               "                reference_expression\n"
                               "                    type: Union[I32]\n"
                               "                    name: d\n"
                               "        return_statement\n"
                               "            integer_literal\n"
                               "                type: I32\n"
                               "                value: 0\n",
                               test.source_paths[0]);
        }
        else
        {
            format_expectation(&test.expected_semantics, 
                               "file\n"
                               "    path: %s\n"
                               "    namespace: Abc\n"
                               "    enumeration\n"
                               "        name: Enumeration\n"
                               "        underlying_type: I32\n"
                               "        is_public: false\n"
                               "        constant\n"
                               "            name: a\n"
                               "            value: 0\n"
                               "    structure\n"
                               "        name: Structure\n"
                               "        is_public: false\n"
                               "        field\n"
                               "            type: I32\n"
                               "            name: a\n"
                               "    structure\n"
                               "        name: Structure\n"
                               "        is_public: false\n"
                               "        formal_type_parameter: T\n"
                               "        field\n"
                               "            type_reference: T\n"
                               "            name: a\n"
                               "    union\n"
                               "        name: Union\n"
                               "        is_public: false\n"
                               "        field\n"
                               "            type: I32\n"
                               "            name: a\n"
                               "    union\n"
                               "        name: Union\n"
                               "        is_public: false\n"
                               "        formal_type_parameter: T\n"
                               "        field\n"
                               "            type_reference: T\n"
                               "            name: a\n"
                               "    function\n"
                               "        name: a\n"
                               "        formal_type_parameter: T\n"
                               "        formal_parameter\n"
                               "            type_reference: T\n"
                               "            name: b\n"
                               "        return_type_reference: T\n"
                               "        return_type_reference: T\n"
                               "        return_statement\n"
                               "            reference_expression\n"
                               "                name: b\n"
                               "            reference_expression\n"
                               "                name: b\n"
                               "    function\n"
                               "        name: a\n"
                               "        actual_type_parameter: %s\n"
                               "        formal_parameter\n"
                               "            type: %s\n"
                               "            name: b\n"
                               "        return_type: %s\n"
                               "        return_type: %s\n"
                               "        return_statement\n"
                               "            reference_expression\n"
                               "                type: %s\n"
                               "                name: b\n"
                               "            reference_expression\n"
                               "                type: %s\n"
                               "                name: b\n"
                               "    function\n"
                               "        name: main\n"
                               "        return_type: I32\n"
                               "        declaration_statement\n"
                               "            variable\n"
                               "                type: %s\n"
                               "                name: d\n"
                               "        assignment_statement\n"
                               "            operator: =\n"
                               "            blank_identifier\n"
                               "                type: %s\n"
                               "            blank_identifier\n"
                               "                type: %s\n"
                               "            call_expression\n"
                               "                type: %s, %s\n"
                               "                reference_expression\n"
                               "                    type: Function(%s) : %s, %s\n"
                               "                    name: a\n"
                               "                    actual_type_parameter: %s\n"
                               "                reference_expression\n"
                               "                    type: %s\n"
                               "                    name: d\n"
                               "        return_statement\n"
                               "            integer_literal\n"
                               "                type: I32\n"
                               "                value: 0\n",
                               test.source_paths[0],
                               types[formal_type_parameter].name,
                               types[formal_type_parameter].name,
                               types[formal_type_parameter].name,
                               types[formal_type_parameter].name,
                               types[formal_type_parameter].name,
                               types[formal_type_parameter].name,
                               types[actual_type_parameter].name,
                               types[formal_type_parameter].name,
                               types[formal_type_parameter].name,
                               types[formal_type_parameter].name,
                               types[formal_type_parameter].name,
                               types[formal_type_parameter].name,
                               types[formal_type_parameter].name,
                               types[formal_type_parameter].name,
                               types[formal_type_parameter].name,
                               types[actual_type_parameter].name);
        }
    }
    else
    {
        format_expectation(&test.expected_error, 
                           "%s:30:12: No matching signature for a.\n"
                           "Declarations:\n"
                           "    %s:24:10: a[T](T b) : T, T\n",
                           test.source_paths[0],
                           test.source_paths[0]);
        
    }

    if (formal_type_parameter == LAST_TYPE_INDEX)
    {
        formal_type_parameter = 0;
        actual_type_parameter++;
    }
    else
        formal_type_parameter++;

    return true;
}

bool semantics_expressions_call_explicit_actual_type_parameters(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            call_reference_that_is_not_a_function();
            state++;
            break;

        case 1:
            call_undefined_function();
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
            if (literal_as_actual_parameter())
                break;
            
            state++;

        case 5:
            missing_multiple_actual_type_parameters();
            state++;
            break;

        case 6:
            missing_one_actual_type_parameter();
            state++;
            break;

        case 7:
            monomorphic_name_typed_formal_parameter_in_polymorphic_function();
            state++;
            break;

        case 8:
            polymorphic_name_typed_formal_parameter_in_polymorphic_function();
            state++;
            break;

        case 9:
            monomorphic_pointer_typed_formal_parameter_in_polymorphic_function();
            state++;
            break;

        case 10:
            polymorphic_pointer_typed_formal_parameter_in_polymorphic_function();
            state++;
            break;

        case 11:
            monomorphic_dynamic_array_typed_formal_parameter_in_polymorphic_function();
            state++;
            break;

        case 12:
            polymorphic_dynamic_array_typed_formal_parameter_in_polymorphic_function();
            state++;
            break;

        case 13:
            monomorphic_fixed_array_typed_formal_parameter_in_polymorphic_function();
            state++;
            break;

        case 14:
            polymorphic_fixed_array_typed_formal_parameter_in_polymorphic_function();
            state++;
            break;

        case 15:
            monomorphic_function_typed_formal_parameter_in_polymorphic_function();
            state++;
            break;

        case 16:
            polymorphic_function_typed_formal_parameter_in_polymorphic_function();
            state++;
            break;

        case 17:
            monomorphic_actual_type_parameter_for_polymorphic_compound_typed_formal_parameter_in_polymorphic_function();
            state++;
            break;

        case 18:
            polymorphic_actual_type_parameter_for_polymorphic_compound_typed_formal_parameter_in_polymorphic_function();
            state++;
            break;

        case 19:
            polymorphic_formal_type_parameter_used_as_explicit_type_parameter();
            state++;
            break;

        case 20:
            if (pass_x_typed_expression())
                break;

            state++;

        default:
            return false;
    }

    return true;
}
