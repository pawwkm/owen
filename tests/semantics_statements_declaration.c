#include "runner.h"

static bool x_typed_variable_equal_none_typed_expression(void)
{    
    static uint8_t index = 0;
    if (index == TYPES_LENGTH)
        return false;

    format_test_name("semantics/statements/declaration/%s_typed_variable_equal_none_typed_expression", types[index].file_friendly_name);

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
                          "function a()\n"
                          "end\n"
                          "\n"
                          "function b()\n"
                          "    %s c = a()\n"
                          "end", 
                          types[index].name);

    format_command_line_options(test.source_paths[0]);

    if (is_noalias_type(types[index].name))
    {
        format_expectation(&test.expected_error, 
                           "%s:28:5: Only Pointerless formal parameter types can be noalias qualified.\n",
                           test.source_paths[0]);
    }
    else
    {
        format_expectation(&test.expected_error, 
                           "%s:28:%u: %s expected but found none.\n",
                           test.source_paths[0],
                           11 + strlen(types[index].name),
                           types[index].name);
    }
    
    index++;
    
    return true;
}

static bool pointer_variables_equal_compatible_tuple(void)
{
    static uint8_t variable_type_index = FIRST_POINTER_TYPE_INDEX;
    static uint8_t tuple_type_index = FIRST_POINTER_TYPE_INDEX;

    while (true)
    {
        if (variable_type_index > LAST_POINTER_TYPE_INDEX)
            return false;

        bool is_valid_case = compatible_types(types[variable_type_index].name, types[tuple_type_index].name) &&
                             !is_noalias_type(types[variable_type_index].name);
        if (is_valid_case)
        {
            format_test_name("semantics/statements/declaration/%s_typed_variables_equal_%s_typed_tuple", types[variable_type_index].file_friendly_name, types[tuple_type_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "function a(%s b) : %s, %s\n"
                                  "    return b, b\n"
                                  "end\n"
                                  "\n"
                                  "function c(%s d) : %s, %s\n"
                                  "    %s e, %s f = a(d)\n"
                                  "    return e, f\n"
                                  "end\n"
                                  "\n"
                                  "function main() : I32\n"
                                  "    return 0\n"
                                  "end",
                                  types[tuple_type_index].name, types[tuple_type_index].name, types[tuple_type_index].name,
                                  types[tuple_type_index].name, types[variable_type_index].name, types[variable_type_index].name,
                                  types[variable_type_index].name, types[variable_type_index].name);

            format_command_line_options("%s -print-semantics", test.source_paths[0]);
            format_expectation(&test.expected_semantics,
                               "file\n"
                               "    path: %s\n"
                               "    namespace: Abc\n"
                               "    function\n"
                               "        name: a\n"
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
                               "        name: c\n"
                               "        formal_parameter\n"
                               "            type: %s\n"
                               "            name: d\n"
                               "        return_type: %s\n"
                               "        return_type: %s\n"
                               "        declaration_statement\n"
                               "            variable\n"
                               "                type: %s\n"
                               "                name: e\n"
                               "            variable\n"
                               "                type: %s\n"
                               "                name: f\n"
                               "            call_expression\n"
                               "                type: %s, %s\n"
                               "                reference_expression\n"
                               "                    type: Function(%s) : %s, %s\n"
                               "                    name: a\n"
                               "                reference_expression\n"
                               "                    type: %s\n"
                               "                    name: d\n"
                               "        return_statement\n"
                               "            reference_expression\n"
                               "                type: %s\n"
                               "                name: e\n"
                               "            reference_expression\n"
                               "                type: %s\n"
                               "                name: f\n"
                               "    function\n"
                               "        name: main\n"
                               "        return_type: I32\n"
                               "        return_statement\n"
                               "            integer_literal\n"
                               "                type: I32\n"
                               "                value: 0\n",
                               test.source_paths[0],
                               types[tuple_type_index].name,
                               types[tuple_type_index].name,
                               types[tuple_type_index].name,
                               types[tuple_type_index].name,
                               types[tuple_type_index].name,
                               types[tuple_type_index].name,
                               types[variable_type_index].name,
                               types[variable_type_index].name,
                               types[variable_type_index].name,
                               types[variable_type_index].name,
                               types[tuple_type_index].name, types[tuple_type_index].name,
                               types[tuple_type_index].name, types[tuple_type_index].name, types[tuple_type_index].name,
                               types[tuple_type_index].name,
                               types[variable_type_index].name,
                               types[variable_type_index].name);
        }

        if (tuple_type_index == LAST_POINTER_TYPE_INDEX)
        {
            tuple_type_index = FIRST_POINTER_TYPE_INDEX;
            variable_type_index++;
        }
        else
            tuple_type_index++;

        if (is_valid_case)
            return true;
    }

    return false;
}

static bool non_pointer_variables_equal_compatible_tuple(void)
{
    static uint8_t type_index = 0;
    if (type_index == FIRST_POINTER_TYPE_INDEX)
        return false;

    format_test_name("semantics/statements/declaration/%s_typed_variables_equal_%s_typed_tuple", types[type_index].file_friendly_name, types[type_index].file_friendly_name);

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
                           "function a(%s b) : %s, %s\n"
                           "    return b, b\n"
                           "end\n"
                           "\n"
                           "function c(%s d) : %s, %s\n"
                           "    %s e, %s f = a(d)\n"
                           "    return e, f\n"
                           "end\n"
                           "\n"
                           "function main() : I32\n"
                           "    return 0\n"
                           "end",
                           types[type_index].name, types[type_index].name, types[type_index].name,
                           types[type_index].name, types[type_index].name, types[type_index].name,
                           types[type_index].name, types[type_index].name);

    format_command_line_options("%s -print-semantics", test.source_paths[0]);
    if (type_index == POLYMORPHIC_STRUCTURE_TYPE_INDEX)
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
                           "        return_type: Structure[I32]\n"
                           "        return_statement\n"
                           "            reference_expression\n"
                           "                type: Structure[I32]\n"
                           "                name: b\n"
                           "            reference_expression\n"
                           "                type: Structure[I32]\n"
                           "                name: b\n"
                           "    function\n"
                           "        name: c\n"
                           "        formal_parameter\n"
                           "            type: Structure[I32]\n"
                           "            name: d\n"
                           "        return_type: Structure[I32]\n"
                           "        return_type: Structure[I32]\n"
                           "        declaration_statement\n"
                           "            variable\n"
                           "                type: Structure[I32]\n"
                           "                name: e\n"
                           "            variable\n"
                           "                type: Structure[I32]\n"
                           "                name: f\n"
                           "            call_expression\n"
                           "                type: Structure[I32], Structure[I32]\n"
                           "                reference_expression\n"
                           "                    type: Function(Structure[I32]) : Structure[I32], Structure[I32]\n"
                           "                    name: a\n"
                           "                reference_expression\n"
                           "                    type: Structure[I32]\n"
                           "                    name: d\n"
                           "        return_statement\n"
                           "            reference_expression\n"
                           "                type: Structure[I32]\n"
                           "                name: e\n"
                           "            reference_expression\n"
                           "                type: Structure[I32]\n"
                           "                name: f\n"
                           "    function\n"
                           "        name: main\n"
                           "        return_type: I32\n"
                           "        return_statement\n"
                           "            integer_literal\n"
                           "                type: I32\n"
                           "                value: 0\n",
                           test.source_paths[0]);
    }
    else if (type_index == POLYMORPHIC_UNION_TYPE_INDEX)
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
                           "        return_type: Union[I32]\n"
                           "        return_statement\n"
                           "            reference_expression\n"
                           "                type: Union[I32]\n"
                           "                name: b\n"
                           "            reference_expression\n"
                           "                type: Union[I32]\n"
                           "                name: b\n"
                           "    function\n"
                           "        name: c\n"
                           "        formal_parameter\n"
                           "            type: Union[I32]\n"
                           "            name: d\n"
                           "        return_type: Union[I32]\n"
                           "        return_type: Union[I32]\n"
                           "        declaration_statement\n"
                           "            variable\n"
                           "                type: Union[I32]\n"
                           "                name: e\n"
                           "            variable\n"
                           "                type: Union[I32]\n"
                           "                name: f\n"
                           "            call_expression\n"
                           "                type: Union[I32], Union[I32]\n"
                           "                reference_expression\n"
                           "                    type: Function(Union[I32]) : Union[I32], Union[I32]\n"
                           "                    name: a\n"
                           "                reference_expression\n"
                           "                    type: Union[I32]\n"
                           "                    name: d\n"
                           "        return_statement\n"
                           "            reference_expression\n"
                           "                type: Union[I32]\n"
                           "                name: e\n"
                           "            reference_expression\n"
                           "                type: Union[I32]\n"
                           "                name: f\n"
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
                           "        return_type: %s\n"
                           "        return_statement\n"
                           "            reference_expression\n"
                           "                type: %s\n"
                           "                name: b\n"
                           "            reference_expression\n"
                           "                type: %s\n"
                           "                name: b\n"
                           "    function\n"
                           "        name: c\n"
                           "        formal_parameter\n"
                           "            type: %s\n"
                           "            name: d\n"
                           "        return_type: %s\n"
                           "        return_type: %s\n"
                           "        declaration_statement\n"
                           "            variable\n"
                           "                type: %s\n"
                           "                name: e\n"
                           "            variable\n"
                           "                type: %s\n"
                           "                name: f\n"
                           "            call_expression\n"
                           "                type: %s, %s\n"
                           "                reference_expression\n"
                           "                    type: Function(%s) : %s, %s\n"
                           "                    name: a\n"
                           "                reference_expression\n"
                           "                    type: %s\n"
                           "                    name: d\n"
                           "        return_statement\n"
                           "            reference_expression\n"
                           "                type: %s\n"
                           "                name: e\n"
                           "            reference_expression\n"
                           "                type: %s\n"
                           "                name: f\n"
                           "    function\n"
                           "        name: main\n"
                           "        return_type: I32\n"
                           "        return_statement\n"
                           "            integer_literal\n"
                           "                type: I32\n"
                           "                value: 0\n",
                           test.source_paths[0],
                           types[type_index].name,
                           types[type_index].name,
                           types[type_index].name,
                           types[type_index].name,
                           types[type_index].name,
                           types[type_index].name,
                           types[type_index].name,
                           types[type_index].name,
                           types[type_index].name,
                           types[type_index].name,
                           types[type_index].name, types[type_index].name,
                           types[type_index].name, types[type_index].name, types[type_index].name,
                           types[type_index].name,
                           types[type_index].name,
                           types[type_index].name);
    }

    type_index++;

    return true;
}

static bool non_pointer_variables_equal_incompatible_tuple(void)
{
    static uint8_t lhs_type_index = 0;
    static uint8_t rhs_type_index = 0;

    while (true)
    {
        if (lhs_type_index == FIRST_POINTER_TYPE_INDEX)
            return false;

        bool is_valid_case = lhs_type_index != rhs_type_index;
        if (is_valid_case)
        {
            format_test_name("semantics/statements/declaration/%s_typed_variables_equal_%s_typed_tuple", types[lhs_type_index].file_friendly_name, types[rhs_type_index].file_friendly_name);

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
                                  "function a(%s b) : %s, %s\n"
                                  "    return b, b\n"
                                  "end\n"
                                  "\n"
                                  "function c(%s d) : %s, %s\n"
                                  "    %s e, %s f = a(d)\n"
                                  "    return e, f\n"
                                  "end",
                                  types[rhs_type_index].name, types[rhs_type_index].name, types[rhs_type_index].name,
                                  types[rhs_type_index].name, types[lhs_type_index].name, types[lhs_type_index].name,
                                  types[lhs_type_index].name, types[lhs_type_index].name);

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error,
                               "%s:29:%u: %s, %s expected but found %s, %s.\n",
                               test.source_paths[0],
                               15 + strlen(types[lhs_type_index].name) * 2,
                               types[lhs_type_index].name, 
                               types[lhs_type_index].name,
                               types[rhs_type_index].name, 
                               types[rhs_type_index].name);
        }
        
        if (rhs_type_index + 1 == FIRST_POINTER_TYPE_INDEX)
        {
            rhs_type_index = 0;
            lhs_type_index++;
        }
        else
            rhs_type_index++;
        
        if (is_valid_case)
            return true;
    }
}

static void multiple_expressions_too_many_on_lhs_of_operator(void)
{
    format_test_name("semantics/statements/declaration/multiple_expressions_too_many_on_lhs_of_equal");

    format_source_path(0, "semantics/statements/declaration/multiple_expressions_too_many_on_lhs_of_equal.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                           "namespace Abc\n"
                           "\n"
                           "function a(I32 b)\n"
                           "    I32 c, I32 d, I32 e, I32 f = b, b\n"
                           "end");

    SET_COMMAND_LINE("semantics/statements/declaration/multiple_expressions_too_many_on_lhs_of_equal.owen");
    format_expectation(&test.expected_error,
                       "%s:5:37: 4 expressions expected but found 2.\n",
                       test.source_paths[0]);
}

static void multiple_expressions_too_many_on_rhs_of_equal(void)
{
    format_test_name("semantics/statements/declaration/multiple_expressions_too_many_on_rhs_of_equal");

    format_source_path(0, "semantics/statements/declaration/multiple_expressions_too_many_on_rhs_of_equal.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                           "namespace Abc\n"
                           "\n"
                           "function a(I32 b)\n"
                           "    I32 c = b, b, b\n"
                           "end");

    SET_COMMAND_LINE("semantics/statements/declaration/multiple_expressions_too_many_on_rhs_of_equal.owen");
    format_expectation(&test.expected_error,
                       "%s:5:16: 1 expression expected but found 3.\n",
                       test.source_paths[0]);
}

static void one_expression_too_many_on_lhs_of_operator(void)
{
    format_test_name("semantics/statements/declaration/one_expression_too_many_on_lhs_of_equal");

    format_source_path(0, "semantics/statements/declaration/one_expression_too_many_on_lhs_of_equal.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                           "namespace Abc\n"
                           "\n"
                           "function a(I32 b)\n"
                           "    I32 c, I32 d, I32 e = b, b\n"
                           "end");

    SET_COMMAND_LINE("semantics/statements/declaration/one_expression_too_many_on_lhs_of_equal.owen");
    format_expectation(&test.expected_error,
                       "%s:5:30: 3 expressions expected but found 2.\n",
                       test.source_paths[0]);
}

static void one_expression_too_many_on_rhs_of_operator(void)
{
    format_test_name("semantics/statements/declaration/one_expression_too_many_on_rhs_of_equal");

    format_source_path(0, "semantics/statements/declaration/one_expression_too_many_on_rhs_of_equal.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                           "namespace Abc\n"
                           "\n"
                           "function a(I32 b)\n"
                           "    I32 c = b, b\n"
                           "end");

    SET_COMMAND_LINE("semantics/statements/declaration/one_expression_too_many_on_rhs_of_equal.owen");
    format_expectation(&test.expected_error,
                       "%s:5:13: 1 expression expected but found 2.\n",
                       test.source_paths[0]);
}

static bool pointer_variable_equal_incompatible_pointer(void)
{
    static uint8_t expression_type_index = FIRST_POINTER_TYPE_INDEX;
    static uint8_t variable_type_index = FIRST_POINTER_TYPE_INDEX;

    while (true)
    {
        if (expression_type_index > LAST_POINTER_TYPE_INDEX)
            return false;

        bool is_valid_case = !compatible_types(types[variable_type_index].name, types[expression_type_index].name);
        if (is_valid_case)
        {
            format_test_name("semantics/statements/declaration/%s_typed_variable_equal_%s_typed_expression", types[variable_type_index].file_friendly_name, types[expression_type_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "function a(%s c)\n"
                                  "    %s b = c\n"
                                  "end", 
                                  types[expression_type_index].name,
                                  types[variable_type_index].name);

            format_command_line_options("%s", test.source_paths[0]);
            if (is_noalias_type(types[variable_type_index].name))
            {
                format_expectation(&test.expected_error, 
                                   "%s:5:5: Only Pointerless formal parameter types can be noalias qualified.\n",
                                   test.source_paths[0]);
            }
            else 
            {
                format_expectation(&test.expected_error, 
                                   "%s:5:%u: %s expected but found %s.\n",
                                   test.source_paths[0],
                                   10 + strlen(types[variable_type_index].name),
                                   types[variable_type_index].name,
                                   types[expression_type_index].name);
            }
            
        }

        if (variable_type_index == LAST_POINTER_TYPE_INDEX)
        {
            variable_type_index = FIRST_POINTER_TYPE_INDEX;
            expression_type_index++;
        }
        else
            variable_type_index++;

        if (is_valid_case)
            return true;
    }

    return false;
}

static bool x_typed_variable_equal_x_typed_expression(void)
{
    static uint8_t type_index = 0;
    if (type_index == TYPES_LENGTH)
        return false;

    format_test_name("semantics/statements/declaration/%s_typed_variable_equal_%s_typed_expression", types[type_index].file_friendly_name, types[type_index].file_friendly_name);
    
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
                          "function a(%s c) : %s\n"
                          "    %s b = c\n"
                          "    return b\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end",
                          types[type_index].name, types[type_index].name,
                          types[type_index].name);

    format_command_line_options("%s -print-semantics", test.source_paths[0]);
    if (type_index == POLYMORPHIC_UNION_TYPE_INDEX)
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
                           "            name: c\n"
                           "        return_type: Union[I32]\n"
                           "        declaration_statement\n"
                           "            variable\n"
                           "                type: Union[I32]\n"
                           "                name: b\n"
                           "            reference_expression\n"
                           "                type: Union[I32]\n"
                           "                name: c\n"
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
    else if (type_index == POLYMORPHIC_STRUCTURE_TYPE_INDEX)
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
                           "            name: c\n"
                           "        return_type: Structure[I32]\n"
                           "        declaration_statement\n"
                           "            variable\n"
                           "                type: Structure[I32]\n"
                           "                name: b\n"
                           "            reference_expression\n"
                           "                type: Structure[I32]\n"
                           "                name: c\n"
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
    else if (is_noalias_type(types[type_index].name))
    {
        format_expectation(&test.expected_error, 
                           "%s:24:%u: Only Pointerless formal parameter types can be noalias qualified.\n",
                           test.source_paths[0],
                           18 + strlen(types[type_index].name));
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
                           "            name: c\n"
                           "        return_type: %s\n"
                           "        declaration_statement\n"
                           "            variable\n"
                           "                type: %s\n"
                           "                name: b\n"
                           "            reference_expression\n"
                           "                type: %s\n"
                           "                name: c\n"
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
                           types[type_index].name,
                           types[type_index].name,
                           types[type_index].name,
                           types[type_index].name,
                           types[type_index].name);
    }

    type_index++;
    return true;
}

static bool non_pointer_x_typed_variable_equal_non_pointer_y_typed_expression(void)
{
    static uint8_t lhs_type_index = 0;
    static uint8_t rhs_type_index = 0;

    while (true)
    {
        if (lhs_type_index == FIRST_POINTER_TYPE_INDEX)
            return false;

        bool is_valid_case = lhs_type_index != rhs_type_index;
        if (is_valid_case)
        {
            format_test_name("semantics/statements/declaration/%s_typed_variable_equal_%s_typed_expression", types[lhs_type_index].file_friendly_name,
                                                                                                             types[rhs_type_index].file_friendly_name);

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
                                  "function a(%s c)\n"
                                  "    %s b = c\n"
                                  "end",
                                  types[rhs_type_index].name, 
                                  types[lhs_type_index].name);

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error,
                               "%s:25:%u: %s expected but found %s.\n",
                               test.source_paths[0],
                               10 + strlen(types[lhs_type_index].name),
                               types[lhs_type_index].name, 
                               types[rhs_type_index].name);
        }

        if (rhs_type_index + 1 == FIRST_POINTER_TYPE_INDEX)
        {
            rhs_type_index = 0;
            lhs_type_index++;
        }
        else
            rhs_type_index++;

        if (is_valid_case)
            return true;
    }
}

static void variable_with_the_same_name_as_a_function(void)
{
    format_test_name("semantics/statements/declaration/declare_variable_with_the_same_name_as_a_function");

    format_source_path(0, "semantics/statements/declaration/declare_variable_with_the_same_name_as_a_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "end\n"
                          "\n"
                          "function b() : I32\n"
                          "    I32 a = 2\n"
                          "    return a\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/declaration/declare_variable_with_the_same_name_as_a_function.owen");
    format_expectation(&test.expected_error,
                       "%s:8:9: a is already defined.\n",
                        test.source_paths[0]);
}

static void variable_with_the_same_name_as_formal_parameter(void)
{
    format_test_name("semantics/statements/declaration/variable_with_the_same_name_as_formal_parameter");

    format_source_path(0, "semantics/statements/declaration/variable_with_the_same_name_as_formal_parameter.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b) : Bool\n"
                          "    Bool b = false\n"
                          "    return b\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/declaration/variable_with_the_same_name_as_formal_parameter.owen");
    format_expectation(&test.expected_error,
                       "%s:5:10: b is already defined.\n",
                        test.source_paths[0]);
}

static void actual_type_parameter_as_variable_type(void)
{
    format_test_name("semantics/statements/declaration/actual_type_parameter_as_variable_type");

    format_source_path(0, "semantics/statements/declaration/actual_type_parameter_as_variable_type.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T]() : T\n"
                          "    T b = 0\n"
                          "    return b\n"
                          "end\n"
                          "\n"
                          "function c() : I32\n"
                          "    return a[I32]()\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/declaration/actual_type_parameter_as_variable_type.owen " 
                     "-print-semantics");

    format_expectation(&test.expected_semantics,
                       "file\n"
                       "    path: semantics/statements/declaration/actual_type_parameter_as_variable_type.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        return_type_reference: T\n"
                       "        declaration_statement\n"
                       "            variable\n"
                       "                type_reference: T\n"
                       "                name: b\n"
                       "            integer_literal\n"
                       "                value: 0\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
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
                       "        name: c\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: Function() : I32\n"
                       "                    name: a\n"
                       "                    actual_type_parameter: I32\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static bool x_typed_variable_equal_compatible_literal(void)
{
    static uint8_t index = 0;
    while (true)
    {
        if (index == TYPES_LENGTH)
            return false;

        bool is_valid_case = types[index].file_friendly_default_value_name != NULL;
        if (is_valid_case)
        {
            format_test_name("semantics/statements/declaration/%s_type_%s_variable_equal_%s_typed_literal", types[index].file_friendly_name, types[index].file_friendly_default_value_name, types[index].file_friendly_name);

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
                                  "    %s b = %s\n"
                                  "    return b\n"
                                  "end\n"
                                  "\n"
                                  "function main() : I32\n"
                                  "    return 0\n"
                                  "end", 
                                  types[index].name,
                                  types[index].name, types[index].default_value);

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
                                   "        declaration_statement\n"
                                   "            variable\n"
                                   "                type: Structure[I32]\n"
                                   "                name: b\n"
                                   "            compound_literal\n"
                                   "                type: Structure[I32]\n"
                                   "                field_initializer\n"
                                   "                    type: I32\n"
                                   "                    name: a\n"
                                   "                    integer_literal\n"
                                   "                        type: I32\n"
                                   "                        value: 0\n"
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
                                   "        declaration_statement\n"
                                   "            variable\n"
                                   "                type: Union[I32]\n"
                                   "                name: b\n"
                                   "            compound_literal\n"
                                   "                type: Union[I32]\n"
                                   "                field_initializer\n"
                                   "                    type: I32\n"
                                   "                    name: a\n"
                                   "                    integer_literal\n"
                                   "                        type: I32\n"
                                   "                        value: 0\n"
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
            else if (index == FIXED_ARRAY_TYPE_INDEX)
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
                                   "        return_type: [4]I32\n"
                                   "        declaration_statement\n"
                                   "            variable\n"
                                   "                type: [4]I32\n"
                                   "                name: b\n"
                                   "            array_literal\n"
                                   "                type: [4]I32\n"
                                   "                element_initializer\n"
                                   "                    index: 0\n"
                                   "                    integer_literal\n"
                                   "                        type: I32\n"
                                   "                        value: 0\n"
                                   "        return_statement\n"
                                   "            reference_expression\n"
                                   "                type: [4]I32\n"
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
                                   "        declaration_statement\n"
                                   "            variable\n"
                                   "                type: %s\n"
                                   "                name: b\n"
                                   "            compound_literal\n"
                                   "                type: %s\n"
                                   "                field_initializer\n"
                                   "                    type: I32\n"
                                   "                    name: a\n"
                                   "                    integer_literal\n"
                                   "                        type: I32\n"
                                   "                        value: 0\n"
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
                                   types[index].name,
                                   types[index].name,
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
                                   "        declaration_statement\n"
                                   "            variable\n"
                                   "                type: %s\n"
                                   "                name: b\n"
                                   "            %s_literal\n"
                                   "                type: %s\n"
                                   "                value: 0\n"
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
                                   types[index].name,
                                   types[index].name,
                                   types[index].file_friendly_default_value_name,
                                   types[index].name,
                                   types[index].name);
            }
        }

        index++;
        if (is_valid_case)
            return true;
    }

    return false;
}

static bool x_typed_variable_equal_incompatible_literal(void)
{
    static uint8_t lhs_index = 0;
    static uint8_t rhs_index = 0;

    while (true)
    {
        if (lhs_index == TYPES_LENGTH)
            return false;

        bool is_valid_case = !is_noalias_type(types[lhs_index].name) && 
                             types[rhs_index].file_friendly_default_value_name &&
                             types[rhs_index].default_value_name &&
                             (!types[lhs_index].default_value_name || strcmp(types[lhs_index].default_value_name, types[rhs_index].default_value_name)) &&
                             (!rhs_index || (types[rhs_index - 1].default_value_name && strcmp(types[rhs_index].default_value_name, types[rhs_index - 1].default_value_name)));

        if (is_valid_case)
        {
            format_test_name("semantics/statements/declaration/%s_typed_variable_equal_%s_literal", types[lhs_index].file_friendly_name, types[rhs_index].file_friendly_default_value_name);

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
                                  "function a()\n"
                                  "    %s b = %s\n"
                                  "end", 
                                  types[lhs_index].name, types[rhs_index].default_value);

            format_command_line_options("%s", test.source_paths[0]);
            format_expectation(&test.expected_error, 
                               "%s:25:%u: Cannot infer literal as %s.\n",
                               test.source_paths[0],
                               10 + strlen(types[lhs_index].name),
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

static void polymorphic_and_monomorphic_variables(void)
{
    format_test_name("semantics/statements/declaration/polymorphic_and_monomorphic_variables");

    format_source_path(0, "semantics/statements/declaration/polymorphic_and_monomorphic_variables.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                           "namespace Abc\n"
                           "\n"
                           "function a[T]() : T, I32\n"
                           "    T b, I32 c = 0, 1\n"
                           "    \n"
                           "    return b, c\n"
                           "end\n"
                           "\n"
                           "function d() : I32, I32\n"
                           "    I32 e, I32 f = a[I32]()\n"
                           "    \n"
                           "    return e, f\n"
                           "end\n"
                           "\n"
                           "function main() : I32\n"
                           "    return 0\n"
                           "end");

    SET_COMMAND_LINE("semantics/statements/declaration/polymorphic_and_monomorphic_variables.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics,
                       "file\n"
                       "    path: semantics/statements/declaration/polymorphic_and_monomorphic_variables.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        return_type_reference: T\n"
                       "        return_type_reference: I32\n"
                       "        declaration_statement\n"
                       "            variable\n"
                       "                type_reference: T\n"
                       "                name: b\n"
                       "            variable\n"
                       "                type_reference: I32\n"
                       "                name: c\n"
                       "            integer_literal\n"
                       "                value: 0\n"
                       "            integer_literal\n"
                       "                value: 1\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                name: b\n"
                       "            reference_expression\n"
                       "                name: c\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        return_type: I32\n"
                       "        return_type: I32\n"
                       "        declaration_statement\n"
                       "            variable\n"
                       "                type: I32\n"
                       "                name: b\n"
                       "            variable\n"
                       "                type: I32\n"
                       "                name: c\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 1\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: b\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: c\n"
                       "    function\n"
                       "        name: d\n"
                       "        return_type: I32\n"
                       "        return_type: I32\n"
                       "        declaration_statement\n"
                       "            variable\n"
                       "                type: I32\n"
                       "                name: e\n"
                       "            variable\n"
                       "                type: I32\n"
                       "                name: f\n"
                       "            call_expression\n"
                       "                type: I32, I32\n"
                       "                reference_expression\n"
                       "                    type: Function() : I32, I32\n"
                       "                    name: a\n"
                       "                    actual_type_parameter: I32\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: e\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: f\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static bool x_typed_variable_without_expression(void)
{
    static uint8_t index = 0;
    if (index == TYPES_LENGTH)
        return false;

    format_test_name("semantics/statements/declaration/%s_typed_variable_without_expression", types[index].file_friendly_name);

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
                           "    %s b\n"
                           "    return b\n"
                           "end\n"
                           "\n"
                           "function main() : I32\n"
                           "    return 0\n"
                           "end",
                           types[index].name,
                           types[index].name);

    format_command_line_options("%s -print-semantics", test.source_paths[0]);
    if (index == POLYMORPHIC_STRUCTURE_TYPE_INDEX)
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
                           "        declaration_statement\n"
                           "            variable\n"
                           "                type: Structure[I32]\n"
                           "                name: b\n"
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
    else if (index == POLYMORPHIC_UNION_TYPE_INDEX)
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
                           "        declaration_statement\n"
                           "            variable\n"
                           "                type: Union[I32]\n"
                           "                name: b\n"
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
    else if (is_noalias_type(types[index].name))
    {
        format_expectation(&test.expected_error, 
                           "%s:24:16: Only Pointerless formal parameter types can be noalias qualified.\n",
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
                           "        declaration_statement\n"
                           "            variable\n"
                           "                type: %s\n"
                           "                name: b\n"
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
                           types[index].name,
                           types[index].name, 
                           types[index].name);
    }

    index++;
    return true;
}

static void redeclare_variable_from_previous_statement(void)
{
    format_test_name("semantics/statements/declaration/redeclare_variable_from_previous_statement");

    format_source_path(0, "semantics/statements/declaration/redeclare_variable_from_previous_statement.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                           "namespace Abc\n"
                           "\n"
                           "function do_stuff() : I32\n"
                           "    I32 a = 0\n"
                           "    I32 a = 1\n"
                           "    \n"
                           "    return a\n"
                           "end\n");

    SET_COMMAND_LINE("semantics/statements/declaration/redeclare_variable_from_previous_statement.owen");

    format_expectation(&test.expected_error,
                       "%s:6:9: a is already defined.\n",
                       &test.source_paths[0]);
}

static void redeclare_variable_in_assignment_without_expressions(void)
{
    format_test_name("semantics/statements/declaration/redeclare_variable_in_assignment_without_expressions");

    format_source_path(0, "semantics/statements/declaration/redeclare_variable_in_assignment_without_expressions.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                           "namespace Abc\n"
                           "\n"
                           "function a()\n"
                           "    I32 b, I32 b\n"
                           "end\n");

    SET_COMMAND_LINE("semantics/statements/declaration/redeclare_variable_in_assignment_without_expressions.owen");

    format_expectation(&test.expected_error,
                       "%s:5:16: b is already defined.\n",
                       test.source_paths[0]);
}

static void redeclare_variable_in_ballanced_assignment(void)
{
    format_test_name("semantics/statements/declaration/redeclare_variable_in_ballanced_assignment");

    format_source_path(0, "semantics/statements/declaration/redeclare_variable_in_ballanced_assignment.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                           "namespace Abc\n"
                           "\n"
                           "function a()\n"
                           "    I32 b, I32 b = 1, 1\n"
                           "end\n");

    SET_COMMAND_LINE("semantics/statements/declaration/redeclare_variable_in_ballanced_assignment.owen");

    format_expectation(&test.expected_error,
                       "%s:5:16: b is already defined.\n",
                       test.source_paths[0]);
}

static void redeclare_variable_in_tuple_assignment(void)
{
    format_test_name("semantics/statements/declaration/redeclare_variable_in_tuple_assignment");

    format_source_path(0, "semantics/statements/declaration/redeclare_variable_in_tuple_assignment.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                           "namespace Abc\n"
                           "\n"
                           "function c() : I32, I32\n"
                           "    return 1, 0\n"
                           "end\n"
                           "\n"
                           "function a()\n"
                           "    I32 b, I32 b = c()\n"
                           "end\n");

    SET_COMMAND_LINE("semantics/statements/declaration/redeclare_variable_in_tuple_assignment.owen");

    format_expectation(&test.expected_error,
                       "%s:9:16: b is already defined.\n",
                       test.source_paths[0]);
}

static bool x_typed_uninitialized_literal(void)
{    
    static uint8_t index = 0;
    if (index == TYPES_LENGTH)
        return false;

    format_test_name("semantics/statements/declaration/%s_typed_uninitialized_literal", types[index].file_friendly_name);

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
                          "    %s b = ---\n"
                          "    return b\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end", 
                          types[index].name,
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
                           "        declaration_statement\n"
                           "            variable\n"
                           "                type: Union[I32]\n"
                           "                name: b\n"
                           "            uninitialized_literal\n"
                           "                type: Union[I32]\n"
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
                           "        declaration_statement\n"
                           "            variable\n"
                           "                type: Structure[I32]\n"
                           "                name: b\n"
                           "            uninitialized_literal\n"
                           "                type: Structure[I32]\n"
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
    else if (is_noalias_type(types[index].name))
    {
        format_expectation(&test.expected_error,
                           "%s:24:16: Only Pointerless formal parameter types can be noalias qualified.\n",
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
                           "        declaration_statement\n"
                           "            variable\n"
                           "                type: %s\n"
                           "                name: b\n"
                           "            uninitialized_literal\n"
                           "                type: %s\n"
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
                           types[index].name,
                           types[index].name,
                           types[index].name,
                           types[index].name);
    }

    index++;
    
    return true;
}

static void blank_identifier_on_lhs_in_ballanced_assignment(void)
{
    format_test_name("semantics/statements/declaration/blank_identifier_on_lhs_in_ballanced_assignment");

    format_source_path(0, "semantics/statements/declaration/blank_identifier_on_lhs_in_ballanced_assignment.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    I32 _ = 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/declaration/blank_identifier_on_lhs_in_ballanced_assignment.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:9: _ only allowed in tuple declarations and assignments.\n",
                       test.source_paths[0]);
}

static void blank_identifier_on_rhs_in_ballanced_assignment(void)
{
    format_test_name("semantics/statements/declaration/blank_identifier_on_rhs_in_ballanced_assignment");

    format_source_path(0, "semantics/statements/declaration/blank_identifier_on_rhs_in_ballanced_assignment.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    I32 b = _\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/declaration/blank_identifier_on_rhs_in_ballanced_assignment.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:13: _ only allowed in tuple declarations and assignments.\n",
                       test.source_paths[0]);
}

static void blank_identifier_on_lhs_in_tuple_assignment(void)
{
    format_test_name("semantics/statements/declaration/blank_identifier_on_lhs_in_tuple_assignment");

    format_source_path(0, "semantics/statements/declaration/blank_identifier_on_lhs_in_tuple_assignment.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a() : I32, I32\n"
                          "    return 1, 2\n"
                          "end"
                          "\n"
                          "function b()\n"
                          "    I32 _, I32 _ = a()\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/declaration/blank_identifier_on_lhs_in_tuple_assignment.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: %s\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        return_type: I32\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 1\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 2\n"
                       "    function\n"
                       "        name: b\n"
                       "        declaration_statement\n"
                       "            variable\n"
                       "                type: I32\n"
                       "                name: _\n"
                       "            variable\n"
                       "                type: I32\n"
                       "                name: _\n"
                       "            call_expression\n"
                       "                type: I32, I32\n"
                       "                reference_expression\n"
                       "                    type: Function() : I32, I32\n"
                       "                    name: a\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n",
                       test.source_paths[0]);
}

static void zero_initialize_blank(void)
{
    format_test_name("semantics/statements/declaration/zero_initialize_blank");

    format_source_path(0, "semantics/statements/declaration/zero_initialize_blank.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b)\n"
                          "    I32 _\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/declaration/zero_initialize_blank.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:9: _ cannot be zeroed.\n",
                       test.source_paths[0]);
}

static bool variable_equal_dereferenced_expression(void)
{
    static uint8_t type_index = 0;
    while (true)
    {
        if (type_index > LAST_TYPE_INDEX)
            return false;

        bool is_valid_case = type_index >= FIRST_POINTER_TYPE_INDEX && type_index <= LAST_POINTER_TYPE_INDEX;
        if (is_valid_case)
        {
            format_test_name("semantics/statements/declaration/variable_equal_dereferenced_%s_typed_expression", types[type_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "function a(%s b) : I32\n"
                                  "    I32 c = @b\n"
                                  "    return c\n"
                                  "end\n"
                                  "\n"
                                  "function main() : I32\n"
                                  "    return 0\n"
                                  "end",
                                  types[type_index].name);

            format_command_line_options("%s -print-semantics", test.source_paths[0]);
            format_expectation(&test.expected_semantics,
                               "file\n"
                               "    path: %s\n"
                               "    namespace: Abc\n"
                               "    function\n"
                               "        name: a\n"
                               "        formal_parameter\n"
                               "            type: %s\n"
                               "            name: b\n"
                               "        return_type: I32\n"
                               "        declaration_statement\n"
                               "            variable\n"
                               "                type: I32\n"
                               "                name: c\n"
                               "            dereference\n"
                               "                type: I32\n"
                               "                reference_expression\n"
                               "                    type: %s\n"
                               "                    name: b\n"
                               "        return_statement\n"
                               "            reference_expression\n"
                               "                type: I32\n"
                               "                name: c\n"
                               "    function\n"
                               "        name: main\n"
                               "        return_type: I32\n"
                               "        return_statement\n"
                               "            integer_literal\n"
                               "                type: I32\n"
                               "                value: 0\n",
                               test.source_paths[0],
                               types[type_index].name,
                               types[type_index].name);
        }

        type_index++;
        if (is_valid_case)
            return true;
    }
}

bool semantics_statements_declaration(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            if (x_typed_variable_equal_none_typed_expression())
                break;

            state++;

        case 1:
            if (pointer_variables_equal_compatible_tuple())
                break;
            
            state++;

        case 2:
            if (non_pointer_variables_equal_compatible_tuple())
                break;

            state++;

        case 3:
            if (non_pointer_variables_equal_incompatible_tuple())
                break;

            state++;

        case 4:
            multiple_expressions_too_many_on_lhs_of_operator();
            state++;
            break;

        case 5:
            multiple_expressions_too_many_on_rhs_of_equal();
            state++;
            break;

        case 6:
            one_expression_too_many_on_lhs_of_operator();
            state++;
            break;

        case 7:
            one_expression_too_many_on_rhs_of_operator();
            state++;
            break;

        case 8:
            if (pointer_variable_equal_incompatible_pointer())
                break;

            state++;

        case 9:
            if (x_typed_variable_equal_x_typed_expression())
                break;

            state++;

        case 10:
            if (non_pointer_x_typed_variable_equal_non_pointer_y_typed_expression())
                break;

            state++;

        case 11:
            variable_with_the_same_name_as_a_function();
            state++;
            break;

        case 12:
            variable_with_the_same_name_as_formal_parameter();
            state++;
            break;

        case 13:
            actual_type_parameter_as_variable_type();
            state++;
            break;

        case 14:
            if (x_typed_variable_equal_compatible_literal())
                break;

            state++;

        case 15:
            if (x_typed_variable_equal_incompatible_literal())
                break;

            state++;

        case 16:
            polymorphic_and_monomorphic_variables();
            state++;
            break;

        case 17:
            if (x_typed_variable_without_expression())
                break;

            state++;

        case 18:
            redeclare_variable_from_previous_statement();
            state++;
            break;

        case 19:
            redeclare_variable_in_assignment_without_expressions();
            state++;
            break;

        case 20:
            redeclare_variable_in_ballanced_assignment();
            state++;
            break;

        case 21:
            redeclare_variable_in_tuple_assignment();
            state++;
            break;

        case 22:
            if (x_typed_uninitialized_literal())
                break;

            state++;

        case 23:
            blank_identifier_on_lhs_in_ballanced_assignment();
            state++;
            break;
       
        case 24:
            blank_identifier_on_rhs_in_ballanced_assignment();
            state++;
            break;

        case 25:
            blank_identifier_on_lhs_in_tuple_assignment();
            state++;
            break;

        case 26:
            zero_initialize_blank();
            state++;
            break;

        case 27:
            if (variable_equal_dereferenced_expression())
                break;

            state++;

        default:
            return false;
    }

    return true;
}
