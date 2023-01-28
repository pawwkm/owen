#include "runner.h"

static bool assign_non_pointer_x_typed_expression_to_non_pointer_x_typed_field(void)
{
    static uint8_t index = 0;
    while (index < FIRST_POINTER_TYPE_INDEX && !types[index].default_value_name)
        index++;

    if (index == FIRST_POINTER_TYPE_INDEX)
        return false;

    format_test_name("semantics/expressions/compound_literal/assign_%s_typed_expression_to_%s_typed_field", types[index].file_friendly_name, types[index].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    %s a\n"
                          "end\n"
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
                          "function do_stuff(%s b) : A\n"
                          "    return { a = b }\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end", 
                          types[index].name,
                          types[index].name);

    format_command_line_options("%s -print-semantics", test.source_paths[0]);
    if (index == MONOMORPHIC_STRUCTURE_TYPE_INDEX)
    {
        format_expectation(&test.expected_semantics, 
                           "file\n"
                           "    path: %s\n"
                           "    namespace: Abc\n"
                           "    structure\n"
                           "        name: A\n"
                           "        is_public: false\n"
                           "        field\n"
                           "            type: %s\n"
                           "            name: a\n"
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
                           "        name: do_stuff\n"
                           "        formal_parameter\n"
                           "            type: %s\n"
                           "            name: b\n"
                           "        return_type: A\n"
                           "        return_statement\n"
                           "            compound_literal\n"
                           "                type: A\n"
                           "                field_initializer\n"
                           "                    type: %s\n"
                           "                    name: a\n"
                           "                    reference_expression\n"
                           "                        type: %s\n"
                           "                        name: b\n"
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
    else if (index == POLYMORPHIC_STRUCTURE_TYPE_INDEX)
    {
        format_expectation(&test.expected_semantics, 
                           "file\n"
                           "    path: %s\n"
                           "    namespace: Abc\n"
                           "    structure\n"
                           "        name: A\n"
                           "        is_public: false\n"
                           "        field\n"
                           "            type: %s\n"
                           "            name: a\n"
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
                           "        name: do_stuff\n"
                           "        formal_parameter\n"
                           "            type: %s\n"
                           "            name: b\n"
                           "        return_type: A\n"
                           "        return_statement\n"
                           "            compound_literal\n"
                           "                type: A\n"
                           "                field_initializer\n"
                           "                    type: %s\n"
                           "                    name: a\n"
                           "                    reference_expression\n"
                           "                        type: %s\n"
                           "                        name: b\n"
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
    else if (index == MONOMORPHIC_UNION_TYPE_INDEX)
    {
        format_expectation(&test.expected_semantics, 
                           "file\n"
                           "    path: %s\n"
                           "    namespace: Abc\n"
                           "    structure\n"
                           "        name: A\n"
                           "        is_public: false\n"
                           "        field\n"
                           "            type: %s\n"
                           "            name: a\n"
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
                           "        name: do_stuff\n"
                           "        formal_parameter\n"
                           "            type: %s\n"
                           "            name: b\n"
                           "        return_type: A\n"
                           "        return_statement\n"
                           "            compound_literal\n"
                           "                type: A\n"
                           "                field_initializer\n"
                           "                    type: %s\n"
                           "                    name: a\n"
                           "                    reference_expression\n"
                           "                        type: %s\n"
                           "                        name: b\n"
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
    else if (index == POLYMORPHIC_UNION_TYPE_INDEX)
    {
        format_expectation(&test.expected_semantics, 
                           "file\n"
                           "    path: %s\n"
                           "    namespace: Abc\n"
                           "    structure\n"
                           "        name: A\n"
                           "        is_public: false\n"
                           "        field\n"
                           "            type: %s\n"
                           "            name: a\n"
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
                           "        name: do_stuff\n"
                           "        formal_parameter\n"
                           "            type: %s\n"
                           "            name: b\n"
                           "        return_type: A\n"
                           "        return_statement\n"
                           "            compound_literal\n"
                           "                type: A\n"
                           "                field_initializer\n"
                           "                    type: %s\n"
                           "                    name: a\n"
                           "                    reference_expression\n"
                           "                        type: %s\n"
                           "                        name: b\n"
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
                           "        name: A\n"
                           "        is_public: false\n"
                           "        field\n"
                           "            type: %s\n"
                           "            name: a\n"
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
                           "        name: do_stuff\n"
                           "        formal_parameter\n"
                           "            type: %s\n"
                           "            name: b\n"
                           "        return_type: A\n"
                           "        return_statement\n"
                           "            compound_literal\n"
                           "                type: A\n"
                           "                field_initializer\n"
                           "                    type: %s\n"
                           "                    name: a\n"
                           "                    reference_expression\n"
                           "                        type: %s\n"
                           "                        name: b\n"
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

static bool assign_x_typed_expression_to_pointer_x_typed_field(void)
{
    static uint8_t expression_type_index = FIRST_POINTER_TYPE_INDEX;
    static uint8_t field_type_index = FIRST_POINTER_TYPE_INDEX;

    while (true)
    {
        if (expression_type_index > LAST_POINTER_TYPE_INDEX)
            return false;

        bool is_valid_case = !is_qualified_type(types[field_type_index].name);
        if (is_valid_case)
        {
            format_test_name("semantics/expressions/compound_literal/assign_%s_typed_expression_to_%s_typed_field", types[expression_type_index].file_friendly_name, types[field_type_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "structure A\n"
                                  "    %s a\n"
                                  "end\n"
                                  "\n"
                                  "function do_stuff(%s b) : A\n"
                                  "    return { a = b }\n"
                                  "end\n"
                                  "\n"
                                  "function main() : I32\n"
                                  "    return 0\n"
                                  "end", 
                                  types[field_type_index].name,
                                  types[expression_type_index].name);

            if (compatible_types(types[field_type_index].name, types[expression_type_index].name))
            {
                format_command_line_options("%s -print-semantics", test.source_paths[0]);
                format_expectation(&test.expected_semantics, 
                                    "file\n"
                                    "    path: %s\n"
                                    "    namespace: Abc\n"
                                    "    structure\n"
                                    "        name: A\n"
                                    "        is_public: false\n"
                                    "        field\n"
                                    "            type: %s\n"
                                    "            name: a\n"
                                    "    function\n"
                                    "        name: do_stuff\n"
                                    "        formal_parameter\n"
                                    "            type: %s\n"
                                    "            name: b\n"
                                    "        return_type: A\n"
                                    "        return_statement\n"
                                    "            compound_literal\n"
                                    "                type: A\n"
                                    "                field_initializer\n"
                                    "                    type: %s\n"
                                    "                    name: a\n"
                                    "                    reference_expression\n"
                                    "                        type: %s\n"
                                    "                        name: b\n"
                                    "    function\n"
                                    "        name: main\n"
                                    "        return_type: I32\n"
                                    "        return_statement\n"
                                    "            integer_literal\n"
                                    "                type: I32\n"
                                    "                value: 0\n",
                                    test.source_paths[0],
                                    types[expression_type_index].name,
                                    types[field_type_index].name,
                                    types[expression_type_index].name,
                                    types[field_type_index].name);
            }
            else
            {
                format_command_line_options(test.source_paths[0]);
                format_expectation(&test.expected_error, 
                                   "%s:9:18: %s expected but found %s.\n",
                                   test.source_paths[0],
                                   types[field_type_index].name,
                                   types[expression_type_index].name);
            }
        }
        
        if (field_type_index > LAST_POINTER_TYPE_INDEX)
        {
            field_type_index = FIRST_POINTER_TYPE_INDEX;
            expression_type_index++;
        }
        else
            field_type_index++;

        if (is_valid_case)
            return true;
    }
    
    return false;
}

static bool assign_non_pointer_x_typed_expression_to_non_pointer_y_typed_field(void)
{
    static uint8_t lhs_index = 0;
    static uint8_t rhs_index = 0;

    while (true)
    {
        if (lhs_index == FIRST_POINTER_TYPE_INDEX)
            return false;

        bool is_valid_case = lhs_index != rhs_index;
        if (is_valid_case)
        {
            format_test_name("semantics/expressions/compound_literal/assign_%s_typed_expression_to_%s_typed_field", types[rhs_index].file_friendly_name, types[lhs_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "structure A\n"
                                  "    %s a\n"
                                  "end\n"
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
                                  "function do_stuff(%s b) : A\n"
                                  "    return { a = b }\n"
                                  "end", 
                                  types[lhs_index].name,
                                  types[rhs_index].name);

            format_command_line_options("%s", test.source_paths[0]);
            format_expectation(&test.expected_error, 
                               "%s:29:18: %s expected but found %s.\n",
                               test.source_paths[0],
                               types[lhs_index].name,
                               types[rhs_index].name);
        }

        if (rhs_index == FIRST_POINTER_TYPE_INDEX)
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

static bool assign_non_typed_expression_to_x_typed_field(void)
{
    static uint8_t index = 0;
    if (index > FIRST_POINTER_TYPE_INDEX)
        return false;

    format_test_name("semantics/expressions/compound_literal/assign_non_typed_expression_to_%s_typed_field", types[index].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    %s a\n"
                          "end\n"
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
                          "function f()\n"
                          "end\n"
                          "\n"
                          "function do_stuff() : A\n"
                          "    return { a = f() }\n"
                          "end", 
                          types[index].name,
                          types[index].name);

    format_command_line_options("%s", test.source_paths[0]);
    format_expectation(&test.expected_error, 
                       "%s:32:19: %s expected but found none.\n",
                       test.source_paths[0],
                       types[index].name);

    index++;
    return true;
}

static bool assign_tuple_typed_expression_to_x_typed_field(void)
{
    static uint8_t index = 0;
    if (index > FIRST_POINTER_TYPE_INDEX)
        return false;

    format_test_name("semantics/expressions/compound_literal/assign_tuple_typed_expression_to_%s_typed_field", types[index].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    %s a\n"
                          "end\n"
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
                          "function f() : I32, I32\n"
                          "    return 0, 0\n"
                          "end\n"
                          "\n"
                          "function do_stuff() : A\n"
                          "    return { a = f() }\n"
                          "end", 
                          types[index].name,
                          types[index].name);

    format_command_line_options("%s", test.source_paths[0]);
    format_expectation(&test.expected_error, 
                       "%s:33:19: %s expected but found I32, I32.\n",
                       test.source_paths[0],
                       types[index].name);

    index++;
    return true;
}

static void field_initialized_multiple_times(void)
{
    format_test_name("semantics/expressions/compound_literal/field_initialized_multiple_times");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    I32 a\n"
                          "end\n"
                          "\n"
                          "function do_stuff() : A\n"
                          "    return { a = 0, a = 1 }\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/compound_literal/field_initialized_multiple_times.owen");
    format_expectation(&test.expected_error, 
                       "semantics/expressions/compound_literal/field_initialized_multiple_times.owen:9:21: a is initialized multiple times.\n");
}

static bool x_type_compatible_literal_assigned_to_x_typed_field(void)
{
    static uint8_t index = 0;

    while (true)
    {
        if (index == LAST_TYPE_INDEX)
            return false;

        bool is_valid_case = types[index].file_friendly_default_value_name != NULL;
        if (is_valid_case)
        {
            format_test_name("semantics/expressions/compound_literal/%s_type_%s_literal_assigned_to_%s_typed_field", types[index].file_friendly_name, types[index].file_friendly_default_value_name, types[index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "structure A\n"
                                  "    %s a\n"
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
                                  "function do_stuff() : A\n"
                                  "    return { a = %s }\n"
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
                                   "        name: A\n"
                                   "        is_public: false\n"
                                   "        field\n"
                                   "            type: %s\n"
                                   "            name: a\n"
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
                                   "        name: do_stuff\n"
                                   "        return_type: A\n"
                                   "        return_statement\n"
                                   "            compound_literal\n"
                                   "                type: A\n"
                                   "                field_initializer\n"
                                   "                    type: %s\n"
                                   "                    name: a\n"
                                   "                    compound_literal\n"
                                   "                        type: %s\n"
                                   "                        field_initializer\n"
                                   "                            type: I32\n"
                                   "                            name: a\n"
                                   "                            integer_literal\n"
                                   "                                type: I32\n"
                                   "                                value: 0\n"
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
            else if (index == POLYMORPHIC_UNION_TYPE_INDEX)
            {
                format_expectation(&test.expected_semantics, 
                                   "file\n"
                                   "    path: %s\n"
                                   "    namespace: Abc\n"
                                   "    structure\n"
                                   "        name: A\n"
                                   "        is_public: false\n"
                                   "        field\n"
                                   "            type: %s\n"
                                   "            name: a\n"
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
                                   "        name: do_stuff\n"
                                   "        return_type: A\n"
                                   "        return_statement\n"
                                   "            compound_literal\n"
                                   "                type: A\n"
                                   "                field_initializer\n"
                                   "                    type: %s\n"
                                   "                    name: a\n"
                                   "                    compound_literal\n"
                                   "                        type: %s\n"
                                   "                        field_initializer\n"
                                   "                            type: I32\n"
                                   "                            name: a\n"
                                   "                            integer_literal\n"
                                   "                                type: I32\n"
                                   "                                value: 0\n"
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
            else if (index == FIXED_ARRAY_TYPE_INDEX)
            {
                format_expectation(&test.expected_semantics, 
                                   "file\n"
                                   "    path: %s\n"
                                   "    namespace: Abc\n"
                                   "    structure\n"
                                   "        name: A\n"
                                   "        is_public: false\n"
                                   "        field\n"
                                   "            type: [4]I32\n"
                                   "            name: a\n"
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
                                   "        name: do_stuff\n"
                                   "        return_type: A\n"
                                   "        return_statement\n"
                                   "            compound_literal\n"
                                   "                type: A\n"
                                   "                field_initializer\n"
                                   "                    type: [4]I32\n"
                                   "                    name: a\n"
                                   "                    array_literal\n"
                                   "                        type: [4]I32\n"
                                   "                        element_initializer\n"
                                   "                            index: 0\n"
                                   "                            integer_literal\n"
                                   "                                type: I32\n"
                                   "                                value: 0\n"
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
                                   "        name: A\n"
                                   "        is_public: false\n"
                                   "        field\n"
                                   "            type: %s\n"
                                   "            name: a\n"
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
                                   "        name: do_stuff\n"
                                   "        return_type: A\n"
                                   "        return_statement\n"
                                   "            compound_literal\n"
                                   "                type: A\n"
                                   "                field_initializer\n"
                                   "                    type: %s\n"
                                   "                    name: a\n"
                                   "                    compound_literal\n"
                                   "                        type: %s\n"
                                   "                        field_initializer\n"
                                   "                            type: I32\n"
                                   "                            name: a\n"
                                   "                            integer_literal\n"
                                   "                                type: I32\n"
                                   "                                value: 0\n"
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
            else
            {
                format_expectation(&test.expected_semantics, 
                                   "file\n"
                                   "    path: %s\n"
                                   "    namespace: Abc\n"
                                   "    structure\n"
                                   "        name: A\n"
                                   "        is_public: false\n"
                                   "        field\n"
                                   "            type: %s\n"
                                   "            name: a\n"
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
                                   "        name: do_stuff\n"
                                   "        return_type: A\n"
                                   "        return_statement\n"
                                   "            compound_literal\n"
                                   "                type: A\n"
                                   "                field_initializer\n"
                                   "                    type: %s\n"
                                   "                    name: a\n"
                                   "                    %s_literal\n"
                                   "                        type: %s\n"
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

static bool x_type_incompatible_literal_assigned_to_x_typed_field(void)
{
    static uint8_t lhs_index = 0;
    static uint8_t rhs_index = 0;

    while (true)
    {
        if (lhs_index == LAST_TYPE_INDEX)
            return false;

        bool is_valid_case = types[lhs_index].file_friendly_default_value_name &&
                             types[lhs_index].default_value &&
                             types[rhs_index].file_friendly_default_value_name &&
                             types[rhs_index].default_value &&
                             strcmp(types[lhs_index].default_value_name, types[rhs_index].default_value_name);

        if (is_valid_case)
        {
            format_test_name("semantics/expressions/compound_literal/%s_type_%s_literal_assigned_to_%s_typed_field", types[rhs_index].file_friendly_name, types[rhs_index].file_friendly_default_value_name, types[lhs_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "structure A\n"
                                  "    %s a\n"
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
                                  "function do_stuff() : A\n"
                                  "    return { a = %s }\n"
                                  "end", 
                                  types[lhs_index].name,
                                  types[rhs_index].default_value);

            format_command_line_options("%s", test.source_paths[0]);
            format_expectation(&test.expected_error, 
                                "%s:25:18: Cannot infer literal as %s.\n",
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

static void monomorphic_literal_in_polymorphic_function(void)
{
    format_test_name("semantics/expressions/compound_literal/monomorphic_literal_in_polymorphic_function");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    I32 a\n"
                          "end\n"
                          "\n"
                          "function a[T](I32 b) : A\n"
                          "    return { a = b }\n"
                          "end\n"
                          "\n"
                          "function c(I32 d) : A\n"
                          "    return a[I32](d)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/compound_literal/monomorphic_literal_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/compound_literal/monomorphic_literal_in_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: a\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: I32\n"
                       "            name: b\n"
                       "        return_type_reference: A\n"
                       "        return_statement\n"
                       "            compound_literal\n"
                       "                field_initializer\n"
                       "                    name: a\n"
                       "                    reference_expression\n"
                       "                        name: b\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: b\n"
                       "        return_type: A\n"
                       "        return_statement\n"
                       "            compound_literal\n"
                       "                type: A\n"
                       "                field_initializer\n"
                       "                    type: I32\n"
                       "                    name: a\n"
                       "                    reference_expression\n"
                       "                        type: I32\n"
                       "                        name: b\n"
                       "    function\n"
                       "        name: c\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: d\n"
                       "        return_type: A\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: A\n"
                       "                reference_expression\n"
                       "                    type: Function(I32) : A\n"
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

bool semantics_expressions_compound_literal(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            if (assign_non_pointer_x_typed_expression_to_non_pointer_x_typed_field())
                break;
            
            state++;
            
        case 1:
            if (assign_x_typed_expression_to_pointer_x_typed_field())
                break;
            
            state++;

        case 2:
            if (assign_non_pointer_x_typed_expression_to_non_pointer_y_typed_field())
                break;

            state++;

        case 3:
            if (assign_non_typed_expression_to_x_typed_field())
                break;

            state++;

        case 4:
            if (assign_tuple_typed_expression_to_x_typed_field())
                break;

            state++;

        case 5:
            field_initialized_multiple_times();
            state++;
            break;

        case 6:
            if (x_type_compatible_literal_assigned_to_x_typed_field())
                break;

            state++;

        case 7:
            if (x_type_incompatible_literal_assigned_to_x_typed_field())
                break;

            state++;

        case 8:
            monomorphic_literal_in_polymorphic_function();
            state++;
            break;

        default:
            return false;
    }

    return true;
}
