#include "runner.h"

static void polymorphic_and_monomorphic_return_types(void)
{
    format_test_name("semantics/statements/return/polymorphic_and_monomorphic_return_types");

    format_source_path(0, "semantics/statements/return/polymorphic_and_monomorphic_return_types.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T](T b, I32 c) : T, I32\n"
                          "    return b, c\n"
                          "end\n"
                          "\n"
                          "function d(I32 e)\n"
                          "    e, e = a(e, e)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/return/polymorphic_and_monomorphic_return_types.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/statements/return/polymorphic_and_monomorphic_return_types.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: T\n"
                       "            name: b\n"
                       "        formal_parameter\n"
                       "            type_reference: I32\n"
                       "            name: c\n"
                       "        return_type_reference: T\n"
                       "        return_type_reference: I32\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                name: b\n"
                       "            reference_expression\n"
                       "                name: c\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: b\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: c\n"
                       "        return_type: I32\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: b\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: c\n"
                       "    function\n"
                       "        name: d\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: e\n"
                       "        assignment_statement\n"
                       "            operator: =\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: e\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: e\n"
                       "            call_expression\n"
                       "                type: I32, I32\n"
                       "                reference_expression\n"
                       "                    type: Function(I32, I32) : I32, I32\n"
                       "                    name: a\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: e\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: e\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void return_call_to_function_that_returns_multiple_values_matching_return_types(void)
{
    format_test_name("semantics/statements/return/return_call_to_function_that_returns_multiple_values_matching_return_types");

    format_source_path(0, "semantics/statements/return/return_call_to_function_that_returns_multiple_values_matching_return_types.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a() : I32, I32\n"
                          "    return 0, 0\n"
                          "end\n"
                          "\n"
                          "function b() : I32, I32\n"
                          "    return a()\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/return/return_call_to_function_that_returns_multiple_values_matching_return_types.owen");
    format_expectation(&test.expected_error, 
                       "%s:9:13: 2 expressions expected but found 1.\n",
                       test.source_paths[0]);
}

static void return_call_to_function_that_returns_no_value(void)
{
    format_test_name("semantics/statements/return/return_call_to_function_that_returns_no_value");

    format_source_path(0, "semantics/statements/return/return_call_to_function_that_returns_no_value.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "end\n"
                          "\n"
                          "function b() : I32\n"
                          "    return a()\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/return/return_call_to_function_that_returns_no_value.owen");
    format_expectation(&test.expected_error, 
                       "%s:8:13: I32 expected but found none.\n",
                       test.source_paths[0]);
}

static void return_multiple_values_too_few(void)
{
    format_test_name("semantics/statements/return/return_multiple_values_too_few");

    format_source_path(0, "semantics/statements/return/return_multiple_values_too_few.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b) : I32, I32, I32\n"
                          "    return b\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/return/return_multiple_values_too_few.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:12: 3 expressions expected but found 1.\n",
                       test.source_paths[0]);
}

static void return_multiple_values_too_many(void)
{
    format_test_name("semantics/statements/return/return_multiple_values_too_many");

    format_source_path(0, "semantics/statements/return/return_multiple_values_too_many.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b, I32 c, I32 d) : I32\n"
                          "    return b, c, d\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/return/return_multiple_values_too_many.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:15: 1 expression expected but found 3.\n",
                       test.source_paths[0]);
}

static void return_one_too_few_values(void)
{
    format_test_name("semantics/statements/return/return_one_too_few_values");

    format_source_path(0, "semantics/statements/return/return_one_too_few_values.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b) : I32, I32\n"
                          "    return b\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/return/return_one_too_few_values.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:12: 2 expressions expected but found 1.\n",
                       test.source_paths[0]);
}

static void return_one_too_many_values(void)
{
    format_test_name("semantics/statements/return/return_one_too_many_values");

    format_source_path(0, "semantics/statements/return/return_one_too_many_values.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b, I32 c) : I32\n"
                          "    return b, c\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/return/return_one_too_many_values.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:15: 1 expression expected but found 2.\n",
                       test.source_paths[0]);
}

static bool single_expected_expression(void)
{
    static uint8_t parameter_type = 0;
    static uint8_t return_type = 0;

    if (return_type > LAST_TYPE_INDEX)
        return false;

    format_test_name("semantics/statements/return/%s_typed_expression_where_%s_type_is_expected", types[parameter_type].file_friendly_name, types[return_type].file_friendly_address_of_name);

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
                          "function a(%s b) : %s\n"
                          "    return b\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end\n",
                          types[parameter_type].name,
                          types[return_type].name);

    format_command_line_options("%s -print-semantics", test.source_paths[0]);
    if (compatible_types(types[return_type].name, types[parameter_type].name) && !is_noalias_type(types[return_type].name))
    {
        if (parameter_type == POLYMORPHIC_UNION_TYPE_INDEX)
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
                               "        formal_parameter\n"
                               "            type: Union[I32]\n"
                               "            name: b\n"
                               "        return_type: Union[I32]\n"
                               "        return_statement\n"
                               "            reference_expression\n"
                               "                type: Union[I32]\n"
                               "                name: b\n"
                               "    function\n"
                               "        name: main\n"
                               "        return_type: I32\n"
                               "        return_statement\n"
                               "            integer_literal\n"
                               "                type: I32\n"
                               "                value: 0\n",
                               test.source_paths[0]);
        }
        else if (parameter_type == POLYMORPHIC_STRUCTURE_TYPE_INDEX)
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
                               "        formal_parameter\n"
                               "            type: Structure[I32]\n"
                               "            name: b\n"
                               "        return_type: Structure[I32]\n"
                               "        return_statement\n"
                               "            reference_expression\n"
                               "                type: Structure[I32]\n"
                               "                name: b\n"
                               "    function\n"
                               "        name: main\n"
                               "        return_type: I32\n"
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
                               "        formal_parameter\n"
                               "            type: %s\n"
                               "            name: b\n"
                               "        return_type: %s\n"
                               "        return_statement\n"
                               "            reference_expression\n"
                               "                type: %s\n"
                               "                name: b\n"
                               "    function\n"
                               "        name: main\n"
                               "        return_type: I32\n"
                               "        return_statement\n"
                               "            integer_literal\n"
                               "                type: I32\n"
                               "                value: 0\n",
                               test.source_paths[0],
                               types[parameter_type].name,
                               types[return_type].name,
                               types[parameter_type].name);
        }
    }
    else if (is_noalias_type(types[return_type].name))
    {
        format_expectation(&test.expected_error, "%s:24:%u: Only Pointerless formal parameter types can be noalias qualified.\n",
                           test.source_paths[0],
                           18 + strlen(types[parameter_type].name));
    }
    else
    {
        format_expectation(&test.expected_error, "%s:25:12: %s expected but found %s.\n",
                           test.source_paths[0],
                           types[return_type].name,
                           types[parameter_type].name);
    }

    parameter_type++;
    if (parameter_type > LAST_TYPE_INDEX)
    {
        parameter_type = 0;
        return_type++;
    }

    return true;
}

static bool multiple_expected_expressions(void)
{
    static uint8_t parameter_type = 0;
    static uint8_t return_type = 0;

    if (return_type > LAST_TYPE_INDEX)
        return false;

    format_test_name("semantics/statements/return/%s_typed_expressions_where_%s_types_is_expected", types[parameter_type].file_friendly_name, types[return_type].file_friendly_address_of_name);

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
                          "function a(%s b, %s c) : %s, %s\n"
                          "    return b, c\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end\n",
                          types[parameter_type].name, types[parameter_type].name,
                          types[return_type].name, types[return_type].name);

    format_command_line_options("%s -print-semantics", test.source_paths[0]);
    if (compatible_types(types[return_type].name, types[parameter_type].name) && !is_noalias_type(types[return_type].name))
    {
        if (parameter_type == POLYMORPHIC_UNION_TYPE_INDEX)
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
                               "        formal_parameter\n"
                               "            type: Union[I32]\n"
                               "            name: b\n"
                               "        formal_parameter\n"
                               "            type: Union[I32]\n"
                               "            name: c\n"
                               "        return_type: Union[I32]\n"
                               "        return_type: Union[I32]\n"
                               "        return_statement\n"
                               "            reference_expression\n"
                               "                type: Union[I32]\n"
                               "                name: b\n"
                               "            reference_expression\n"
                               "                type: Union[I32]\n"
                               "                name: c\n"
                               "    function\n"
                               "        name: main\n"
                               "        return_type: I32\n"
                               "        return_statement\n"
                               "            integer_literal\n"
                               "                type: I32\n"
                               "                value: 0\n",
                               test.source_paths[0]);
        }
        else if (parameter_type == POLYMORPHIC_STRUCTURE_TYPE_INDEX)
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
                               "        formal_parameter\n"
                               "            type: Structure[I32]\n"
                               "            name: b\n"
                               "        formal_parameter\n"
                               "            type: Structure[I32]\n"
                               "            name: c\n"
                               "        return_type: Structure[I32]\n"
                               "        return_type: Structure[I32]\n"
                               "        return_statement\n"
                               "            reference_expression\n"
                               "                type: Structure[I32]\n"
                               "                name: b\n"
                               "            reference_expression\n"
                               "                type: Structure[I32]\n"
                               "                name: c\n"
                               "    function\n"
                               "        name: main\n"
                               "        return_type: I32\n"
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
                               "        formal_parameter\n"
                               "            type: %s\n"
                               "            name: b\n"
                               "        formal_parameter\n"
                               "            type: %s\n"
                               "            name: c\n"
                               "        return_type: %s\n"
                               "        return_type: %s\n"
                               "        return_statement\n"
                               "            reference_expression\n"
                               "                type: %s\n"
                               "                name: b\n"
                               "            reference_expression\n"
                               "                type: %s\n"
                               "                name: c\n"
                               "    function\n"
                               "        name: main\n"
                               "        return_type: I32\n"
                               "        return_statement\n"
                               "            integer_literal\n"
                               "                type: I32\n"
                               "                value: 0\n",
                               test.source_paths[0],
                               types[parameter_type].name,
                               types[parameter_type].name,
                               types[return_type].name,
                               types[return_type].name,
                               types[parameter_type].name,
                               types[parameter_type].name);
        }
    }
    else if (is_noalias_type(types[return_type].name))
    {
        format_expectation(&test.expected_error, "%s:24:%u: Only Pointerless formal parameter types can be noalias qualified.\n",
                           test.source_paths[0],
                           22 + 2 * strlen(types[parameter_type].name));
    }
    else
    {
        format_expectation(&test.expected_error, "%s:25:12: %s expected but found %s.\n",
                           test.source_paths[0],
                           types[return_type].name,
                           types[parameter_type].name);
    }

    parameter_type++;
    if (parameter_type > LAST_TYPE_INDEX)
    {
        parameter_type = 0;
        return_type++;
    }

    return true;
}

static void return_values_when_no_return_types_are_declared(void)
{
    format_test_name("semantics/statements/return/return_values_when_no_return_types_are_declared");

    format_source_path(0, "semantics/statements/return/return_values_when_no_return_types_are_declared.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b)\n"
                          "    return b\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/return/return_values_when_no_return_types_are_declared.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:12: 0 expressions expected but found 1.\n",
                       test.source_paths[0]);
}

static void return_no_values_when_return_types_are_declared(void)
{
    format_test_name("semantics/statements/return/return_no_values_when_return_types_are_declared");

    format_source_path(0, "semantics/statements/return/return_no_values_when_return_types_are_declared.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a() : I32\n"
                          "    return\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/return/return_no_values_when_return_types_are_declared.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:5: 1 expression expected but found 0.\n",
                       test.source_paths[0]);
}

static void return_types_but_no_return_statement(void)
{
    format_test_name("semantics/statements/return/return_types_but_no_return_statement");

    format_source_path(0, "semantics/statements/return/return_types_but_no_return_statement.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a() : I32\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/return/return_types_but_no_return_statement.owen");
    format_expectation(&test.expected_error, 
                       "%s:4:10: Missing terminating statement.\n",
                       test.source_paths[0]);
}

static bool return_x_type_compatible_literal(void)
{
    static uint8_t index = 0;
    while (true)
    {
        if (index == LAST_TYPE_INDEX)
            return false;

        bool is_valid_case = types[index].file_friendly_default_value_name != NULL;
        if (is_valid_case)
        {
            format_test_name("semantics/statements/return/return_%s_type_compatible_%s_typed_literal", types[index].file_friendly_name, types[index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
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
                                  "function a() : %s\n"
                                  "    return %s\n"
                                  "end\n"
                                  "\n"
                                  "function main() : I32\n"
                                  "    return 0\n"
                                  "end", 
                                  types[index].name,
                                  types[index].default_value);

            format_command_line_options("%s -print-semantics", test.source_paths[0]);
            if (index == POLYMORPHIC_STRUCTURE_TYPE_INDEX)
            {
                format_expectation(&test.expected_semantics, 
                                   "file\n"
                                   "    path: %s\n"
                                   "    namespace: Abc\n"
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
                                   "        return_type: Structure[I32]\n"
                                   "        return_statement\n"
                                   "            compound_literal\n"
                                   "                type: Structure[I32]\n"
                                   "                field_initializer\n"
                                   "                    type: I32\n"
                                   "                    name: a\n"
                                   "                    integer_literal\n"
                                   "                        type: I32\n"
                                   "                        value: 0\n"
                                   "    function\n"
                                   "        name: main\n"
                                   "        return_type: I32\n"
                                   "        return_statement\n"
                                   "            integer_literal\n"
                                   "                type: I32\n"
                                   "                value: 0\n",
                                   test.source_paths[0]);
            }
            else if (index == POLYMORPHIC_UNION_TYPE_INDEX)
            {
                format_expectation(&test.expected_semantics, 
                                   "file\n"
                                   "    path: %s\n"
                                   "    namespace: Abc\n"
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
                                   "        return_type: Union[I32]\n"
                                   "        return_statement\n"
                                   "            compound_literal\n"
                                   "                type: Union[I32]\n"
                                   "                field_initializer\n"
                                   "                    type: I32\n"
                                   "                    name: a\n"
                                   "                    integer_literal\n"
                                   "                        type: I32\n"
                                   "                        value: 0\n"
                                   "    function\n"
                                   "        name: main\n"
                                   "        return_type: I32\n"
                                   "        return_statement\n"
                                   "            integer_literal\n"
                                   "                type: I32\n"
                                   "                value: 0\n",
                                   test.source_paths[0]);
            }
            else if (types[index].default_value_name && !strcmp("Compound", types[index].default_value_name))
            {
                format_expectation(&test.expected_semantics, 
                                   "file\n"
                                   "    path: %s\n"
                                   "    namespace: Abc\n"
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
                                   "        return_type: %s\n"
                                   "        return_statement\n"
                                   "            compound_literal\n"
                                   "                type: %s\n"
                                   "                field_initializer\n"
                                   "                    type: I32\n"
                                   "                    name: a\n"
                                   "                    integer_literal\n"
                                   "                        type: I32\n"
                                   "                        value: 0\n"
                                   "    function\n"
                                   "        name: main\n"
                                   "        return_type: I32\n"
                                   "        return_statement\n"
                                   "            integer_literal\n"
                                   "                type: I32\n"
                                   "                value: 0\n",
                                   test.source_paths[0],
                                   types[index].name,
                                   types[index].name);
            }
            else
            {
                format_expectation(&test.expected_semantics, 
                                   "file\n"
                                   "    path: %s\n"
                                   "    namespace: Abc\n"
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
                                   "        return_type: %s\n"
                                   "        return_statement\n"
                                   "            %s_literal\n"
                                   "                type: %s\n"
                                   "                value: 0\n"
                                   "    function\n"
                                   "        name: main\n"
                                   "        return_type: I32\n"
                                   "        return_statement\n"
                                   "            integer_literal\n"
                                   "                type: I32\n"
                                   "                value: 0\n",
                                   test.source_paths[0],
                                   types[index].name,
                                   types[index].file_friendly_default_value_name,
                                   types[index].name);
            }
        }

        index++;
        if (is_valid_case)
            return true;
    }

    return false;
}

static bool return_x_type_incompatible_literal(void)
{
    static uint8_t lhs_index = 0;
    static uint8_t rhs_index = 0;

    while (true)
    {
        if (lhs_index == LAST_TYPE_INDEX)
            return false;

        bool is_valid_case = types[rhs_index].file_friendly_default_value_name &&
                             types[rhs_index].default_value_name &&
                             (!types[lhs_index].default_value_name || strcmp(types[lhs_index].default_value_name, types[rhs_index].default_value_name)) &&
                             (!rhs_index || (types[rhs_index - 1].default_value_name && strcmp(types[rhs_index].default_value_name, types[rhs_index - 1].default_value_name)));

        if (is_valid_case)
        {
            format_test_name("semantics/statements/return/return_%s_type_%s_literal", types[lhs_index].file_friendly_name, types[rhs_index].file_friendly_default_value_name);

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
                                  "function a() : %s\n"
                                  "    return %s\n"
                                  "end", 
                                  types[lhs_index].name, 
                                  types[rhs_index].default_value);

            format_command_line_options("%s", test.source_paths[0]);
            format_expectation(&test.expected_error, 
                                "%s:25:12: Cannot infer literal as %s.\n",
                                test.source_paths[0], 
                                types[lhs_index].name);
        }

        if (rhs_index == LAST_TYPE_INDEX)
        {
            rhs_index = 0;
            lhs_index++;
        }
        else
            rhs_index++;

        if (is_valid_case)
            return true;
    }

    return false;
}

static bool x_typed_uninitialized_literal(void)
{    
    static uint8_t index = 0;
    if (index == TYPES_LENGTH)
        return false;

    format_test_name("semantics/statements/return/%s_typed_uninitialized_literal", types[index].file_friendly_name);

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
                          "function a() : %s\n"
                          "    return ---\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end", 
                          types[index].name);

    format_command_line_options("%s -print-semantics", test.source_paths[0]);

    if (index == POLYMORPHIC_UNION_TYPE_INDEX)
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
                           "        return_type: Union[I32]\n"
                           "        return_statement\n"
                           "            uninitialized_literal\n"
                           "                type: Union[I32]\n"
                           "    function\n"
                           "        name: main\n"
                           "        return_type: I32\n"
                           "        return_statement\n"
                           "            integer_literal\n"
                           "                type: I32\n"
                           "                value: 0\n",
                           test.source_paths[0]);
    }
    else if (index == POLYMORPHIC_STRUCTURE_TYPE_INDEX)
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
                           "        return_type: Structure[I32]\n"
                           "        return_statement\n"
                           "            uninitialized_literal\n"
                           "                type: Structure[I32]\n"
                           "    function\n"
                           "        name: main\n"
                           "        return_type: I32\n"
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
                           "        return_type: %s\n"
                           "        return_statement\n"
                           "            uninitialized_literal\n"
                           "                type: %s\n"
                           "    function\n"
                           "        name: main\n"
                           "        return_type: I32\n"
                           "        return_statement\n"
                           "            integer_literal\n"
                           "                type: I32\n"
                           "                value: 0\n",
                           test.source_paths[0],
                           types[index].name,
                           types[index].name);
    }

    index++;
    
    return true;
}

bool semantics_statements_return(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            polymorphic_and_monomorphic_return_types();
            state++;
            break;

        case 1:
            return_call_to_function_that_returns_multiple_values_matching_return_types();
            state++;
            break;

        case 2:
            return_call_to_function_that_returns_no_value();
            state++;
            break;

        case 3:
            return_multiple_values_too_few();
            state++;
            break;

        case 4:
            return_multiple_values_too_many();
            state++;
            break;

        case 5:
            return_one_too_few_values();
            state++;
            break;

        case 6:
            return_one_too_many_values();
            state++;
            break;

        case 7:
            if (single_expected_expression())
                break;

            state++;

        case 8:
            if (multiple_expected_expressions())
                break;

            state++;

        case 11:
            return_values_when_no_return_types_are_declared();
            state++;
            break;

        case 12:
            return_no_values_when_return_types_are_declared();
            state++;
            break;

        case 13:
            return_types_but_no_return_statement();
            state++;
            break;

        case 14:
            if (return_x_type_compatible_literal())
                break;

            state++;

        case 15:
            if (return_x_type_incompatible_literal())
                break;

            state++;

        case 16:
            if (x_typed_uninitialized_literal())
                break;

            state++;

        default:
            return false;
    }

    return true;
}
