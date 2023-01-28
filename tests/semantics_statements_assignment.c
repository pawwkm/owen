#include "runner.h"

static bool x_typed_parameter_operator_none_typed_expression(void)
{    
    static uint8_t index = 0;
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
    if (index == TYPES_LENGTH)
        return false;

    format_test_name("semantics/statements/assignment/%s_typed_parameter_%s_operator_none_typed_expression", types[index].file_friendly_name, assignment_operators[operator_tag].file_friendly_name);

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
                          "function b(%s c)\n"
                          "    c %s a()\n"
                          "end", 
                          types[index].name, 
                          assignment_operators[operator_tag].token);

    format_command_line_options(test.source_paths[0]);
    if (is_readonly_type(types[index].name))
    {
        format_expectation(&test.expected_error,
                           "%s:28:5: Assigning to readonly expression.\n",
                           test.source_paths[0],
                           types[index].name);
    }
    else
    {
        format_expectation(&test.expected_error, 
                           "%s:28:7: Operator not defined for %s and none.\n",
                           test.source_paths[0],
                           types[index].name);
    }
    
    if (operator_tag == LAST_ASSIGNMENT_OPERATOR_TAG)
    {
        operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
        index++;
    }
    else
        operator_tag++;
    
    return true;
}

static bool assign_x_pointer_tuple_compatible_variables(void)
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
            format_test_name("semantics/statements/assignment/assign_%s_tuple_to_%s_variables", types[tuple_type_index].file_friendly_name, types[variable_type_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "function a(%s b) : %s, %s\n"
                                  "    return b, b\n"
                                  "end\n"
                                  "\n"
                                  "function c(%s d) : %s, %s\n"
                                  "    %s e, %s f = d, d\n"
                                  "    e, f = a(d)\n"
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
            if (is_readonly_type(types[variable_type_index].name))
            {
                format_expectation(&test.expected_error,
                                   "%s:10:5: Assigning to readonly expression.\n",
                                   test.source_paths[0]);
            }
            else
            {
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
                                   "            reference_expression\n"
                                   "                type: %s\n"
                                   "                name: d\n"
                                   "            reference_expression\n"
                                   "                type: %s\n"
                                   "                name: d\n"
                                   "        assignment_statement\n"
                                   "            operator: =\n"
                                   "            reference_expression\n"
                                   "                type: %s\n"
                                   "                name: e\n"
                                   "            reference_expression\n"
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
                                   types[tuple_type_index].name,
                                   types[tuple_type_index].name, 
                                   types[variable_type_index].name, 
                                   types[variable_type_index].name, 
                                   types[tuple_type_index].name, types[tuple_type_index].name, 
                                   types[tuple_type_index].name, types[tuple_type_index].name, types[tuple_type_index].name,
                                   types[tuple_type_index].name,
                                   types[variable_type_index].name,
                                   types[variable_type_index].name);
            }
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

static bool assign_x_non_pointer_tuple_compatible_variables(void)
{
    static uint8_t type_index = 0;
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;

    while (true)
    {
        if (type_index == FIRST_POINTER_TYPE_INDEX)
            return false;

        bool is_valid_case = is_assignment_operator_defined_for_type(type_index, operator_tag);
        if (is_valid_case)
        {
            format_test_name("semantics/statements/assignment/%s_tuple_%s_%s_variables", types[type_index].file_friendly_name, assignment_operators[operator_tag].file_friendly_name, types[type_index].file_friendly_name);

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
                                  "    %s e, %s f = d, d\n"
                                  "    e, f %s a(d)\n"
                                  "    return e, f\n"
                                  "end\n"
                                  "\n"
                                  "function main() : I32\n"
                                  "    return 0\n"
                                  "end",
                                  types[type_index].name, types[type_index].name, types[type_index].name,
                                  types[type_index].name, types[type_index].name, types[type_index].name,
                                  types[type_index].name, types[type_index].name,
                                  assignment_operators[operator_tag].token);

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
                                   "            reference_expression\n"
                                   "                type: Structure[I32]\n"
                                   "                name: d\n"
                                   "            reference_expression\n"
                                   "                type: Structure[I32]\n"
                                   "                name: d\n"
                                   "        assignment_statement\n"
                                   "            operator: %s\n"
                                   "            reference_expression\n"
                                   "                type: Structure[I32]\n"
                                   "                name: e\n"
                                   "            reference_expression\n"
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
                                   test.source_paths[0],
                                   assignment_operators[operator_tag].token);
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
                                   "            reference_expression\n"
                                   "                type: Union[I32]\n"
                                   "                name: d\n"
                                   "            reference_expression\n"
                                   "                type: Union[I32]\n"
                                   "                name: d\n"
                                   "        assignment_statement\n"
                                   "            operator: %s\n"
                                   "            reference_expression\n"
                                   "                type: Union[I32]\n"
                                   "                name: e\n"
                                   "            reference_expression\n"
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
                                   test.source_paths[0],
                                   assignment_operators[operator_tag].token);
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
                                   "            reference_expression\n"
                                   "                type: %s\n"
                                   "                name: d\n"
                                   "            reference_expression\n"
                                   "                type: %s\n"
                                   "                name: d\n"
                                   "        assignment_statement\n"
                                   "            operator: %s\n"
                                   "            reference_expression\n"
                                   "                type: %s\n"
                                   "                name: e\n"
                                   "            reference_expression\n"
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
                                   types[type_index].name, 
                                   types[type_index].name, 
                                   assignment_operators[operator_tag].token,
                                   types[type_index].name, 
                                   types[type_index].name, 
                                   types[type_index].name, types[type_index].name,
                                   types[type_index].name, types[type_index].name, types[type_index].name, 
                                   types[type_index].name,
                                   types[type_index].name,
                                   types[type_index].name);
            }
        }
        
        if (operator_tag == LAST_ASSIGNMENT_OPERATOR_TAG)
        {
            operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
            type_index++;
        }
        else
            operator_tag++;
        
        if (is_valid_case)
            return true;
    }
}

static bool assign_x_non_pointer_tuple_incompatible_variables(void)
{
    static uint8_t lhs_type_index = 0;
    static uint8_t rhs_type_index = 0;
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;

    while (true)
    {
        if (lhs_type_index == FIRST_POINTER_TYPE_INDEX)
            return false;

        bool is_valid_case = is_assignment_operator_defined_for_type(lhs_type_index, operator_tag) &&
                             is_assignment_operator_defined_for_type(rhs_type_index, operator_tag) &&
                             lhs_type_index != rhs_type_index;

        if (is_valid_case)
        {
            format_test_name("semantics/statements/assignment/%s_tuple_%s_%s_variables", types[lhs_type_index].file_friendly_name, assignment_operators[operator_tag].file_friendly_name, types[rhs_type_index].file_friendly_name);

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
                                  "function c(%s d, %s e, %s f)\n"
                                  "    d, e %s a(f)\n"
                                  "end",
                                  types[rhs_type_index].name, types[rhs_type_index].name, types[rhs_type_index].name,
                                  types[lhs_type_index].name, types[lhs_type_index].name, types[rhs_type_index].name,
                                  assignment_operators[operator_tag].token);

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error,
                               "%s:29:%u: %s, %s expected but found %s, %s.\n",
                               test.source_paths[0], 
                               12 + strlen(assignment_operators[operator_tag].token),
                               types[lhs_type_index].name, 
                               types[lhs_type_index].name, 
                               types[rhs_type_index].name, 
                               types[rhs_type_index].name);
        }
        
        if (rhs_type_index + 1 == FIRST_POINTER_TYPE_INDEX)
        {
            rhs_type_index = 0;
            if (operator_tag == LAST_ASSIGNMENT_OPERATOR_TAG)
            {
                operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
                lhs_type_index++;
            }
            else
                operator_tag++;
        }
        else
            rhs_type_index++;
        
        if (is_valid_case)
            return true;
    }
}

static bool multiple_expressions_too_many_on_lhs_of_operator(void)
{
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
    if (operator_tag > LAST_ASSIGNMENT_OPERATOR_TAG)
        return false;

    format_test_name("semantics/statements/assignment/multiple_expressions_too_many_on_lhs_of_%s", assignment_operators[operator_tag].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b)\n"
                          "    b, b, b, b %s b, b\n"
                          "end",
                          assignment_operators[operator_tag].token);

    format_command_line_options(test.source_paths[0]);
    format_expectation(&test.expected_error,
                       "%s:5:%u: 4 expressions expected but found 2.\n",
                       test.source_paths[0],
                       20 + strlen(assignment_operators[operator_tag].token));

    operator_tag++;
    return true;
}

static bool multiple_expressions_too_many_on_rhs_of_operator(void)
{
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
    if (operator_tag > LAST_ASSIGNMENT_OPERATOR_TAG)
        return false;

    format_test_name("semantics/statements/assignment/multiple_expressions_too_many_on_rhs_of_%s", assignment_operators[operator_tag].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b)\n"
                          "    b %s b, b, b\n"
                          "end",
                          assignment_operators[operator_tag].token);

    format_command_line_options(test.source_paths[0]);
    format_expectation(&test.expected_error,
                       "%s:5:5: 1 expression expected but found 3.\n",
                       test.source_paths[0]);

    operator_tag++;
    return true;
}

static bool bool_literal_operator_expression(void)
{
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
    if (operator_tag > LAST_ASSIGNMENT_OPERATOR_TAG)
        return false;

    format_test_name("semantics/statements/assignment/bool_literal_%s_expression", assignment_operators[operator_tag].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    false %s true\n"
                          "end",
                          assignment_operators[operator_tag].token);

    format_command_line_options(test.source_paths[0]);
    format_expectation(&test.expected_error,
                       "%s:5:5: Expression is not addressable.\n",
                       test.source_paths[0]);

    operator_tag++;
    return true;
}

static bool parenthesized_expression_operator_expression(void)
{
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
    if (operator_tag > LAST_ASSIGNMENT_OPERATOR_TAG)
        return false;

    format_test_name("semantics/statements/assignment/parenthesized_expression_%s_expression", assignment_operators[operator_tag].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b)\n"
                          "    (b) %s 1\n"
                          "end",
                          assignment_operators[operator_tag].token);

    format_command_line_options(test.source_paths[0]);
    format_expectation(&test.expected_error,
                       "%s:5:5: Expression is not addressable.\n",
                       test.source_paths[0]);

    operator_tag++;
    return true;
}

static bool not_expression_operator_expression(void)
{
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
    if (operator_tag > LAST_ASSIGNMENT_OPERATOR_TAG)
        return false;

    format_test_name("semantics/statements/assignment/not_expression_%s_expression", assignment_operators[operator_tag].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    !false %s !true\n"
                          "end",
                          assignment_operators[operator_tag].token);

    format_command_line_options(test.source_paths[0]);
    format_expectation(&test.expected_error,
                       "%s:5:5: Expression is not addressable.\n",
                       test.source_paths[0]);

    operator_tag++;
    return true;
}

static bool negate_expression_operator_expression(void)
{
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
    if (operator_tag > LAST_ASSIGNMENT_OPERATOR_TAG)
        return false;

    format_test_name("semantics/statements/assignment/negate_expression_%s_expression", assignment_operators[operator_tag].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b)\n"
                          "    -b %s b\n"
                          "end",
                          assignment_operators[operator_tag].token);

    format_command_line_options(test.source_paths[0]);
    format_expectation(&test.expected_error,
                       "%s:5:5: Expression is not addressable.\n",
                       test.source_paths[0]);

    operator_tag++;
    return true;
}

static bool address_of_expression_operator_expression(void)
{
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
    if (operator_tag > LAST_ASSIGNMENT_OPERATOR_TAG)
        return false;

    format_test_name("semantics/statements/assignment/address_of_expression_%s_expression", assignment_operators[operator_tag].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b)\n"
                          "    #b %s 1\n"
                          "end",
                          assignment_operators[operator_tag].token);

    format_command_line_options(test.source_paths[0]);
    format_expectation(&test.expected_error,
                       "%s:5:5: Expression is not addressable.\n",
                       test.source_paths[0]);

    operator_tag++;
    return true;
}

static bool call_expression_operator_expression(void)
{
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
    if (operator_tag > LAST_ASSIGNMENT_OPERATOR_TAG)
        return false;

    format_test_name("semantics/statements/assignment/call_expression_%s_expression", assignment_operators[operator_tag].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    a() %s a()\n"
                          "end",
                          assignment_operators[operator_tag].token);

    format_command_line_options(test.source_paths[0]);
    format_expectation(&test.expected_error,
                       "%s:5:6: Expression is not addressable.\n",
                       test.source_paths[0]);

    operator_tag++;
    return true;
}

static bool binary_expression_operator_expression(void)
{
    static Assignment_Operator_Tag assignment_operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
    if (assignment_operator_tag > LAST_ASSIGNMENT_OPERATOR_TAG)
        return false;

    format_test_name("semantics/statements/assignment/expression_plus_expression_%s_expression", assignment_operators[assignment_operator_tag].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(U32 b)\n"
                          "    b + b %s 1\n"
                          "end",
                          assignment_operators[assignment_operator_tag].token);

    format_command_line_options(test.source_paths[0]);
    format_expectation(&test.expected_error,
                       "%s:5:5: Expression is not addressable.\n",
                       test.source_paths[0]);

    assignment_operator_tag++;

    return true;
}

static bool dynamic_array_length_operator_expression(void)
{
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
    if (operator_tag > LAST_ASSIGNMENT_OPERATOR_TAG)
        return false;

    format_test_name("semantics/statements/assignment/dynamic_array_length_%s_expression", assignment_operators[operator_tag].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a([]I32 b, U32 c)\n"
                          "    b.length %s c\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end",
                          assignment_operators[operator_tag].token);

    format_command_line_options("%s -print-semantics", test.source_paths[0]);
    format_expectation(&test.expected_semantics,
                       "file\n"
                       "    path: %s\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_parameter\n"
                       "            type: []I32\n"
                       "            name: b\n"
                       "        formal_parameter\n"
                       "            type: U32\n"
                       "            name: c\n"
                       "        assignment_statement\n"
                       "            operator: %s\n"
                       "            field_access\n"
                       "                type: U32\n"
                       "                reference_expression\n"
                       "                    type: []I32\n"
                       "                    name: b\n"
                       "                field: length\n"
                       "            reference_expression\n"
                       "                type: U32\n"
                       "                name: c\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n",
                       test.source_paths[0],
                       assignment_operators[operator_tag].token);

    operator_tag++;
    return true;
}

static bool dynamic_array_capacity_operator_expression(void)
{
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
    if (operator_tag > LAST_ASSIGNMENT_OPERATOR_TAG)
        return false;

    format_test_name("semantics/statements/assignment/dynamic_array_capacity_%s_expression", assignment_operators[operator_tag].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a([]I32 b, U32 c)\n"
                          "    b.capacity %s c\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end",
                          assignment_operators[operator_tag].token);

    format_command_line_options("%s -print-semantics", test.source_paths[0]);
    format_expectation(&test.expected_semantics,
                       "file\n"
                       "    path: %s\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_parameter\n"
                       "            type: []I32\n"
                       "            name: b\n"
                       "        formal_parameter\n"
                       "            type: U32\n"
                       "            name: c\n"
                       "        assignment_statement\n"
                       "            operator: %s\n"
                       "            field_access\n"
                       "                type: U32\n"
                       "                reference_expression\n"
                       "                    type: []I32\n"
                       "                    name: b\n"
                       "                field: capacity\n"
                       "            reference_expression\n"
                       "                type: U32\n"
                       "                name: c\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n",
                       test.source_paths[0],
                       assignment_operators[operator_tag].token);
    
    operator_tag++;
    return true;
}

static void dynamic_array_elements_equal_compatible_pointer(void)
{
    format_test_name("semantics/statements/assignment/dynamic_array_elements_equal_compatible_pointer");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a([]I32 b, U32 c)\n"
                          "    b.length = c\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    format_command_line_options("%s -print-semantics", test.source_paths[0]);
    format_expectation(&test.expected_semantics,
                       "file\n"
                       "    path: %s\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_parameter\n"
                       "            type: []I32\n"
                       "            name: b\n"
                       "        formal_parameter\n"
                       "            type: U32\n"
                       "            name: c\n"
                       "        assignment_statement\n"
                       "            operator: =\n"
                       "            field_access\n"
                       "                type: U32\n"
                       "                reference_expression\n"
                       "                    type: []I32\n"
                       "                    name: b\n"
                       "                field: length\n"
                       "            reference_expression\n"
                       "                type: U32\n"
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

static bool fixed_array_length_operator_expression(void)
{
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
    if (operator_tag > LAST_ASSIGNMENT_OPERATOR_TAG)
        return false;

    format_test_name("semantics/statements/assignment/fixed_array_length_%s_expression", assignment_operators[operator_tag].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a([2]I32 b, U32 c)\n"
                          "    b.length %s c\n"
                          "end",
                          assignment_operators[operator_tag].token);

    format_command_line_options(test.source_paths[0]);
    format_expectation(&test.expected_error,
                       "%s:5:5: Expression is not addressable.\n",
                       test.source_paths[0]);

    operator_tag++;
    return true;
}

static bool dereferenced_expression_operator_expression(void)
{
    static uint8_t type_index = 0;
    while (true)
    {
        if (type_index > LAST_TYPE_INDEX)
            return false;

        bool is_valid_case = type_index >= FIRST_POINTER_TYPE_INDEX && type_index <= LAST_POINTER_TYPE_INDEX;
        if (is_valid_case)
        {
            format_test_name("semantics/statements/assignment/dereferenced_%s_typed_expression_equal_expression", types[type_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "function a(%s b)\n"
                                  "    @b = 1\n"
                                  "end\n"
                                  "\n"
                                  "function main() : I32\n"
                                  "    return 0\n"
                                  "end",
                                  types[type_index].name);

            format_command_line_options("%s -print-semantics", test.source_paths[0]);
            if (!is_readonly_type(types[type_index].name))
            {
                format_expectation(&test.expected_semantics,
                                   "file\n"
                                   "    path: %s\n"
                                   "    namespace: Abc\n"
                                   "    function\n"
                                   "        name: a\n"
                                   "        formal_parameter\n"
                                   "            type: %s\n"
                                   "            name: b\n"
                                   "        assignment_statement\n"
                                   "            operator: =\n"
                                   "            dereference\n"
                                   "                type: I32\n"
                                   "                reference_expression\n"
                                   "                    type: %s\n"
                                   "                    name: b\n"
                                   "            integer_literal\n"
                                   "                type: I32\n"
                                   "                value: 1\n"
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
            else
            {
                format_expectation(&test.expected_error, 
                                   "%s:5:5: Assigning to readonly expression.\n",
                                   test.source_paths[0],
                                   types[type_index].name);
            }
        }

        type_index++;
        if (is_valid_case)
            return true;
    }
}

static bool expression_operator_dereferenced_expression(void)
{
    static uint8_t type_index = 0;
    while (true)
    {
        if (type_index > LAST_TYPE_INDEX)
            return false;

        bool is_valid_case = type_index >= FIRST_POINTER_TYPE_INDEX && type_index <= LAST_POINTER_TYPE_INDEX;
        if (is_valid_case)
        {
            format_test_name("semantics/statements/assignment/expression_equal_dereferenced_%s_typed_expression", types[type_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "function a(%s b, I32 c)\n"
                                  "    c = @b\n"
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
                               "        formal_parameter\n"
                               "            type: I32\n"
                               "            name: c\n"
                               "        assignment_statement\n"
                               "            operator: =\n"
                               "            reference_expression\n"
                               "                type: I32\n"
                               "                name: c\n"
                               "            dereference\n"
                               "                type: I32\n"
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
                               types[type_index].name,
                               types[type_index].name);
        }

        type_index++;
        if (is_valid_case)
            return true;
    }
}

static void pointer_typed_field_access(void)
{
    format_test_name("semantics/statements/assignment/expression_equal_field_access_to_pointer_readable");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    I32 a\n"
                          "end\n"
                          "\n"
                          "function a(#A b, I32 c)\n"
                          "    c = b.a\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    format_command_line_options("%s -print-semantics", test.source_paths[0]);
    format_expectation(&test.expected_semantics,
                       "file\n"
                       "    path: %s\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: a\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_parameter\n"
                       "            type: #A\n"
                       "            name: b\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: c\n"
                       "        assignment_statement\n"
                       "            operator: =\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: c\n"
                       "            field_access\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: #A\n"
                       "                    name: b\n"
                       "                field: a\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n",
                       test.source_paths[0]);
}

static void pointer_typed_array_access(void)
{
    format_test_name("semantics/statements/assignment/expression_equal_array_access_to_pointer_readable");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(#[]I32 b, I32 c)\n"
                          "    c = b[0]\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    format_command_line_options("%s -print-semantics", test.source_paths[0]);
    format_expectation(&test.expected_semantics,
                       "file\n"
                       "    path: %s\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_parameter\n"
                       "            type: #[]I32\n"
                       "            name: b\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: c\n"
                       "        assignment_statement\n"
                       "            operator: =\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: c\n"
                       "            array_access\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: #[]I32\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    type: U32\n"
                       "                    value: 0\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n",
                       test.source_paths[0]);
}

static bool one_expression_too_many_on_lhs_of_operator(void)
{
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
    if (operator_tag > LAST_ASSIGNMENT_OPERATOR_TAG)
        return false;

    format_test_name("semantics/statements/assignment/one_expression_too_many_on_lhs_of_%s", assignment_operators[operator_tag].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                           "namespace Abc\n"
                           "\n"
                           "function a(I32 b)\n"
                           "    b, b, b %s b, b\n"
                           "end",
                           assignment_operators[operator_tag].token);

    format_command_line_options(test.source_paths[0]);
    format_expectation(&test.expected_error,
                       "%s:5:%u: 3 expressions expected but found 2.\n",
                       test.source_paths[0],
                       17 + strlen(assignment_operators[operator_tag].token));

    operator_tag++;
    return true;
}

static bool one_expression_too_many_on_rhs_of_operator(void)
{
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
    if (operator_tag > LAST_ASSIGNMENT_OPERATOR_TAG)
        return false;

    format_test_name("semantics/statements/assignment/one_expression_too_many_on_rhs_of_%s", assignment_operators[operator_tag].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                           "namespace Abc\n"
                           "\n"
                           "function a(I32 b)\n"
                           "    b %s b, b\n"
                           "end",
                           assignment_operators[operator_tag].token);

    format_command_line_options(test.source_paths[0]);
    format_expectation(&test.expected_error,
                       "%s:5:5: 1 expression expected but found 2.\n",
                       test.source_paths[0]);

    operator_tag++;
    return true;
}

static void polymorphic_and_monomorphic_variables(void)
{
    format_test_name("semantics/statements/assignment/polymorphic_and_monomorphic_variables");

    format_source_path(0, "semantics/statements/assignment/polymorphic_and_monomorphic_variables.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T](T b, I32 c)\n"
                          "    b, c = c, b\n"
                          "end\n"
                          "\n"
                          "function d(I32 e)\n"
                          "    a(e, e)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/assignment/polymorphic_and_monomorphic_variables.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics,
                       "file\n"
                       "    path: semantics/statements/assignment/polymorphic_and_monomorphic_variables.owen\n"
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
                       "        assignment_statement\n"
                       "            operator: =\n"
                       "            reference_expression\n"
                       "                name: b\n"
                       "            reference_expression\n"
                       "                name: c\n"
                       "            reference_expression\n"
                       "                name: c\n"
                       "            reference_expression\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: b\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: c\n"
                       "        assignment_statement\n"
                       "            operator: =\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: b\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: c\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: c\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: d\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: e\n"
                       "        call_expression\n"
                       "            type: none\n"
                       "            reference_expression\n"
                       "                type: Function(I32, I32)\n"
                       "                name: a\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: e\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: e\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static bool x_typed_variable_x_type_compatible_operator_infer_compatible_literal(void)
{
    static uint8_t type_index = 0;
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;

    while (true)
    {
        if (type_index == TYPES_LENGTH)
            return false;

        bool is_valid_case = is_assignment_operator_defined_for_type(type_index, operator_tag) &&
                             types[type_index].default_value_name;

        if (is_valid_case)
        {
            format_test_name("semantics/statements/assignment/%s_typed_variable_%s_%s_literal", types[type_index].file_friendly_name, assignment_operators[operator_tag].file_friendly_name, types[type_index].file_friendly_default_value_name);

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
                                  "function a(%s b)\n"
                                  "    b %s %s\n"
                                  "end\n"
                                  "\n"
                                  "function main() : I32\n"
                                  "    return 0\n"
                                  "end", 
                                  types[type_index].name,
                                  assignment_operators[operator_tag].token, types[type_index].default_value);

            format_command_line_options("%s -print-semantics", test.source_paths[0]);
            if (type_index == MONOMORPHIC_UNION_TYPE_INDEX)
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
                                   "            type: Union\n"
                                   "            name: b\n"
                                   "        assignment_statement\n"
                                   "            operator: %s\n"
                                   "            reference_expression\n"
                                   "                type: Union\n"
                                   "                name: b\n"
                                   "            compound_literal\n"
                                   "                type: Union\n"
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
                                   assignment_operators[operator_tag].token);
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
                                   "        assignment_statement\n"
                                   "            operator: %s\n"
                                   "            reference_expression\n"
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
                                   "    function\n"
                                   "        name: main\n"
                                   "        return_type: I32\n"
                                   "        return_statement\n"
                                   "            integer_literal\n"
                                   "                type: I32\n"
                                   "                value: 0\n",
                                   test.source_paths[0],
                                   assignment_operators[operator_tag].token);
            }
            else if (type_index == MONOMORPHIC_STRUCTURE_TYPE_INDEX)
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
                                   "            type: Structure\n"
                                   "            name: b\n"
                                   "        assignment_statement\n"
                                   "            operator: %s\n"
                                   "            reference_expression\n"
                                   "                type: Structure\n"
                                   "                name: b\n"
                                   "            compound_literal\n"
                                   "                type: Structure\n"
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
                                   assignment_operators[operator_tag].token);
            }
            else if (type_index == FIXED_ARRAY_TYPE_INDEX)
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
                                   "            type: [4]I32\n"
                                   "            name: b\n"
                                   "        assignment_statement\n"
                                   "            operator: %s\n"
                                   "            reference_expression\n"
                                   "                type: [4]I32\n"
                                   "                name: b\n"
                                   "            array_literal\n"
                                   "                type: [4]I32\n"
                                   "                element_initializer\n"
                                   "                    index: 0\n"
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
                                   assignment_operators[operator_tag].token);
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
                                   "            name: b\n"
                                   "        assignment_statement\n"
                                   "            operator: %s\n"
                                   "            reference_expression\n"
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
                                   "    function\n"
                                   "        name: main\n"
                                   "        return_type: I32\n"
                                   "        return_statement\n"
                                   "            integer_literal\n"
                                   "                type: I32\n"
                                   "                value: 0\n",
                                   test.source_paths[0],
                                   assignment_operators[operator_tag].token);
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
                                   "        assignment_statement\n"
                                   "            operator: %s\n"
                                   "            reference_expression\n"
                                   "                type: %s\n"
                                   "                name: b\n"
                                   "            %s_literal\n"
                                   "                type: %s\n"
                                   "                value: %s\n"
                                   "    function\n"
                                   "        name: main\n"
                                   "        return_type: I32\n"
                                   "        return_statement\n"
                                   "            integer_literal\n"
                                   "                type: I32\n"
                                   "                value: 0\n",
                                   test.source_paths[0],
                                   types[type_index].name,
                                   assignment_operators[operator_tag].token,
                                   types[type_index].name,
                                   types[type_index].file_friendly_default_value_name,
                                   types[type_index].name,
                                   !strcmp("Float", types[type_index].default_value_name) ? "0" : types[type_index].default_value);
            }
        }

        if (operator_tag == LAST_ASSIGNMENT_OPERATOR_TAG)
        {
            operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
            type_index++;
        }
        else
            operator_tag++;

        if (is_valid_case)
            return true;
    }
}

static bool x_typed_variable_x_type_compatible_operator_infer_incompatible_literal(void)
{
    static uint8_t lhs_type_index = 0;
    static uint8_t rhs_type_index = 0;
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;

    while (true)
    {
        if (lhs_type_index == TYPES_LENGTH)
            return false;

        bool is_valid_case = is_assignment_operator_defined_for_type(lhs_type_index, operator_tag) &&
                             is_assignment_operator_defined_for_type(rhs_type_index, operator_tag) &&
                             types[lhs_type_index].default_value_name &&
                             types[rhs_type_index].default_value_name &&
                             strcmp(types[lhs_type_index].default_value_name, types[rhs_type_index].default_value_name);

        if (is_valid_case)
        {
            format_test_name("semantics/statements/assignment/%s_typed_variable_%s_%s_literal", types[lhs_type_index].file_friendly_name, assignment_operators[operator_tag].file_friendly_name, types[rhs_type_index].file_friendly_name);

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
                                  "function a(%s b)\n"
                                  "    b %s %s\n"
                                  "end", 
                                  types[lhs_type_index].name,
                                  assignment_operators[operator_tag].token, types[rhs_type_index].default_value);

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error,
                               "%s:25:%u: Cannot infer literal as %s.\n",
                               test.source_paths[0],
                               8 + strlen(assignment_operators[operator_tag].token),
                               types[lhs_type_index].name);
        }

        if (rhs_type_index == LAST_TYPE_INDEX)
        {
            rhs_type_index = 0;
            if (operator_tag == LAST_ASSIGNMENT_OPERATOR_TAG)
            {
                operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
                lhs_type_index++;
            }
            else
                operator_tag++;
        }
        else
            rhs_type_index++;

        if (is_valid_case)
            return true;
    }
}

static bool x_typed_reference_expression_x_type_compatible_operator_x_typed_expression(void)
{
    static uint8_t type_index = 0;
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;

    while (true)
    {
        if (type_index > LAST_TYPE_INDEX)
            return false;

        bool is_valid_case = is_assignment_operator_defined_for_type(type_index, operator_tag);
        if (is_valid_case)
        {
            format_test_name("semantics/statements/assignment/%s_typed_reference_expression_%s_operator_%s_typed_expression", types[type_index].file_friendly_name,
                                                                                                                              assignment_operators[operator_tag].file_friendly_name, 
                                                                                                                              types[type_index].file_friendly_name);
    
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
                                  "function a(%s b)\n"
                                  "    b %s b\n"
                                  "end\n"
                                  "\n"
                                  "function main() : I32\n"
                                  "    return 0\n"
                                  "end",
                                  types[type_index].name,
                                  assignment_operators[operator_tag].token);

            format_command_line_options("%s -print-semantics", test.source_paths[0]);
            if (is_readonly_type(types[type_index].name))
            {
                format_expectation(&test.expected_error,
                                   "%s:25:5: Assigning to readonly expression.\n",
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
                                   "        assignment_statement\n"
                                   "            operator: %s\n"
                                   "            reference_expression\n"
                                   "                type: Union[I32]\n"
                                   "                name: b\n"
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
                                   test.source_paths[0],
                                   assignment_operators[operator_tag].token);
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
                                   "            name: b\n"
                                   "        assignment_statement\n"
                                   "            operator: %s\n"
                                   "            reference_expression\n"
                                   "                type: Structure[I32]\n"
                                   "                name: b\n"
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
                                   test.source_paths[0],
                                   assignment_operators[operator_tag].token);
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
                                   "        assignment_statement\n"
                                   "            operator: %s\n"
                                   "            reference_expression\n"
                                   "                type: %s\n"
                                   "                name: b\n"
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
                                   assignment_operators[operator_tag].token,
                                   types[type_index].name,
                                   types[type_index].name);
            }
        }

        if (operator_tag == LAST_ASSIGNMENT_OPERATOR_TAG)
        {
            operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
            type_index++;
        }
        else
            operator_tag++;
    
        if (is_valid_case)
            return true;
    }
}

static bool x_typed_reference_expression_x_type_compatible_operator_y_typed_expression(void)
{
    static uint8_t lhs_type_index = 0;
    static uint8_t rhs_type_index = 0;
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;

    while (true)
    {
        if (lhs_type_index > LAST_TYPE_INDEX)
            return false;

        bool is_valid_case = is_assignment_operator_defined_for_type(lhs_type_index, operator_tag) && 
                            !is_assignment_operator_defined_for_type(rhs_type_index, operator_tag) &&
                            ((lhs_type_index >= FIRST_POINTER_TYPE_INDEX && (rhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || rhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)) || 
                             (rhs_type_index >= FIRST_POINTER_TYPE_INDEX && (lhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || lhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)));

        if (is_valid_case)
        {
            format_test_name("semantics/statements/assignment/%s_typed_reference_expression_%s_operator_%s_typed_expression", types[lhs_type_index].file_friendly_name,
                                                                                                                              assignment_operators[operator_tag].file_friendly_name, 
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
                                  "function a(%s b, %s c)\n"
                                  "    b %s c\n"
                                  "end",
                                  types[lhs_type_index].name, types[rhs_type_index].name,
                                  assignment_operators[operator_tag].token);

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error,
                                "%s:25:7: Operator not defined for %s and %s.\n",
                                test.source_paths[0],
                                types[lhs_type_index].name, 
                                types[rhs_type_index].name);
        }

        if (rhs_type_index == LAST_TYPE_INDEX)
        {
            rhs_type_index = 0;
            if (operator_tag == LAST_ASSIGNMENT_OPERATOR_TAG)
            {
                operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
                lhs_type_index++;
            }
            else
                operator_tag++;
        }
        else
            rhs_type_index++;

        if (is_valid_case)
            return true;
    }
}

static bool x_typed_reference_expression_y_type_compatible_operator_y_typed_expression(void)
{
    static uint8_t lhs_type_index = 0;
    static uint8_t rhs_type_index = 0;
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;

    while (true)
    {
        if (lhs_type_index > LAST_TYPE_INDEX)
            return false;

        bool is_valid_case = !is_assignment_operator_defined_for_type(lhs_type_index, operator_tag) && 
                              is_assignment_operator_defined_for_type(rhs_type_index, operator_tag) &&
                            ((lhs_type_index >= FIRST_POINTER_TYPE_INDEX && (rhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || rhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)) || 
                             (rhs_type_index >= FIRST_POINTER_TYPE_INDEX && (lhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || lhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)));

        if (is_valid_case)
        {
            format_test_name("semantics/statements/assignment/%s_typed_reference_expression_%s_operator_%s_typed_expression", types[lhs_type_index].file_friendly_name,
                                                                                                                              assignment_operators[operator_tag].file_friendly_name, 
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
                                  "function a(%s b, %s c)\n"
                                  "    b %s c\n"
                                  "end",
                                  types[lhs_type_index].name, types[rhs_type_index].name,
                                  assignment_operators[operator_tag].token);

            format_command_line_options(test.source_paths[0]);
            if (is_readonly_type(types[lhs_type_index].name))
            {
                format_expectation(&test.expected_error, 
                                   "%s:25:5: Assigning to readonly expression.\n",
                                   test.source_paths[0]);
            }
            else
            {
                format_expectation(&test.expected_error, 
                                   "%s:25:7: Operator not defined for %s and %s.\n",
                                   test.source_paths[0],
                                   types[lhs_type_index].name, 
                                   types[rhs_type_index].name);
            }
        }

        if (rhs_type_index == LAST_TYPE_INDEX)
        {
            rhs_type_index = 0;
            if (operator_tag == LAST_ASSIGNMENT_OPERATOR_TAG)
            {
                operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
                lhs_type_index++;
            }
            else
                operator_tag++;
        }
        else
            rhs_type_index++;

        if (is_valid_case)
            return true;
    }
}

static bool x_typed_reference_expression_x_and_y_type_incompatible_operator_y_typed_expression(void)
{
    static uint8_t lhs_type_index = 0;
    static uint8_t rhs_type_index = 0;
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;

    while (true)
    {
        if (lhs_type_index > LAST_TYPE_INDEX)
            return false;

        bool is_valid_case = !is_assignment_operator_defined_for_type(lhs_type_index, operator_tag) && 
                             !is_assignment_operator_defined_for_type(rhs_type_index, operator_tag) &&
                            ((lhs_type_index >= FIRST_POINTER_TYPE_INDEX && (rhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || rhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)) || 
                             (rhs_type_index >= FIRST_POINTER_TYPE_INDEX && (lhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || lhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)));

        if (is_valid_case)
        {
            format_test_name("semantics/statements/assignment/%s_typed_reference_expression_%s_operator_%s_typed_expression", types[lhs_type_index].file_friendly_name,
                                                                                                                              assignment_operators[operator_tag].file_friendly_name, 
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
                                  "function a(%s b, %s c)\n"
                                  "    b %s c\n"
                                  "end",
                                  types[lhs_type_index].name, types[rhs_type_index].name,
                                  assignment_operators[operator_tag].token);

            format_command_line_options(test.source_paths[0]);
            if (is_readonly_type(types[lhs_type_index].name))
            {
                format_expectation(&test.expected_error, 
                                   "%s:25:5: Assigning to readonly expression.\n",
                                   test.source_paths[0]);
            }
            else
            {
                format_expectation(&test.expected_error, 
                                   "%s:25:7: Operator not defined for %s and %s.\n",
                                   test.source_paths[0],
                                   types[lhs_type_index].name, 
                                   types[rhs_type_index].name);
            }
        }

        if (rhs_type_index == LAST_TYPE_INDEX)
        {
            rhs_type_index = 0;
            if (operator_tag == LAST_ASSIGNMENT_OPERATOR_TAG)
            {
                operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
                lhs_type_index++;
            }
            else
                operator_tag++;
        }
        else
            rhs_type_index++;

        if (is_valid_case)
            return true;
    }
}

static bool x_typed_field_access_expression_x_type_compatible_operator_x_typed_expression(void)
{
    static uint8_t type_index = 0;
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;

    while (true)
    {
        if (type_index > FIRST_POINTER_TYPE_INDEX)
            return false;

        bool is_valid_case = is_assignment_operator_defined_for_type(type_index, operator_tag);
        if (is_valid_case)
        {
            format_test_name("semantics/statements/assignment/%s_typed_field_access_expression_%s_operator_%s_typed_expression", types[type_index].file_friendly_name,
                                                                                                                                 assignment_operators[operator_tag].file_friendly_name, 
                                                                                                                                 types[type_index].file_friendly_name);
    
            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "enumeration Enumeration : I32\n"
                                  "    a\n"
                                  "end\n"
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
                                  "function a(A b, %s c)\n"
                                  "    b.a %s c\n"
                                  "end\n"
                                  "\n"
                                  "function main() : I32\n"
                                  "    return 0\n"
                                  "end",
                                  types[type_index].name,
                                  types[type_index].name,
                                  assignment_operators[operator_tag].token);

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
                                   "        name: A\n"
                                   "        is_public: false\n"
                                   "        field\n"
                                   "            type: Union[I32]\n"
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
                                   "        name: a\n"
                                   "        formal_parameter\n"
                                   "            type: A\n"
                                   "            name: b\n"
                                   "        formal_parameter\n"
                                   "            type: Union[I32]\n"
                                   "            name: c\n"
                                   "        assignment_statement\n"
                                   "            operator: %s\n"
                                   "            field_access\n"
                                   "                type: Union[I32]\n"
                                   "                reference_expression\n"
                                   "                    type: A\n"
                                   "                    name: b\n"
                                   "                field: a\n"
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
                                   test.source_paths[0],
                                   assignment_operators[operator_tag].token);
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
                                   "        name: A\n"
                                   "        is_public: false\n"
                                   "        field\n"
                                   "            type: Structure[I32]\n"
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
                                   "        name: a\n"
                                   "        formal_parameter\n"
                                   "            type: A\n"
                                   "            name: b\n"
                                   "        formal_parameter\n"
                                   "            type: Structure[I32]\n"
                                   "            name: c\n"
                                   "        assignment_statement\n"
                                   "            operator: %s\n"
                                   "            field_access\n"
                                   "                type: Structure[I32]\n"
                                   "                reference_expression\n"
                                   "                    type: A\n"
                                   "                    name: b\n"
                                   "                field: a\n"
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
                                   test.source_paths[0],
                                   assignment_operators[operator_tag].token);
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
                                   "        name: a\n"
                                   "        formal_parameter\n"
                                   "            type: A\n"
                                   "            name: b\n"
                                   "        formal_parameter\n"
                                   "            type: %s\n"
                                   "            name: c\n"
                                   "        assignment_statement\n"
                                   "            operator: %s\n"
                                   "            field_access\n"
                                   "                type: %s\n"
                                   "                reference_expression\n"
                                   "                    type: A\n"
                                   "                    name: b\n"
                                   "                field: a\n"
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
                                   types[type_index].name,
                                   types[type_index].name,
                                   assignment_operators[operator_tag].token,
                                   types[type_index].name,
                                   types[type_index].name);
            }
        }

        if (operator_tag == LAST_ASSIGNMENT_OPERATOR_TAG)
        {
            operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
            type_index++;
        }
        else
            operator_tag++;
    
        if (is_valid_case)
            return true;
    }
}

static bool x_typed_field_access_expression_x_type_compatible_operator_y_typed_expression(void)
{
    static uint8_t lhs_type_index = 0;
    static uint8_t rhs_type_index = 0;
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;

    while (true)
    {
        if (lhs_type_index > LAST_TYPE_INDEX)
            return false;

        bool is_valid_case = is_assignment_operator_defined_for_type(lhs_type_index, operator_tag) && 
                            !is_assignment_operator_defined_for_type(rhs_type_index, operator_tag) &&
                            ((lhs_type_index >= FIRST_POINTER_TYPE_INDEX && (rhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || rhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)) || 
                             (rhs_type_index >= FIRST_POINTER_TYPE_INDEX && (lhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || lhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)));

        if (is_valid_case)
        {
            format_test_name("semantics/statements/assignment/%s_typed_field_access_expression_%s_operator_%s_typed_expression", types[lhs_type_index].file_friendly_name,
                                                                                                                                 assignment_operators[operator_tag].file_friendly_name, 
                                                                                                                                 types[rhs_type_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "enumeration Enumeration : I32\n"
                                  "    a\n"
                                  "end\n"
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
                                  "function a(A b, %s c)\n"
                                  "    b.a %s c\n"
                                  "end",
                                  types[lhs_type_index].name, 
                                  types[rhs_type_index].name,
                                  assignment_operators[operator_tag].token);

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error,
                                "%s:29:9: Operator not defined for %s and %s.\n",
                                test.source_paths[0], 
                                types[lhs_type_index].name, 
                                types[rhs_type_index].name);
        }

        if (rhs_type_index == LAST_TYPE_INDEX)
        {
            rhs_type_index = 0;
            if (operator_tag == LAST_ASSIGNMENT_OPERATOR_TAG)
            {
                operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
                lhs_type_index++;
            }
            else
                operator_tag++;
        }
        else
            rhs_type_index++;

        if (is_valid_case)
            return true;
    }
}

static bool x_typed_field_access_expression_y_type_compatible_operator_y_typed_expression(void)
{
    static uint8_t lhs_type_index = 0;
    static uint8_t rhs_type_index = 0;
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;

    while (true)
    {
        if (lhs_type_index > FIRST_POINTER_TYPE_INDEX)
            return false;

        bool is_valid_case = !is_assignment_operator_defined_for_type(lhs_type_index, operator_tag) && 
                              is_assignment_operator_defined_for_type(rhs_type_index, operator_tag) &&
                            ((lhs_type_index >= FIRST_POINTER_TYPE_INDEX && (rhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || rhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)) || 
                             (rhs_type_index >= FIRST_POINTER_TYPE_INDEX && (lhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || lhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)));

        if (is_valid_case)
        {
            format_test_name("semantics/statements/assignment/%s_typed_field_access_expression_%s_operator_%s_typed_expression", types[lhs_type_index].file_friendly_name,
                                                                                                                                 assignment_operators[operator_tag].file_friendly_name, 
                                                                                                                                 types[rhs_type_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "enumeration Enumeration : I32\n"
                                  "    a\n"
                                  "end\n"
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
                                  "function a(A b, %s c)\n"
                                  "    b.a %s c\n"
                                  "end",
                                  types[lhs_type_index].name, 
                                  types[rhs_type_index].name,
                                  assignment_operators[operator_tag].token);

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error, 
                               "%s:29:9: Operator not defined for %s and %s.\n",
                               test.source_paths[0],
                               types[lhs_type_index].name, 
                               types[rhs_type_index].name);
        }

        if (rhs_type_index == FIRST_POINTER_TYPE_INDEX)
        {
            rhs_type_index = 0;
            if (operator_tag == LAST_ASSIGNMENT_OPERATOR_TAG)
            {
                operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
                lhs_type_index++;
            }
            else
                operator_tag++;
        }
        else
            rhs_type_index++;

        if (is_valid_case)
            return true;
    }
}

static bool x_typed_field_access_expression_x_and_y_type_incompatible_operator_y_typed_expression(void)
{
    static uint8_t lhs_type_index = 0;
    static uint8_t rhs_type_index = 0;
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;

    while (true)
    {
        if (lhs_type_index > FIRST_POINTER_TYPE_INDEX)
            return false;

        bool is_valid_case = !is_assignment_operator_defined_for_type(lhs_type_index, operator_tag) && 
                             !is_assignment_operator_defined_for_type(rhs_type_index, operator_tag) &&
                            ((lhs_type_index >= FIRST_POINTER_TYPE_INDEX && (rhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || rhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)) || 
                             (rhs_type_index >= FIRST_POINTER_TYPE_INDEX && (lhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || lhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)));

        if (is_valid_case)
        {
            format_test_name("semantics/statements/assignment/%s_typed_field_access_expression_%s_operator_%s_typed_expression", types[lhs_type_index].file_friendly_name,
                                                                                                                                 assignment_operators[operator_tag].file_friendly_name, 
                                                                                                                                 types[rhs_type_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "enumeration Enumeration : I32\n"
                                  "    a\n"
                                  "end\n"
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
                                  "function a(A b, %s c)\n"
                                  "    b.a %s c\n"
                                  "end",
                                  types[lhs_type_index].name, 
                                  types[rhs_type_index].name,
                                  assignment_operators[operator_tag].token);

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error, 
                               "%s:29:9: Operator not defined for %s and %s.\n",
                               test.source_paths[0],
                               types[lhs_type_index].name, 
                               types[rhs_type_index].name);
        }

        if (rhs_type_index == FIRST_POINTER_TYPE_INDEX)
        {
            rhs_type_index = 0;
            if (operator_tag == LAST_ASSIGNMENT_OPERATOR_TAG)
            {
                operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
                lhs_type_index++;
            }
            else
                operator_tag++;
        }
        else
            rhs_type_index++;

        if (is_valid_case)
            return true;
    }
}


static bool x_typed_array_access_expression_x_type_compatible_operator_x_typed_expression(void)
{
    static uint8_t type_index = 0;
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;

    while (true)
    {
        if (type_index > FIRST_POINTER_TYPE_INDEX)
            return false;

        bool is_valid_case = is_assignment_operator_defined_for_type(type_index, operator_tag);
        if (is_valid_case)
        {
            format_test_name("semantics/statements/assignment/%s_typed_array_access_expression_%s_operator_%s_typed_expression", types[type_index].file_friendly_name,
                                                                                                                                 assignment_operators[operator_tag].file_friendly_name, 
                                                                                                                                 types[type_index].file_friendly_name);
    
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
                                  "function a([]%s b, %s c)\n"
                                  "    b[0] %s c\n"
                                  "end\n"
                                  "\n"
                                  "function main() : I32\n"
                                  "    return 0\n"
                                  "end",
                                  types[type_index].name, types[type_index].name,
                                  assignment_operators[operator_tag].token);

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
                                   "            type: []Union[I32]\n"
                                   "            name: b\n"
                                   "        formal_parameter\n"
                                   "            type: Union[I32]\n"
                                   "            name: c\n"
                                   "        assignment_statement\n"
                                   "            operator: %s\n"
                                   "            array_access\n"
                                   "                type: Union[I32]\n"
                                   "                reference_expression\n"
                                   "                    type: []Union[I32]\n"
                                   "                    name: b\n"
                                   "                integer_literal\n"
                                   "                    type: U32\n"
                                   "                    value: 0\n"
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
                                   test.source_paths[0],
                                   assignment_operators[operator_tag].token);
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
                                   "            type: []Structure[I32]\n"
                                   "            name: b\n"
                                   "        formal_parameter\n"
                                   "            type: Structure[I32]\n"
                                   "            name: c\n"
                                   "        assignment_statement\n"
                                   "            operator: %s\n"
                                   "            array_access\n"
                                   "                type: Structure[I32]\n"
                                   "                reference_expression\n"
                                   "                    type: []Structure[I32]\n"
                                   "                    name: b\n"
                                   "                integer_literal\n"
                                   "                    type: U32\n"
                                   "                    value: 0\n"
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
                                   test.source_paths[0],
                                   assignment_operators[operator_tag].token);
            }
            else if (type_index == FIXED_ARRAY_TYPE_INDEX)
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
                                   "            type: [][4]I32\n"
                                   "            name: b\n"
                                   "        formal_parameter\n"
                                   "            type: [4]I32\n"
                                   "            name: c\n"
                                   "        assignment_statement\n"
                                   "            operator: %s\n"
                                   "            array_access\n"
                                   "                type: [4]I32\n"
                                   "                reference_expression\n"
                                   "                    type: [][4]I32\n"
                                   "                    name: b\n"
                                   "                integer_literal\n"
                                   "                    type: U32\n"
                                   "                    value: 0\n"
                                   "            reference_expression\n"
                                   "                type: [4]I32\n"
                                   "                name: c\n"
                                   "    function\n"
                                   "        name: main\n"
                                   "        return_type: I32\n"
                                   "        return_statement\n"
                                   "            integer_literal\n"
                                   "                type: I32\n"
                                   "                value: 0\n",
                                   test.source_paths[0],
                                   assignment_operators[operator_tag].token);
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
                                   "            type: []%s\n"
                                   "            name: b\n"
                                   "        formal_parameter\n"
                                   "            type: %s\n"
                                   "            name: c\n"
                                   "        assignment_statement\n"
                                   "            operator: %s\n"
                                   "            array_access\n"
                                   "                type: %s\n"
                                   "                reference_expression\n"
                                   "                    type: []%s\n"
                                   "                    name: b\n"
                                   "                integer_literal\n"
                                   "                    type: U32\n"
                                   "                    value: 0\n"
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
                                   types[type_index].name,
                                   types[type_index].name,
                                   assignment_operators[operator_tag].token,
                                   types[type_index].name,
                                   types[type_index].name,
                                   types[type_index].name);
            }
        }

        if (operator_tag == LAST_ASSIGNMENT_OPERATOR_TAG)
        {
            operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
            type_index++;
        }
        else
            operator_tag++;
    
        if (is_valid_case)
            return true;
    }
}

static bool x_typed_array_access_expression_x_type_compatible_operator_y_typed_expression(void)
{
    static uint8_t lhs_type_index = 0;
    static uint8_t rhs_type_index = 0;
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;

    while (true)
    {
        if (lhs_type_index > LAST_TYPE_INDEX)
            return false;

        bool is_valid_case = is_assignment_operator_defined_for_type(lhs_type_index, operator_tag) && 
                            !is_assignment_operator_defined_for_type(rhs_type_index, operator_tag) &&
                            ((lhs_type_index >= FIRST_POINTER_TYPE_INDEX && (rhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || rhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)) || 
                             (rhs_type_index >= FIRST_POINTER_TYPE_INDEX && (lhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || lhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)));

        if (is_valid_case)
        {
            format_test_name("semantics/statements/assignment/%s_typed_array_access_expression_%s_operator_%s_typed_expression", types[lhs_type_index].file_friendly_name,
                                                                                                                                 assignment_operators[operator_tag].file_friendly_name, 
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
                                  "function a([]%s b, %s c)\n"
                                  "    b[0] %s c\n"
                                  "end",
                                  types[lhs_type_index].name, types[rhs_type_index].name,
                                  assignment_operators[operator_tag].token);

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error,
                                "%s:25:10: Operator not defined for %s and %s.\n",
                                test.source_paths[0],
                                types[lhs_type_index].name, 
                                types[rhs_type_index].name);
        }

        if (rhs_type_index == LAST_TYPE_INDEX)
        {
            rhs_type_index = 0;
            if (operator_tag == LAST_ASSIGNMENT_OPERATOR_TAG)
            {
                operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
                lhs_type_index++;
            }
            else
                operator_tag++;
        }
        else
            rhs_type_index++;

        if (is_valid_case)
            return true;
    }
}

static bool x_typed_array_access_expression_y_type_compatible_operator_y_typed_expression(void)
{
    static uint8_t lhs_type_index = 0;
    static uint8_t rhs_type_index = 0;
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;

    while (true)
    {
        if (lhs_type_index > FIRST_POINTER_TYPE_INDEX)
            return false;

        bool is_valid_case = !is_assignment_operator_defined_for_type(lhs_type_index, operator_tag) && 
                              is_assignment_operator_defined_for_type(rhs_type_index, operator_tag) &&
                            ((lhs_type_index >= FIRST_POINTER_TYPE_INDEX && (rhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || rhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)) || 
                             (rhs_type_index >= FIRST_POINTER_TYPE_INDEX && (lhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || lhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)));

        if (is_valid_case)
        {
            format_test_name("semantics/statements/assignment/%s_typed_array_access_expression_%s_operator_%s_typed_expression", types[lhs_type_index].file_friendly_name,
                                                                                                                                 assignment_operators[operator_tag].file_friendly_name, 
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
                                  "function a([]%s b, %s c)\n"
                                  "    b[0] %s c\n"
                                  "end",
                                  types[lhs_type_index].name, types[rhs_type_index].name,
                                  assignment_operators[operator_tag].token);

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error, 
                               "%s:25:10: Operator not defined for %s and %s.\n",
                               test.source_paths[0],
                               types[lhs_type_index].name, 
                               types[rhs_type_index].name);
        }

        if (rhs_type_index == FIRST_POINTER_TYPE_INDEX)
        {
            rhs_type_index = 0;
            if (operator_tag == LAST_ASSIGNMENT_OPERATOR_TAG)
            {
                operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
                lhs_type_index++;
            }
            else
                operator_tag++;
        }
        else
            rhs_type_index++;

        if (is_valid_case)
            return true;
    }
}

static bool x_typed_array_access_expression_x_and_y_type_incompatible_operator_y_typed_expression(void)
{
    static uint8_t lhs_type_index = 0;
    static uint8_t rhs_type_index = 0;
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;

    while (true)
    {
        if (lhs_type_index > FIRST_POINTER_TYPE_INDEX)
            return false;

        bool is_valid_case = !is_assignment_operator_defined_for_type(lhs_type_index, operator_tag) && 
                             !is_assignment_operator_defined_for_type(rhs_type_index, operator_tag) &&
                            ((lhs_type_index >= FIRST_POINTER_TYPE_INDEX && (rhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || rhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)) || 
                             (rhs_type_index >= FIRST_POINTER_TYPE_INDEX && (lhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || lhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)));

        if (is_valid_case)
        {
            format_test_name("semantics/statements/assignment/%s_typed_array_access_expression_%s_operator_%s_typed_expression", types[lhs_type_index].file_friendly_name,
                                                                                                                                 assignment_operators[operator_tag].file_friendly_name, 
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
                                  "function a([]%s b, %s c)\n"
                                  "    b[0] %s c\n"
                                  "end",
                                  types[lhs_type_index].name, types[rhs_type_index].name,
                                  assignment_operators[operator_tag].token);

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error, 
                               "%s:25:10: Operator not defined for %s and %s.\n",
                               test.source_paths[0],
                               types[lhs_type_index].name, 
                               types[rhs_type_index].name);
        }

        if (rhs_type_index == FIRST_POINTER_TYPE_INDEX)
        {
            rhs_type_index = 0;
            if (operator_tag == LAST_ASSIGNMENT_OPERATOR_TAG)
            {
                operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
                lhs_type_index++;
            }
            else
                operator_tag++;
        }
        else
            rhs_type_index++;

        if (is_valid_case)
            return true;
    }
}

static bool pointer_typed_parameter_operator_unsigned_integer_typed_expression(void)
{
    static uint8_t lhs_type_index = FIRST_POINTER_TYPE_INDEX;
    static uint8_t rhs_type_index = FIRST_UNSIGNED_NUMBER_TYPE_INDEX;
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;

    if (lhs_type_index > LAST_POINTER_TYPE_INDEX)
        return false;

    format_test_name("semantics/statements/assignment/%s_typed_expression_%s_operator_%s_typed_expression", types[lhs_type_index].file_friendly_name,
                                                                                                            assignment_operators[operator_tag].file_friendly_name, 
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
                           "function a(%s b, %s c)\n"
                           "    b %s c\n"
                           "end\n"
                           "\n"
                           "function main() : I32\n"
                           "    return 0\n"
                           "end",
                           types[lhs_type_index].name, 
                           types[rhs_type_index].name,
                           assignment_operators[operator_tag].token);

    format_command_line_options("%s -print-semantics", test.source_paths[0]);
    if (is_readonly_type(types[lhs_type_index].name))
    {
        format_expectation(&test.expected_error,
                           "%s:25:5: Assigning to readonly expression.\n",
                           test.source_paths[0]);
    }
    else if (operator_tag == Assignment_Operator_Tag_plus_equal || operator_tag == Assignment_Operator_Tag_minus_equal)
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
                           "        assignment_statement\n"
                           "            operator: %s\n"
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
                           types[lhs_type_index].name,
                           types[rhs_type_index].name,
                           assignment_operators[operator_tag].token,
                           types[lhs_type_index].name,
                           types[rhs_type_index].name);
    }
    else
    {
        format_expectation(&test.expected_error, 
                           "%s:25:7: Operator not defined for %s and %s.\n",
                           test.source_paths[0],
                           types[lhs_type_index].name, 
                           types[rhs_type_index].name);
    }

    if (rhs_type_index == LAST_UNSIGNED_NUMBER_TYPE_INDEX)
    {
        rhs_type_index = FIRST_UNSIGNED_NUMBER_TYPE_INDEX;
        if (operator_tag == LAST_ASSIGNMENT_OPERATOR_TAG)
        {
            operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
            lhs_type_index++;
        }
        else
            operator_tag++;
    }
    else
        rhs_type_index++;
        
    return true;
}

static void blank_identifier_on_lhs_in_ballanced_assignment(void)
{
    format_test_name("semantics/statements/assignment/blank_identifier_on_lhs_in_ballanced_assignment");

    format_source_path(0, "semantics/statements/assignment/blank_identifier_on_lhs_in_ballanced_assignment.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b)\n"
                          "    _ = b\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/assignment/blank_identifier_on_lhs_in_ballanced_assignment.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:5: _ only allowed in tuple declarations and assignments.\n",
                       test.source_paths[0]);
}

static void blank_identifier_on_rhs_in_ballanced_assignment(void)
{
    format_test_name("semantics/statements/assignment/blank_identifier_on_rhs_in_ballanced_assignment");

    format_source_path(0, "semantics/statements/assignment/blank_identifier_on_rhs_in_ballanced_assignment.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b)\n"
                          "    b = _\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/assignment/blank_identifier_on_rhs_in_ballanced_assignment.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:9: _ only allowed in tuple declarations and assignments.\n",
                       test.source_paths[0]);
}

static void blank_identifier_on_lhs_in_tuple_assignment(void)
{
    format_test_name("semantics/statements/assignment/blank_identifier_on_lhs_in_tuple_assignment");

    format_source_path(0, "semantics/statements/assignment/blank_identifier_on_lhs_in_tuple_assignment.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a() : I32, I32\n"
                          "    return 1, 2\n"
                          "end"
                          "\n"
                          "function b()\n"
                          "    _, _ = a()\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/assignment/blank_identifier_on_lhs_in_tuple_assignment.owen "
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
                       "        assignment_statement\n"
                       "            operator: =\n"
                       "            blank_identifier\n"
                       "                type: I32\n"
                       "            blank_identifier\n"
                       "                type: I32\n"
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

bool blanks_operator_tuple(void)
{
    static Assignment_Operator_Tag operator_tag = FIRST_ASSIGNMENT_OPERATOR_TAG;
    if (operator_tag > LAST_ASSIGNMENT_OPERATOR_TAG)
        return false;

    format_test_name("semantics/statements/assignment/blanks_%s_operator_tuple", assignment_operators[operator_tag].file_friendly_name);
    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a() : I32, I32\n"
                          "    return 1, 2\n"
                          "end\n"
                          "\n"
                          "function b()\n"
                          "    _, _ %s a()\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end",
                          assignment_operators[operator_tag].token);

    format_command_line_options("%s -print-semantics", test.source_paths[0]);
    if (operator_tag == Assignment_Operator_Tag_equal)
    {
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
                           "        assignment_statement\n"
                           "            operator: =\n"
                           "            blank_identifier\n"
                           "                type: I32\n"
                           "            blank_identifier\n"
                           "                type: I32\n"
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
    else
    {
        format_expectation(&test.expected_error,
                           "%s:9:10: Operator not defined for _.\n",
                           test.source_paths[0]);
    }

    operator_tag++;

    return true;
}

static bool assign_to_readonly_expression(void)
{
    static uint8_t state = 0;
    switch (state++)
    {
        case 0:
            format_test_name("semantics/statements/assignment/assign_to_compound_readonly_field");

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "structure S\n"
                                  "    I32 a\n"
                                  "end\n"
                                  "\n"
                                  "function a(readonly #S s)\n"
                                  "    s.a = 1\n"
                                  "end\n"
                                  "\n"
                                  "function main() : I32\n"
                                  "    return 0\n"
                                  "end");

            format_command_line_options("%s", test.source_paths[0]);
            format_expectation(&test.expected_error,
                               "%s:9:5: Assigning to readonly expression.\n",
                               test.source_paths[0]);
            break;
        
        case 1:
            format_test_name("semantics/statements/assignment/assign_to_dymnamic_array_readonly_field");

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "function a(readonly #[]I32 b)\n"
                                  "    b.length = 1\n"
                                  "end\n"
                                  "\n"
                                  "function main() : I32\n"
                                  "    return 0\n"
                                  "end");

            format_command_line_options("%s", test.source_paths[0]);
            format_expectation(&test.expected_error,
                               "%s:5:5: Assigning to readonly expression.\n",
                               test.source_paths[0]);
            break;
        
        case 2:
            format_test_name("semantics/statements/assignment/assign_to_fixed_array_readonly_field");

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "function a(readonly #[2]I32 b)\n"
                                  "    b.length = 1\n"
                                  "end\n"
                                  "\n"
                                  "function main() : I32\n"
                                  "    return 0\n"
                                  "end");

            format_command_line_options("%s", test.source_paths[0]);
            format_expectation(&test.expected_error,
                               "%s:5:5: Expression is not addressable.\n",
                               test.source_paths[0]);
            break;
        
        case 3:
            format_test_name("semantics/statements/assignment/assign_to_fixed_array_readonly_element");

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "function a(readonly #[2]I32 b)\n"
                                  "    b[0] = 1\n"
                                  "end\n"
                                  "\n"
                                  "function main() : I32\n"
                                  "    return 0\n"
                                  "end");

            format_command_line_options("%s", test.source_paths[0]);
            format_expectation(&test.expected_error,
                               "%s:5:6: Assigning to readonly expression.\n",
                               test.source_paths[0]);
            break;

        default:
            return false;
    }
    
    return true;
}

bool semantics_statements_assignment(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            if (x_typed_parameter_operator_none_typed_expression())
                break;
            
            state++;

        case 1:
            if (assign_x_pointer_tuple_compatible_variables())
                break;

            state++;

        case 2:
            if (assign_x_non_pointer_tuple_compatible_variables())
                break;

            state++;

        case 3:
            if (assign_x_non_pointer_tuple_incompatible_variables())
                break;

            state++;

        case 4:
            if (multiple_expressions_too_many_on_lhs_of_operator())
                break;

            state++;

        case 5:
            if (multiple_expressions_too_many_on_rhs_of_operator())
                break;

            state++;

        case 6:
            if (bool_literal_operator_expression())
                break;

            state++;

        case 7:
            if (parenthesized_expression_operator_expression())
                break;

            state++;
        
        case 8:
            if (not_expression_operator_expression())
                break;

            state++;

        case 9:
            if (negate_expression_operator_expression())
                break;

            state++;

        case 10:
            if (address_of_expression_operator_expression())
                break;
            
            state++;

        case 11:
            if (call_expression_operator_expression())
                break;

            state++;

        case 12:
            if (binary_expression_operator_expression())
                break;

            state++;

        case 13:
            if (dynamic_array_length_operator_expression())
                break;

            state++;

        case 14:
            if (dynamic_array_capacity_operator_expression())
                break;

            state++;

        case 15:
            dynamic_array_elements_equal_compatible_pointer();
            state++;
            break;

        case 16:
            if (fixed_array_length_operator_expression())
                break;

            state++;

        case 17:
            if (dereferenced_expression_operator_expression())
                break;

            state++;

        case 18:
            if (expression_operator_dereferenced_expression())
                break;

            state++;

        case 19:
            pointer_typed_field_access();
            state++;
            break;

        case 20:
            pointer_typed_array_access();
            state++;
            break;

        case 21:
            if (one_expression_too_many_on_lhs_of_operator())
                break;

            state++;

        case 22:
            if (one_expression_too_many_on_rhs_of_operator())
                break;

            state++;

        case 23:
            polymorphic_and_monomorphic_variables();
            state++;
            break;

        case 24:
            if (x_typed_variable_x_type_compatible_operator_infer_compatible_literal())
                break;

            state++;
            
        case 25:
            if (x_typed_variable_x_type_compatible_operator_infer_incompatible_literal())
                break;

            state++;

        case 26:
            if (x_typed_reference_expression_x_type_compatible_operator_x_typed_expression())
                break;

            state++;

        case 27:
            if (x_typed_reference_expression_x_type_compatible_operator_y_typed_expression())
                break;

            state++;

        case 28:
            if (x_typed_reference_expression_y_type_compatible_operator_y_typed_expression())
                break;

            state++;

        case 29:
            if (x_typed_reference_expression_x_and_y_type_incompatible_operator_y_typed_expression())
                break;

            state++;

        case 30:
            if (x_typed_field_access_expression_x_type_compatible_operator_x_typed_expression())
                break;

            state++;

        case 31:
            if (x_typed_field_access_expression_x_type_compatible_operator_y_typed_expression())
                break;

            state++;

        case 32:
            if (x_typed_field_access_expression_y_type_compatible_operator_y_typed_expression())
                break;

            state++;

        case 33:
            if (x_typed_field_access_expression_x_and_y_type_incompatible_operator_y_typed_expression())
                break;

            state++;
            
        case 34:
            if (x_typed_array_access_expression_x_type_compatible_operator_x_typed_expression())
                break;

            state++;

        case 35:
            if (x_typed_array_access_expression_x_type_compatible_operator_y_typed_expression())
                break;

            state++;

        case 36:
            if (x_typed_array_access_expression_y_type_compatible_operator_y_typed_expression())
                break;

            state++;

        case 37:
            if (x_typed_array_access_expression_x_and_y_type_incompatible_operator_y_typed_expression())
                break;

            state++;

        case 38:
            if (pointer_typed_parameter_operator_unsigned_integer_typed_expression())
                break;

            state++;

        case 39:
            blank_identifier_on_lhs_in_ballanced_assignment();
            state++;
            break;

        case 40:
            blank_identifier_on_rhs_in_ballanced_assignment();
            state++;
            break;

        case 41:
            blank_identifier_on_lhs_in_tuple_assignment();
            state++;
            break;

        case 42:
            if (blanks_operator_tuple())
                break;

            state++;

        case 43:
            if (assign_to_readonly_expression())
                break;
            
            state++;
        
        default:
            return false;
    }

    return true;
}
