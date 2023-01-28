#include "runner.h"

static void call_reference_that_is_not_a_function(void)
{
    format_test_name("semantics/expressions/call/monomorphic/call_reference_that_is_not_a_function");

    format_source_path(0, "semantics/expressions/call/monomorphic/call_reference_that_is_not_a_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b)\n"
                          "    b()\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/monomorphic/call_reference_that_is_not_a_function.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:5: Function type expected but found I32.\n",
                       test.source_paths[0]);
}

static void call_to_function_with_multiple_actual_parameters_matching_multiple_formal_parameters(void)
{
    format_test_name("semantics/expressions/call/monomorphic/call_to_function_with_multiple_actual_parameters_matching_multiple_formal_parameters");

    format_source_path(0, "semantics/expressions/call/monomorphic/call_to_function_with_multiple_actual_parameters_matching_multiple_formal_parameters.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b) : I32, I32\n"
                          "    return b, b\n"
                          "end\n"
                          "\n"
                          "function c(I32 d)\n"
                          "    d, d = a(d)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/monomorphic/call_to_function_with_multiple_actual_parameters_matching_multiple_formal_parameters.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/call/monomorphic/call_to_function_with_multiple_actual_parameters_matching_multiple_formal_parameters.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: b\n"
                       "        return_type: I32\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: b\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: c\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: d\n"
                       "        assignment_statement\n"
                       "            operator: =\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: d\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: d\n"
                       "            call_expression\n"
                       "                type: I32, I32\n"
                       "                reference_expression\n"
                       "                    type: Function(I32) : I32, I32\n"
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

static void call_to_function_with_multiple_actual_parameters_matching_the_formal_parameters(void)
{
    format_test_name("semantics/expressions/call/monomorphic/call_to_function_with_multiple_actual_parameters_matching_the_formal_parameters");

    format_source_path(0, "semantics/expressions/call/monomorphic/call_to_function_with_multiple_actual_parameters_matching_the_formal_parameters.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b, I32 c) : I32, I32\n"
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

    SET_COMMAND_LINE("semantics/expressions/call/monomorphic/call_to_function_with_multiple_actual_parameters_matching_the_formal_parameters.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/call/monomorphic/call_to_function_with_multiple_actual_parameters_matching_the_formal_parameters.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
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

static void call_to_function_with_one_actual_parameter_matching_one_formal_parameter(void)
{
    format_test_name("semantics/expressions/call/monomorphic/call_to_function_with_one_actual_parameter_matching_one_formal_parameter");

    format_source_path(0, "semantics/expressions/call/monomorphic/call_to_function_with_one_actual_parameter_matching_one_formal_parameter.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b) : I32\n"
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

    SET_COMMAND_LINE("semantics/expressions/call/monomorphic/call_to_function_with_one_actual_parameter_matching_one_formal_parameter.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/call/monomorphic/call_to_function_with_one_actual_parameter_matching_one_formal_parameter.owen\n"
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

static void call_to_function_with_zero_actual_parameters_matching_zero_formal_parameters(void)
{
    format_test_name("semantics/expressions/call/monomorphic/call_to_function_with_zero_actual_parameters_matching_zero_formal_parameters");

    format_source_path(0, "semantics/expressions/call/monomorphic/call_to_function_with_zero_actual_parameters_matching_zero_formal_parameters.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "end\n"
                          "\n"
                          "function b()\n"
                          "    a()\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/call/monomorphic/call_to_function_with_zero_actual_parameters_matching_zero_formal_parameters.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/call/monomorphic/call_to_function_with_zero_actual_parameters_matching_zero_formal_parameters.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "    function\n"
                       "        name: b\n"
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
                       "                value: 0\n" );
}

static void call_undefined_function(void)
{
    format_test_name("semantics/expressions/call/monomorphic/call_undefined_function");

    format_source_path(0, "semantics/expressions/call/monomorphic/call_undefined_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    b()\n"
                          "end\n");

    SET_COMMAND_LINE("semantics/expressions/call/monomorphic/call_undefined_function.owen");

    format_expectation(&test.expected_error, 
                       "%s:5:5: b is undefined.\n",
                       test.source_paths[0]);
}

static bool literal_as_actual_parameter(void)
{
    static uint8_t index = 0;
    while (index < TYPES_LENGTH && !types[index].default_value_name)
        index++;

    if (index == TYPES_LENGTH)
        return false;

    format_test_name("semantics/expressions/call/monomorphic/%s_literal_where_%s_typed_actual_parameter_expected", types[index].file_friendly_default_value_name, types[index].file_friendly_name);

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
                          "function a(%s b) : %s\n"
                          "    return b\n"
                          "end\n"
                          "\n"
                          "function c()\n"
                          "    a(%s)\n"
                          "end", 
                          types[index].name, types[index].name,
                          types[index].default_value);

    format_command_line_options("%s", test.source_paths[0]);
    format_expectation(&test.expected_error, 
                       "%s:25:7: %s type cannot be inferred.\n",
                       test.source_paths[0],
                       types[index].default_value_name);

    index++;
    return true;
}

static bool pass_x_typed_expression(void)
{
    static uint8_t actual_parameter = 0;
    static uint8_t formal_parameter = 0;

    if (actual_parameter == TYPES_LENGTH)
        return false;
    
    format_test_name("semantics/expressions/call/monomorphic/pass_%s_typed_expression_to_%s_typed_formal_parameter", types[actual_parameter].file_friendly_name, types[formal_parameter].file_friendly_name);

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
                          "function c(%s d) : %s\n"
                          "    return a(d)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end", 
                          types[formal_parameter].name, types[formal_parameter].name,
                          types[actual_parameter].name, types[formal_parameter].name);

    format_command_line_options("%s -print-semantics", test.source_paths[0]);
    if (is_noalias_type(types[formal_parameter].name))
    {
        format_expectation(&test.expected_error, 
                           "%s:24:%u: Only Pointerless formal parameter types can be noalias qualified.\n",
                           test.source_paths[0],
                           18 + strlen(types[formal_parameter].name));
    }
    else if (compatible_types(types[formal_parameter].name, types[actual_parameter].name))
    {
        if (formal_parameter == POLYMORPHIC_STRUCTURE_TYPE_INDEX)
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
                               "        name: c\n"
                               "        formal_parameter\n"
                               "            type: Structure[I32]\n"
                               "            name: d\n"
                               "        return_type: Structure[I32]\n"
                               "        return_statement\n"
                               "            call_expression\n"
                               "                type: Structure[I32]\n"
                               "                reference_expression\n"
                               "                    type: Function(Structure[I32]) : Structure[I32]\n"
                               "                    name: a\n"
                               "                reference_expression\n"
                               "                    type: Structure[I32]\n"
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
        else if (formal_parameter == POLYMORPHIC_UNION_TYPE_INDEX)
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
                               "        name: c\n"
                               "        formal_parameter\n"
                               "            type: Union[I32]\n"
                               "            name: d\n"
                               "        return_type: Union[I32]\n"
                               "        return_statement\n"
                               "            call_expression\n"
                               "                type: Union[I32]\n"
                               "                reference_expression\n"
                               "                    type: Function(Union[I32]) : Union[I32]\n"
                               "                    name: a\n"
                               "                reference_expression\n"
                               "                    type: Union[I32]\n"
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
                               "        name: c\n"
                               "        formal_parameter\n"
                               "            type: %s\n"
                               "            name: d\n"
                               "        return_type: %s\n"
                               "        return_statement\n"
                               "            call_expression\n"
                               "                type: %s\n"
                               "                reference_expression\n"
                               "                    type: Function(%s) : %s\n"
                               "                    name: a\n"
                               "                reference_expression\n"
                               "                    type: %s\n"
                               "                    name: d\n"
                               "    function\n"
                               "        name: main\n"
                               "        return_type: I32\n"
                               "        return_statement\n"
                               "            integer_literal\n"
                               "                type: I32\n"
                               "                value: 0\n",
                               test.source_paths[0],
                               types[formal_parameter].name,
                               types[formal_parameter].name,
                               types[formal_parameter].name,
                               types[actual_parameter].name,
                               types[formal_parameter].name,
                               types[formal_parameter].name,
                               types[formal_parameter].name, types[formal_parameter].name,
                               types[actual_parameter].name);
        }
    }
    else
    {
        format_expectation(&test.expected_error, 
                           "%s:29:12: No matching signature for a.\n"
                           "Declarations:\n"
                           "    %s:24:10: a(%s b) : %s\n",
                           test.source_paths[0],
                           test.source_paths[0],
                           types[formal_parameter].name, 
                           types[formal_parameter].name);
        
    }
    
    if (formal_parameter == LAST_TYPE_INDEX)
    {
        formal_parameter = 0;
        actual_parameter++;
    }
    else
        formal_parameter++;

    return true;
}

bool semantics_expressions_call_monomorphic(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            call_reference_that_is_not_a_function();
            state++;
            break;

        case 1:
            call_to_function_with_multiple_actual_parameters_matching_multiple_formal_parameters();
            state++;
            break;

        case 2:
            call_to_function_with_multiple_actual_parameters_matching_the_formal_parameters();
            state++;
            break;

        case 3:
            call_to_function_with_one_actual_parameter_matching_one_formal_parameter();
            state++;
            break;

        case 4:
            call_to_function_with_zero_actual_parameters_matching_zero_formal_parameters();
            state++;
            break;

        case 5:
            call_undefined_function();
            state++;
            break;

        case 6:
            if (literal_as_actual_parameter())
                break;

            state++;

        case 7:
            if (pass_x_typed_expression())
                break;

            state++;

        default:
            return false;
    }

    return true;
}
