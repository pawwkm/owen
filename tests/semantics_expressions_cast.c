#include "runner.h"

static bool is_valid_cast(uint8_t source_index, uint8_t target_index)
{
    if (source_index == READONLY_NOALIAS_TYPE_INDEX || 
        source_index == NOALIAS_TYPE_INDEX ||
        target_index == READONLY_NOALIAS_TYPE_INDEX || 
        target_index == NOALIAS_TYPE_INDEX)
        return false;
    
    return source_index >= FIRST_INTEGER_TYPE_INDEX && source_index <= LAST_INTEGER_TYPE_INDEX && target_index >= FIRST_INTEGER_TYPE_INDEX && target_index <= LAST_INTEGER_TYPE_INDEX ||
           source_index >= FIRST_FLOAT_TYPE_INDEX && source_index <= LAST_FLOAT_TYPE_INDEX && target_index >= FIRST_FLOAT_TYPE_INDEX && target_index <= LAST_FLOAT_TYPE_INDEX ||
           source_index >= FIRST_FLOAT_TYPE_INDEX && source_index <= LAST_FLOAT_TYPE_INDEX && target_index >= FIRST_INTEGER_TYPE_INDEX && target_index <= LAST_INTEGER_TYPE_INDEX ||
           source_index >= FIRST_INTEGER_TYPE_INDEX && source_index <= LAST_INTEGER_TYPE_INDEX && target_index >= FIRST_FLOAT_TYPE_INDEX && target_index <= LAST_FLOAT_TYPE_INDEX ||
           source_index >= FIRST_INTEGER_TYPE_INDEX && source_index <= LAST_INTEGER_TYPE_INDEX && target_index == ENUMERATION_TYPE_INDEX ||
           target_index >= FIRST_INTEGER_TYPE_INDEX && target_index <= LAST_INTEGER_TYPE_INDEX && source_index == ENUMERATION_TYPE_INDEX ||
           source_index >= FIRST_FLOAT_TYPE_INDEX && source_index <= LAST_FLOAT_TYPE_INDEX && target_index == ENUMERATION_TYPE_INDEX ||
           target_index >= FIRST_FLOAT_TYPE_INDEX && target_index <= LAST_FLOAT_TYPE_INDEX && source_index == ENUMERATION_TYPE_INDEX ||
           source_index == ENUMERATION_TYPE_INDEX && target_index == ENUMERATION_TYPE_INDEX;
}

static bool cast_non_function_type_to_non_function_type(void)
{
    static uint8_t source_index = 0;
    static uint8_t target_index = 0;

    if (target_index == LAST_TYPE_INDEX)
        return false;

    format_test_name("semantics/expressions/cast/cast_%s_to_%s", types[source_index].file_friendly_name, types[target_index].file_friendly_name);

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
                          "    return %s(b)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end",
                          types[source_index].name,
                          types[target_index].name,
                          types[target_index].name);

    format_command_line_options("%s -print-semantics", test.source_paths[0]);

    bool is_valid_case = is_valid_cast(source_index, target_index);
    if (is_valid_case)
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
                           "            cast\n"
                           "                type: %s\n"
                           "                reference_expression\n"
                           "                    type: %s\n"
                           "                    name: b\n"
                           "    function\n"
                           "        name: main\n"
                           "        return_type: I32\n"
                           "        return_statement\n"
                           "            integer_literal\n"
                           "                type: I32\n"
                           "                value: 0\n",
                           test.source_paths[0],
                           types[source_index].name,
                           types[target_index].name,
                           types[target_index].name,
                           types[source_index].name);
    }
    else if (is_noalias_type(types[target_index].name))
    {
        format_expectation(&test.expected_error,
                           "%s:24:%u: Only Pointerless formal parameter types can be noalias qualified.\n",
                           test.source_paths[0],
                           18 + strlen(types[source_index].name));
    }
    else
    {
        format_expectation(&test.expected_error,
                           "%s:25:12: Cannot cast %s to %s.\n",
                           test.source_paths[0],
                           types[source_index].name,
                           types[target_index].name);
    }

    if (source_index == LAST_TYPE_INDEX)
    {
        target_index++;
        source_index = 0;
    }
    else
        source_index++;

    return true;
}

static void polymorphic_types(void)
{
    format_test_name("semantics/expressions/cast/polymorphic_types");

    format_source_path(0, "semantics/expressions/cast/polymorphic_types.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[P, R](P p) : R\n"
                          "    return R(p)\n"
                          "end\n"
                          "\n"
                          "function b(I8 c) : I32\n"
                          "    return a[I8, I32](c)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");
    
    format_command_line_options("semantics/expressions/cast/polymorphic_types.owen -print-semantics");
    format_expectation(&test.expected_semantics,
                       "file\n"
                       "    path: semantics/expressions/cast/polymorphic_types.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: P\n"
                       "        formal_type_parameter: R\n"
                       "        formal_parameter\n"
                       "            type_reference: P\n"
                       "            name: p\n"
                       "        return_type_reference: R\n"
                       "        return_statement\n"
                       "            cast\n"
                       "                type_reference: R\n"
                       "                reference_expression\n"
                       "                    name: p\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I8\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: I8\n"
                       "            name: p\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            cast\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: I8\n"
                       "                    name: p\n"
                       "    function\n"
                       "        name: b\n"
                       "        formal_parameter\n"
                       "            type: I8\n"
                       "            name: c\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: Function(I8) : I32\n"
                       "                    name: a\n"
                       "                    actual_type_parameter: I8\n"
                       "                    actual_type_parameter: I32\n"
                       "                reference_expression\n"
                       "                    type: I8\n"
                       "                    name: c\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n" );
}

static bool cast_literal_to_x(void)
{
    static uint8_t index = 0;
    while (true)
    {
        if (index == LAST_TYPE_INDEX)
            return false;

        bool is_valid_case = types[index].default_value && is_valid_cast(index, index);
        if (is_valid_case)
        {
            format_test_name("semantics/expressions/cast/cast_%s_literal_to_%s", types[index].file_friendly_name, types[index].file_friendly_name);

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
                                  "    return %s(%s)\n"
                                  "end\n"
                                  "\n"
                                  "function main() : I32\n"
                                  "    return 0\n"
                                  "end",
                                  types[index].name,
                                  types[index].name,
                                  types[index].default_value);

            format_command_line_options("%s -print-semantics", test.source_paths[0]);
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
                               "            cast\n"
                               "                type: %s\n"
                               "                %s_literal\n"
                               "                    type: %s\n"
                               "                    value: 0\n"
                               "    function\n"
                               "        name: main\n"
                               "        return_type: I32\n"
                               "        return_statement\n"
                               "            integer_literal\n"
                               "                type: I32\n"
                               "                value: 0\n",
                               test.source_paths[0],
                               types[index].name,
                               types[index].name,
                               types[index].file_friendly_default_value_name,
                               types[index].name);
        }

        index++;
        if (is_valid_case)
            return true;
    }

    return false;
}

static void matching_monomorphic_function(void)
{
    format_test_name("semantics/expressions/cast/matching_monomorphic_function");

    format_source_path(0, "semantics/expressions/cast/matching_monomorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b) : I32\n"
                          "    return b\n"
                          "end\n"
                          "\n"
                          "function c() : Function(I32) : I32\n"
                          "    return Function(I32) : I32(a)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");
    
    format_command_line_options("semantics/expressions/cast/matching_monomorphic_function.owen -print-semantics");
    format_expectation(&test.expected_semantics,
                       "file\n"
                       "    path: semantics/expressions/cast/matching_monomorphic_function.owen\n"
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
                       "        return_type: Function(I32) : I32\n"
                       "        return_statement\n"
                       "            cast\n"
                       "                type: Function(I32) : I32\n"
                       "                reference_expression\n"
                       "                    type: Function(I32) : I32\n"
                       "                    name: a\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void matching_polymorphic_function(void)
{
    format_test_name("semantics/expressions/cast/matching_polymorphic_function");

    format_source_path(0, "semantics/expressions/cast/matching_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T](T b) : T\n"
                          "    return b\n"
                          "end\n"
                          "\n"
                          "function c() : Function(I32) : I32\n"
                          "    return Function(I32) : I32(a)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");
    
    format_command_line_options("semantics/expressions/cast/matching_polymorphic_function.owen -print-semantics");
    format_expectation(&test.expected_semantics,
                       "file\n"
                       "    path: semantics/expressions/cast/matching_polymorphic_function.owen\n"
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
                       "        return_type: Function(I32) : I32\n"
                       "        return_statement\n"
                       "            cast\n"
                       "                type: Function(I32) : I32\n"
                       "                reference_expression\n"
                       "                    type: Function(I32) : I32\n"
                       "                    name: a\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void formal_type_parameter_nested_in_compound(void)
{
    format_test_name("semantics/expressions/cast/formal_type_parameter_nested_in_compound");

    format_source_path(0, "semantics/expressions/cast/formal_type_parameter_nested_in_compound.owen");
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
                          "function c() : Function(A[I32]) : I32\n"
                          "    return Function(A[I32]) : I32(a)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");
    
    format_command_line_options("semantics/expressions/cast/formal_type_parameter_nested_in_compound.owen -print-semantics");
    format_expectation(&test.expected_semantics,
                       "file\n"
                       "    path: semantics/expressions/cast/formal_type_parameter_nested_in_compound.owen\n"
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
                       "        return_type: Function(A[I32]) : I32\n"
                       "        return_statement\n"
                       "            cast\n"
                       "                type: Function(A[I32]) : I32\n"
                       "                reference_expression\n"
                       "                    type: Function(A[I32]) : I32\n"
                       "                    name: a\n"
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
    format_test_name("semantics/expressions/cast/formal_type_parameter_not_specified_by_actual_parameters");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T1, T2](T1 b) : T2\n"
                          "    return b + 2\n"
                          "end\n"
                          "\n"
                          "function c() : Function(I32) : I32\n"
                          "    return Function(I32) : I32(a)\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/cast/formal_type_parameter_not_specified_by_actual_parameters.owen");

    format_expectation(&test.expected_error, 
                       "%s:9:32: T2 could not be inferred from the call site. Specify it explicitly.\n",
                       test.source_paths[0]);
}

static void formal_type_parameter_redefined_by_actual_parameter(void)
{
    format_test_name("semantics/expressions/cast/formal_type_parameter_redefined_by_actual_parameter");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T](T b, T c) : T\n"
                          "    return b + c\n"
                          "end\n"
                          "\n"
                          "function d(I32 e, F32 f) : Function(I32, F32) : I32\n"
                          "    return Function(I32, F32) : I32(a)\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/cast/formal_type_parameter_redefined_by_actual_parameter.owen");

    format_expectation(&test.expected_error, 
                       "%s:9:37: T redefined as F32. It was originally I32.\n",
                       test.source_paths[0]);
}

bool semantics_expressions_cast(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            if (cast_non_function_type_to_non_function_type())
                break;

            state++;

        case 1:
            polymorphic_types();
            state++;
            break;

        case 2:
            if (cast_literal_to_x())
                break;

            state++;

        case 3:
            matching_monomorphic_function();
            state++;
            break;

        case 4:
            matching_polymorphic_function();
            state++;
            break;

        case 5:
            formal_type_parameter_nested_in_compound();
            state++;
            break;

        case 6:
            formal_type_parameter_not_specified_by_actual_parameters();
            state++;
            break;

        case 7:
            formal_type_parameter_redefined_by_actual_parameter();
            state++;
            break;

        default:
            return false;
    }

    return true;
}
