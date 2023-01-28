#include "runner.h"
#include <assert.h>

static bool x_type_compatible_literal_operator_x_typed_expression(void)
{
    static uint8_t type_index = FIRST_NUMBER_TYPE_INDEX;
    static Binary_Operator_Tag operator_tag = FIRST_BINARY_OPERATOR_TAG;

    while (true)
    {
        if (type_index == LAST_NUMBER_TYPE_INDEX)
            return false;

        bool defined = is_binary_operator_defined_for_type(type_index, operator_tag);

        if (defined)
        {
            uint8_t operator_index = index_of_binary_operator(operator_tag);

            char* return_type = binary_operators[operator_index].is_relational ? "Bool" : types[type_index].name;
            
            format_test_name("semantics/expressions/binary/%s_type_compatible_%s_%s_operator_%s_typed_expression", types[type_index].file_friendly_name, types[type_index].file_friendly_default_value_name, binary_operators[operator_index].file_friendly_name, types[type_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                   "namespace Abc\n"
                                   "\n"
                                   "function a(%s b) : %s\n"
                                   "    return b %s %s\n"
                                   "end\n"
                                   "\n"
                                   "function main() : I32\n"
                                   "    return 0\n"
                                   "end",
                                   types[type_index].name, return_type,
                                   binary_operators[operator_index].token, types[type_index].default_value);

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
                               "        return_statement\n"
                               "            binary\n"
                               "                type: %s\n"
                               "                operator: %s\n"
                               "                reference_expression\n"
                               "                    type: %s\n"
                               "                    name: b\n"
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
                               types[type_index].name,
                               return_type,
                               return_type,
                               binary_operators[operator_index].token,
                               types[type_index].name, 
                               types[type_index].file_friendly_default_value_name,
                               types[type_index].name);
        }

        if (operator_tag == LAST_BINARY_OPERATOR_TAG)
        {
            operator_tag = FIRST_BINARY_OPERATOR_TAG;
            type_index++;
        }
        else
            operator_tag = next_binary_operator(operator_tag);
       
        if (defined)
            return true;
    }

    return false;
}

static bool x_typed_expression_operator_x_type_compatible_literal(void)
{
    static uint8_t type_index = FIRST_NUMBER_TYPE_INDEX;
    static Binary_Operator_Tag operator_tag = FIRST_BINARY_OPERATOR_TAG;

    while (true)
    {
        if (type_index == LAST_NUMBER_TYPE_INDEX)
            return false;

        bool defined = is_binary_operator_defined_for_type(type_index, operator_tag);

        if (defined)
        {
            uint8_t operator_index = index_of_binary_operator(operator_tag);

            char* return_type = binary_operators[operator_index].is_relational ? "Bool" : types[type_index].name;
            
            format_test_name("semantics/expressions/binary/%s_typed_expression_%s_operator_%s_type_compatible_%s", types[type_index].file_friendly_name, binary_operators[operator_index].file_friendly_name, types[type_index].file_friendly_name, types[type_index].file_friendly_default_value_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                   "namespace Abc\n"
                                   "\n"
                                   "function a(%s b) : %s\n"
                                   "    return %s %s b\n"
                                   "end\n"
                                   "\n"
                                   "function main() : I32\n"
                                   "    return 0\n"
                                   "end",
                                   types[type_index].name, return_type,
                                   types[type_index].default_value, binary_operators[operator_index].token);

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
                               "        return_statement\n"
                               "            binary\n"
                               "                type: %s\n"
                               "                operator: %s\n"
                               "                %s_literal\n"
                               "                    type: %s\n"
                               "                    value: 0\n"
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
                               return_type,
                               return_type,
                               binary_operators[operator_index].token,
                               types[type_index].file_friendly_default_value_name,
                               types[type_index].name, 
                               types[type_index].name);
        }

        if (operator_tag == LAST_BINARY_OPERATOR_TAG)
        {
            operator_tag = FIRST_BINARY_OPERATOR_TAG;
            type_index++;
        }
        else
            operator_tag = next_binary_operator(operator_tag);
       
        if (defined)
            return true;
    }

    return false;
}


static bool infer_float_literal_x_operator_integer_float_as_x_type(void)
{
    static uint8_t type_index = FIRST_FLOAT_TYPE_INDEX;
    static Binary_Operator_Tag operator_tag = FIRST_BINARY_OPERATOR_TAG;

    while (true)
    {
        if (type_index == LAST_FLOAT_TYPE_INDEX)
            return false;

        uint8_t operator_index = index_of_binary_operator(operator_tag);
        bool is_valid_case = is_binary_operator_defined_for_type(type_index, operator_tag) && !binary_operators[operator_index].is_relational;
        
        if (is_valid_case)
        {
            format_test_name("semantics/expressions/binary/infer_float_literal_%s_operator_float_literal_as_%s_type", binary_operators[operator_index].file_friendly_name, types[type_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "function a() : %s\n"
                                  "    return %s %s %s\n"
                                  "end\n" 
                                  "\n"
                                  "function main() : I32\n"
                                  "    return 0\n"
                                  "end",
                                  types[type_index].name,
                                  types[type_index].default_value, binary_operators[operator_index].token, types[type_index].default_value);

            format_command_line_options("%s -print-semantics", test.source_paths[0]);
            format_expectation(&test.expected_semantics, 
                               "file\n"
                               "    path: %s\n"
                               "    namespace: Abc\n"
                               "    function\n"
                               "        name: a\n"
                               "        return_type: %s\n"
                               "        return_statement\n"
                               "            binary\n"
                               "                type: %s\n"
                               "                operator: %s\n"
                               "                float_literal\n"
                               "                    type: %s\n"
                               "                    value: 0\n"
                               "                float_literal\n"
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
                               types[type_index].name,
                               types[type_index].name,
                               binary_operators[operator_index].token,
                               types[type_index].name,
                               types[type_index].name);
        }

        if (operator_tag == LAST_BINARY_OPERATOR_TAG)
        {
            operator_tag = FIRST_BINARY_OPERATOR_TAG;
            type_index++;
        }
        else
            operator_tag = next_binary_operator(operator_tag);
       
        if (is_valid_case)
            return true;
    }

    return false;
}

static bool infer_integer_literal_x_operator_integer_literal_as_x_type(void)
{
    static uint8_t type_index = FIRST_INTEGER_TYPE_INDEX;
    static Binary_Operator_Tag operator_tag = FIRST_BINARY_OPERATOR_TAG;

    while (true)
    {
        if (type_index == LAST_INTEGER_TYPE_INDEX)
            return false;

        uint8_t operator_index = index_of_binary_operator(operator_tag);
        bool is_valid_case = is_binary_operator_defined_for_type(type_index, operator_tag) && !binary_operators[operator_index].is_relational;
        if (is_valid_case)
        {
            
            format_test_name("semantics/expressions/binary/infer_integer_literal_%s_operator_integer_literal_as_%s_type", binary_operators[operator_index].file_friendly_name, types[type_index].name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "function a() : %s\n"
                                  "    return %s %s %s\n"
                                  "end\n"
                                  "\n"
                                  "function main() : I32\n"
                                  "    return 0\n"
                                  "end", 
                                  types[type_index].name,
                                  types[type_index].default_value, binary_operators[operator_index].token, types[type_index].default_value);

            format_command_line_options("%s -print-semantics", test.source_paths[0]);
            format_expectation(&test.expected_semantics, 
                               "file\n"
                               "    path: %s\n"
                               "    namespace: Abc\n"
                               "    function\n"
                               "        name: a\n"
                               "        return_type: %s\n"
                               "        return_statement\n"
                               "            binary\n"
                               "                type: %s\n"
                               "                operator: %s\n"
                               "                integer_literal\n"
                               "                    type: %s\n"
                               "                    value: 0\n"
                               "                integer_literal\n"
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
                               types[type_index].name,
                               types[type_index].name,
                               binary_operators[operator_index].token,
                               types[type_index].name,
                               types[type_index].name);
        }
        
        if (operator_tag == LAST_BINARY_OPERATOR_TAG)
        {
            operator_tag = FIRST_BINARY_OPERATOR_TAG;
            type_index++;
        }
        else
            operator_tag = next_binary_operator(operator_tag);
       
        if (is_valid_case)
            return true;
    }

    return false;
}

static bool x_typed_expression_operator_defined_for_x_type_incompatible_literal(void)
{
    static uint8_t lhs_type_index = 0;
    static uint8_t rhs_type_index = 0;
    static Binary_Operator_Tag operator_tag = FIRST_BINARY_OPERATOR_TAG;

    while (true)
    {
        if (lhs_type_index > LAST_TYPE_INDEX)
            return false;

        uint8_t operator_index = index_of_binary_operator(operator_tag);
        char* binary_expression_type = binary_operators[operator_index].is_relational ? "Bool" : types[lhs_type_index].name;

        bool is_valid_case = is_binary_operator_defined_for_type(lhs_type_index, operator_tag) && 
                             is_binary_operator_defined_for_type(rhs_type_index, operator_tag) && 
                             types[rhs_type_index].default_value && 
                             lhs_type_index != rhs_type_index && 
                             types[lhs_type_index].file_friendly_default_value_name &&
                             types[rhs_type_index].file_friendly_default_value_name &&
                             !strcmp(binary_expression_type, types[lhs_type_index].name) && 
                             strcmp(types[lhs_type_index].file_friendly_default_value_name, types[rhs_type_index].file_friendly_default_value_name);

        if (is_valid_case)
        {
            format_test_name("semantics/expressions/binary/%s_typed_expression_%s_%s_typed_%s", types[lhs_type_index].file_friendly_name, binary_operators[operator_index].file_friendly_name, types[rhs_type_index].file_friendly_name, types[rhs_type_index].file_friendly_default_value_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "enumeration Enumeration : I32\n"
                                  "    a\n"
                                  "end\n"
                                  "\n"
                                  "union Union\n"
                                  "    I32 a\n"
                                  "end\n"
                                  "\n"
                                  "structure Structure\n"
                                  "    I32 a\n"
                                  "end\n"
                                  "\n"
                                  "function a(%s b) : %s\n"
                                  "    return b %s %s\n"
                                  "end", types[lhs_type_index].name, types[lhs_type_index].name,
                                  binary_operators[operator_index].token, types[rhs_type_index].default_value);

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error, 
                                "%s:17:%u: Cannot infer literal as %s.\n",
                                test.source_paths[0],
                                15 + strlen(binary_operators[operator_index].token),
                                types[lhs_type_index].name);   
        }

        if (rhs_type_index == LAST_TYPE_INDEX)
        {
            rhs_type_index = 0;
            if (operator_tag == LAST_BINARY_OPERATOR_TAG)
            {
                operator_tag = FIRST_BINARY_OPERATOR_TAG;
                lhs_type_index++;
            }
            else
                operator_tag = next_binary_operator(operator_tag);
        }
        else
            rhs_type_index++;

        if (is_valid_case)
            return true;
    }

    return false;
}

static bool type_incompatible_literal_operator_defined_for_x_x_typed_expression(void)
{
    static uint8_t lhs_type_index = 0;
    static uint8_t rhs_type_index = 0;
    static Binary_Operator_Tag operator_tag = FIRST_BINARY_OPERATOR_TAG;

    while (true)
    {
        if (lhs_type_index > LAST_TYPE_INDEX)
            return false;

        uint8_t operator_index = index_of_binary_operator(operator_tag);
        char* binary_expression_type = binary_operators[operator_index].is_relational ? "Bool" : types[lhs_type_index].name;

        bool is_valid_case = is_binary_operator_defined_for_type(lhs_type_index, operator_tag) && 
                             is_binary_operator_defined_for_type(rhs_type_index, operator_tag) && 
                             types[rhs_type_index].default_value && 
                             lhs_type_index != rhs_type_index && 
                             types[lhs_type_index].file_friendly_default_value_name &&
                             types[rhs_type_index].file_friendly_default_value_name &&
                             !strcmp(binary_expression_type, types[lhs_type_index].name) && 
                             strcmp(types[lhs_type_index].file_friendly_default_value_name, types[rhs_type_index].file_friendly_default_value_name);

        if (is_valid_case)
        {
            format_test_name("semantics/expressions/binary/%s_typed_%s_%s_operator_%s_typed_expression", types[lhs_type_index].file_friendly_name, types[rhs_type_index].file_friendly_default_value_name, binary_operators[operator_index].file_friendly_name, types[rhs_type_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "enumeration Enumeration : I32\n"
                                  "    a\n"
                                  "end\n"
                                  "\n"
                                  "union Union\n"
                                  "    I32 a\n"
                                  "end\n"
                                  "\n"
                                  "structure Structure\n"
                                  "    I32 a\n"
                                  "end\n"
                                  "\n"
                                  "function a(%s b) : %s\n"
                                  "    return %s %s b\n"
                                  "end", types[lhs_type_index].name, types[lhs_type_index].name,
                                  types[rhs_type_index].default_value, binary_operators[operator_index].token);

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error, 
                                "%s:17:12: Cannot infer literal as %s.\n",
                                test.source_paths[0],
                                types[lhs_type_index].name);
        }

        if (rhs_type_index == LAST_TYPE_INDEX)
        {
            rhs_type_index = 0;
            if (operator_tag == LAST_BINARY_OPERATOR_TAG)
            {
                operator_tag = FIRST_BINARY_OPERATOR_TAG;
                lhs_type_index++;
            }
            else
                operator_tag = next_binary_operator(operator_tag);
        }
        else
            rhs_type_index++;

        if (is_valid_case)
            return true;
    }

    return false;
}

static bool x_typed_expression_x_type_compatible_operator_x_typed_expression(void)
{
    static uint8_t type_index = 0;
    static Binary_Operator_Tag operator_tag = FIRST_BINARY_OPERATOR_TAG;

    while (true)
    {
        if (type_index == LAST_TYPE_INDEX)
            return false;

        bool defined = is_binary_operator_defined_for_type(type_index, operator_tag);

        if (defined)
        {
            uint8_t operator_index = index_of_binary_operator(operator_tag);

            char* return_type = binary_operators[operator_index].is_relational ? "Bool" : types[type_index].name;
            
            format_test_name("semantics/expressions/binary/%s_typed_expression_%s_operator_%s_typed_expression", types[type_index].file_friendly_name, binary_operators[operator_index].file_friendly_name, types[type_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "enumeration Enumeration : I32\n"
                                  "    a\n"
                                  "end\n"
                                  "\n"
                                  "function a(%s b) : %s\n"
                                  "    return b %s b\n"
                                  "end\n"
                                  "\n"
                                  "function main() : I32\n"
                                  "    return 0\n"
                                  "end",
                                  types[type_index].name, return_type,
                                  binary_operators[operator_index].token, types[type_index].default_value);

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
                               "    function\n"
                               "        name: a\n"
                               "        formal_parameter\n"
                               "            type: %s\n"
                               "            name: b\n"
                               "        return_type: %s\n"
                               "        return_statement\n"
                               "            binary\n"
                               "                type: %s\n"
                               "                operator: %s\n"
                               "                reference_expression\n"
                               "                    type: %s\n"
                               "                    name: b\n"
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
                               return_type,
                               return_type,
                               binary_operators[operator_index].token,
                               types[type_index].name,
                               types[type_index].name);
        }

        if (operator_tag == LAST_BINARY_OPERATOR_TAG)
        {
            operator_tag = FIRST_BINARY_OPERATOR_TAG;
            type_index++;
        }
        else
            operator_tag = next_binary_operator(operator_tag);
       
        if (defined)
            return true;
    }
}

static bool x_typed_expression_x_type_compatible_operator_y_typed_expression(void)
{
    static uint8_t lhs_type_index = 0;
    static uint8_t rhs_type_index = 0;
    static Binary_Operator_Tag operator_tag = FIRST_BINARY_OPERATOR_TAG;

    while (true)
    {
        if (lhs_type_index > LAST_TYPE_INDEX)
            return false;

        uint8_t operator_index = index_of_binary_operator(operator_tag);
        char* return_type = NULL;
        if (rhs_type_index >= FIRST_POINTER_TYPE_INDEX)
            return_type = binary_operators[operator_index].is_relational ? "Bool" : types[rhs_type_index].name;
        else
            return_type = binary_operators[operator_index].is_relational ? "Bool" : types[lhs_type_index].name;

        bool is_valid_case = is_binary_operator_defined_for_type(lhs_type_index, operator_tag) && 
                            !is_binary_operator_defined_for_type(rhs_type_index, operator_tag) &&
                            !is_noalias_type(return_type) &&
                            ((lhs_type_index >= FIRST_POINTER_TYPE_INDEX && (rhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || rhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)) || 
                             (rhs_type_index >= FIRST_POINTER_TYPE_INDEX && (lhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || lhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)));

        if (is_valid_case)
        {
            format_test_name("semantics/expressions/binary/%s_typed_expression_%s_operator_%s_typed_expression", types[lhs_type_index].file_friendly_name, binary_operators[operator_index].file_friendly_name, types[rhs_type_index].file_friendly_name);

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
                                  "function a(%s b, %s c) : %s\n"
                                  "    return b %s c\n"
                                  "end",
                                  types[lhs_type_index].name, types[rhs_type_index].name, return_type,
                                  binary_operators[operator_index].token);

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error, 
                               "%s:25:14: Operator not defined for %s and %s.\n",
                               test.source_paths[0],
                               types[lhs_type_index].name, 
                               types[rhs_type_index].name);
        }

        if (rhs_type_index == LAST_TYPE_INDEX)
        {
            rhs_type_index = 0;
            if (operator_tag == LAST_BINARY_OPERATOR_TAG)
            {
                operator_tag = FIRST_BINARY_OPERATOR_TAG;
                lhs_type_index++;
            }
            else
                operator_tag = next_binary_operator(operator_tag);
        }
        else
            rhs_type_index++;

        if (is_valid_case)
            return true;
    }
}

static bool x_typed_expression_y_type_compatible_operator_y_typed_expression(void)
{
    static uint8_t lhs_type_index = 0;
    static uint8_t rhs_type_index = 0;
    static Binary_Operator_Tag operator_tag = FIRST_BINARY_OPERATOR_TAG;

    while (true)
    {
        if (lhs_type_index > LAST_TYPE_INDEX)
            return false;

        uint8_t operator_index = index_of_binary_operator(operator_tag);
        char* return_type = NULL;
        if (rhs_type_index >= FIRST_POINTER_TYPE_INDEX)
            return_type = binary_operators[operator_index].is_relational ? "Bool" : types[rhs_type_index].name;
        else
            return_type = binary_operators[operator_index].is_relational ? "Bool" : types[lhs_type_index].name;

        bool is_valid_case = !is_binary_operator_defined_for_type(lhs_type_index, operator_tag) && 
                              is_binary_operator_defined_for_type(rhs_type_index, operator_tag) &&
                             !is_noalias_type(return_type) &&
                            ((lhs_type_index >= FIRST_POINTER_TYPE_INDEX && (rhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || rhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)) || 
                             (rhs_type_index >= FIRST_POINTER_TYPE_INDEX && (lhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || lhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)));

        if (is_valid_case)
        {    
            format_test_name("semantics/expressions/binary/%s_typed_expression_%s_operator_%s_typed_expression", types[lhs_type_index].file_friendly_name, binary_operators[operator_index].file_friendly_name, types[rhs_type_index].file_friendly_name);

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
                                  "function a(%s b, %s c) : %s\n"
                                  "    return b %s c\n"
                                  "end",
                                  types[lhs_type_index].name, types[rhs_type_index].name, return_type,
                                  binary_operators[operator_index].token);

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error, 
                               "%s:25:14: Operator not defined for %s and %s.\n",
                               test.source_paths[0],
                               types[lhs_type_index].name, 
                               types[rhs_type_index].name);
        }

        if (rhs_type_index == LAST_TYPE_INDEX)
        {
            rhs_type_index = 0;
            if (operator_tag == LAST_BINARY_OPERATOR_TAG)
            {
                operator_tag = FIRST_BINARY_OPERATOR_TAG;
                lhs_type_index++;
            }
            else
                operator_tag = next_binary_operator(operator_tag);
        }
        else
            rhs_type_index++;

        if (is_valid_case)
            return true;
    }
}

static bool x_typed_expression_x_and_y_type_incompatible_operator_y_typed_expression(void)
{
    static uint8_t lhs_type_index = 0;
    static uint8_t rhs_type_index = 0;
    static Binary_Operator_Tag operator_tag = FIRST_BINARY_OPERATOR_TAG;

    while (true)
    {
        if (lhs_type_index > LAST_TYPE_INDEX)
            return false;

        uint8_t operator_index = index_of_binary_operator(operator_tag);
        char* return_type = NULL;
        if (rhs_type_index >= FIRST_POINTER_TYPE_INDEX)
            return_type = binary_operators[operator_index].is_relational ? "Bool" : types[rhs_type_index].name;
        else
            return_type = binary_operators[operator_index].is_relational ? "Bool" : types[lhs_type_index].name;

        bool is_valid_case = !is_binary_operator_defined_for_type(lhs_type_index, operator_tag) && 
                             !is_binary_operator_defined_for_type(rhs_type_index, operator_tag) &&
                             !is_noalias_type(return_type) &&
                            ((lhs_type_index >= FIRST_POINTER_TYPE_INDEX && (rhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || rhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)) || 
                             (rhs_type_index >= FIRST_POINTER_TYPE_INDEX && (lhs_type_index < FIRST_UNSIGNED_NUMBER_TYPE_INDEX || lhs_type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)));

        if (is_valid_case)
        {    
            format_test_name("semantics/expressions/binary/%s_typed_expression_%s_operator_%s_typed_expression", types[lhs_type_index].file_friendly_name, binary_operators[operator_index].file_friendly_name, types[rhs_type_index].file_friendly_name);

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
                                  "function a(%s b, %s c) : %s\n"
                                  "    return b %s c\n"
                                  "end",
                                  types[lhs_type_index].name, types[rhs_type_index].name, return_type,
                                  binary_operators[operator_index].token);

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error, 
                               "%s:25:14: Operator not defined for %s and %s.\n",
                               test.source_paths[0],
                               types[lhs_type_index].name, 
                               types[rhs_type_index].name);
        }

        if (rhs_type_index == LAST_TYPE_INDEX)
        {
            rhs_type_index = 0;
            if (operator_tag == LAST_BINARY_OPERATOR_TAG)
            {
                operator_tag = FIRST_BINARY_OPERATOR_TAG;
                lhs_type_index++;
            }
            else
                operator_tag = next_binary_operator(operator_tag);
        }
        else
            rhs_type_index++;

        if (is_valid_case)
            return true;
    }
}

static bool x_typed_expression_operator_none_typed_expression(void)
{
    static uint8_t type_index = 0;
    static Binary_Operator_Tag operator_tag = FIRST_BINARY_OPERATOR_TAG;

    while (true)
    {
        if (type_index == LAST_TYPE_INDEX)
            return false;

        uint8_t operator_index = index_of_binary_operator(operator_tag);
        format_test_name("semantics/expressions/binary/%s_typed_expression_%s_operator_none_typed_expression", types[type_index].file_friendly_name, binary_operators[operator_index].file_friendly_name);

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
                               "function b(%s c) : I32\n"
                               "    return c %s a()\n"
                               "end",
                               types[type_index].name,
                               binary_operators[operator_index].token);

        format_command_line_options(test.source_paths[0]);
        format_expectation(&test.expected_error, 
                           "%s:28:14: Operator not defined for %s and none.\n",
                           test.source_paths[0],
                           types[type_index].name);

        if (operator_tag == LAST_BINARY_OPERATOR_TAG)
        {
            operator_tag = FIRST_BINARY_OPERATOR_TAG;
            type_index++;
        }
        else
            operator_tag = next_binary_operator(operator_tag);

        return true;
    }
}

static bool none_typed_expression_operator_x_typed_expression(void)
{
    static uint8_t type_index = 0;
    static Binary_Operator_Tag operator_tag = FIRST_BINARY_OPERATOR_TAG;

    while (true)
    {
        if (type_index == LAST_TYPE_INDEX)
            return false;

        uint8_t operator_index = index_of_binary_operator(operator_tag);
        format_test_name("semantics/expressions/binary/none_typed_expression_%s_operator_%s_typed_expression", binary_operators[operator_index].file_friendly_name, types[type_index].file_friendly_name);

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
                               "function a()\n"
                               "end\n"
                               "\n"
                               "function b(%s c) : I32\n"
                               "    return a() %s c\n"
                               "end",
                               types[type_index].name,
                               binary_operators[operator_index].token);

        format_command_line_options(test.source_paths[0]);
        format_expectation(&test.expected_error, 
                           "%s:27:16: Operator not defined for none and %s.\n",
                           test.source_paths[0],
                           types[type_index].name);

        if (operator_tag == LAST_BINARY_OPERATOR_TAG)
        {
            operator_tag = FIRST_BINARY_OPERATOR_TAG;
            type_index++;
        }
        else
            operator_tag = next_binary_operator(operator_tag);

        return true;
    }
}

static bool x_typed_expression_operator_tuple_typed_expression(void)
{
    static uint8_t type_index = 0;
    static Binary_Operator_Tag operator_tag = FIRST_BINARY_OPERATOR_TAG;

    while (true)
    {
        if (type_index == LAST_TYPE_INDEX)
            return false;

        uint8_t operator_index = index_of_binary_operator(operator_tag);
        format_test_name("semantics/expressions/binary/%s_typed_expression_%s_operator_tuple_typed_expression", types[type_index].file_friendly_name, binary_operators[operator_index].file_friendly_name);

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
                               "function a() : I32, I32\n"
                               "    return 0, 0\n"
                               "end\n"
                               "\n"
                               "function b(%s c) : I32\n"
                               "    return c %s a()\n"
                               "end",
                               types[type_index].name,
                               binary_operators[operator_index].token);

        format_command_line_options(test.source_paths[0]);
        format_expectation(&test.expected_error, 
                           "%s:29:14: Operator not defined for %s and I32, I32.\n",
                           test.source_paths[0],
                           types[type_index].name);

        if (operator_tag == LAST_BINARY_OPERATOR_TAG)
        {
            operator_tag = FIRST_BINARY_OPERATOR_TAG;
            type_index++;
        }
        else
            operator_tag = next_binary_operator(operator_tag);

        return true;
    }
}

static bool tuple_typed_expression_operator_x_typed_expression(void)
{
    static uint8_t type_index = 0;
    static Binary_Operator_Tag operator_tag = FIRST_BINARY_OPERATOR_TAG;

    while (true)
    {
        if (type_index == LAST_TYPE_INDEX)
            return false;

        uint8_t operator_index = index_of_binary_operator(operator_tag);
        format_test_name("semantics/expressions/binary/tuple_typed_expression_%s_operator_%s_typed_expression", binary_operators[operator_index].file_friendly_name, types[type_index].file_friendly_name);

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
                               "function a() : I32, I32\n"
                               "    return 0, 0\n"
                               "end\n"
                               "\n"
                               "function b(%s c) : I32\n"
                               "    return a() %s c\n"
                               "end",
                               types[type_index].name,
                               binary_operators[operator_index].token);

        format_command_line_options(test.source_paths[0]);
        format_expectation(&test.expected_error, 
                           "%s:29:16: Operator not defined for I32, I32 and %s.\n",
                           test.source_paths[0],
                           types[type_index].name);

        if (operator_tag == LAST_BINARY_OPERATOR_TAG)
        {
            operator_tag = FIRST_BINARY_OPERATOR_TAG;
            type_index++;
        }
        else
            operator_tag = next_binary_operator(operator_tag);

        return true;
    }
}

static bool pointer_typed_expression_plus_or_minus_uxx_typed_expression(void)
{
    static uint8_t lhs_type_index = FIRST_POINTER_TYPE_INDEX;
    static uint8_t rhs_type_index = FIRST_UNSIGNED_NUMBER_TYPE_INDEX;
    static Binary_Operator_Tag operator_tag = Binary_Operator_Tag_plus;

    while (true)
    {
        if (lhs_type_index == LAST_POINTER_TYPE_INDEX)
            return false;

        char* return_type = types[lhs_type_index].name;
        bool is_valid_case = !is_noalias_type(return_type);

        if (is_valid_case)
        {
            uint8_t operator_index = index_of_binary_operator(operator_tag);
            format_test_name("semantics/expressions/binary/%s_typed_expression_%s_operator_%s_typed_expression", types[lhs_type_index].file_friendly_name, binary_operators[operator_index].file_friendly_name, types[rhs_type_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                    "namespace Abc\n"
                                    "\n"
                                    "function a(%s b, %s c) : %s\n"
                                    "    return b %s c\n"
                                    "end\n"
                                    "\n"
                                    "function main() : I32\n"
                                    "    return 0\n"
                                    "end",
                                    types[lhs_type_index].name, types[rhs_type_index].name, return_type,
                                    binary_operators[operator_index].token);

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
                                "            type: %s\n"
                                "            name: c\n"
                                "        return_type: %s\n"
                                "        return_statement\n"
                                "            binary\n"
                                "                type: %s\n"
                                "                operator: %s\n"
                                "                reference_expression\n"
                                "                    type: %s\n"
                                "                    name: b\n"
                                "                reference_expression\n"
                                "                    type: %s\n"
                                "                    name: c\n"
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
                                return_type,
                                types[lhs_type_index].name,
                                binary_operators[operator_index].token,
                                types[lhs_type_index].name,
                                types[rhs_type_index].name);
        }

        if (rhs_type_index == LAST_UNSIGNED_NUMBER_TYPE_INDEX - 1)
        {
            rhs_type_index = FIRST_UNSIGNED_NUMBER_TYPE_INDEX;
            if (operator_tag == Binary_Operator_Tag_minus)
            {
                operator_tag = Binary_Operator_Tag_plus;
                lhs_type_index++;
            }
            else
                operator_tag = next_binary_operator(operator_tag);
        }
        else
            rhs_type_index++;

        if (is_valid_case)
            return true;
    }
}

static bool uxx_typed_expression_plus_or_minus_pointer_typed_expression(void)
{
    static uint8_t lhs_type_index = FIRST_UNSIGNED_NUMBER_TYPE_INDEX;
    static uint8_t rhs_type_index = FIRST_POINTER_TYPE_INDEX;
    static Binary_Operator_Tag operator_tag = Binary_Operator_Tag_plus;

    while (true)
    {
        if (lhs_type_index == LAST_UNSIGNED_NUMBER_TYPE_INDEX)
            return false;

        char* return_type = types[rhs_type_index].name;
        bool is_valid_case = !is_noalias_type(return_type);

        if (is_valid_case)
        {
            uint8_t operator_index = index_of_binary_operator(operator_tag);
            format_test_name("semantics/expressions/binary/%s_typed_expression_%s_operator_%s_typed_expression", types[lhs_type_index].file_friendly_name, binary_operators[operator_index].file_friendly_name, types[rhs_type_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                   "namespace Abc\n"
                                   "\n"
                                   "function a(%s b, %s c) : %s\n"
                                   "    return b %s c\n"
                                   "end\n"
                                   "function main() : I32\n"
                                   "    return 0\n"
                                   "end",
                                   types[lhs_type_index].name, types[rhs_type_index].name, return_type,
                                   binary_operators[operator_index].token);

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
                               "            type: %s\n"
                               "            name: c\n"
                               "        return_type: %s\n"
                               "        return_statement\n"
                               "            binary\n"
                               "                type: %s\n"
                               "                operator: %s\n"
                               "                reference_expression\n"
                               "                    type: %s\n"
                               "                    name: b\n"
                               "                reference_expression\n"
                               "                    type: %s\n"
                               "                    name: c\n"
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
                               return_type,
                               types[rhs_type_index].name,
                               binary_operators[operator_index].token,
                               types[lhs_type_index].name,
                               types[rhs_type_index].name);
        }

        if (rhs_type_index == LAST_POINTER_TYPE_INDEX - 1)
        {
            rhs_type_index = FIRST_POINTER_TYPE_INDEX;
            if (operator_tag == Binary_Operator_Tag_minus)
            {
                operator_tag = Binary_Operator_Tag_plus;
                lhs_type_index++;
            }
            else
                operator_tag = next_binary_operator(operator_tag);
        }
        else
            rhs_type_index++;

        if (is_valid_case)
            return true;
    }
}

static void monomorphic_binary_in_polymorphic_function(void)
{
    format_test_name("semantics/expressions/binary/monomorphic_binary_in_polymorphic_function");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T](I32 b) : I32\n"
                          "    return b + b\n"
                          "end\n"
                          "\n"
                          "function c(I32 d) : I32\n"
                          "    return a[I32](d)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/binary/monomorphic_binary_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: %s\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: I32\n"
                       "            name: b\n"
                       "        return_type_reference: I32\n"
                       "        return_statement\n"
                       "            binary\n"
                       "                operator: +\n"
                       "                reference_expression\n"
                       "                    name: b\n"
                       "                reference_expression\n"
                       "                    name: b\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: b\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            binary\n"
                       "                type: I32\n"
                       "                operator: +\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: b\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: b\n"
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
                       "                value: 0\n",
                       test.source_paths[0]);
}

static void polymorphic_binary_in_polymorphic_function(void)
{
    format_test_name("semantics/expressions/binary/polymorphic_binary_in_polymorphic_function");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T](T b) : T\n"
                          "    return b + b\n"
                          "end\n"
                          "\n"
                          "function c(I32 d) : I32\n"
                          "    return a(d)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/binary/polymorphic_binary_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: %s\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: T\n"
                       "            name: b\n"
                       "        return_type_reference: T\n"
                       "        return_statement\n"
                       "            binary\n"
                       "                operator: +\n"
                       "                reference_expression\n"
                       "                    name: b\n"
                       "                reference_expression\n"
                       "                    name: b\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: b\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            binary\n"
                       "                type: I32\n"
                       "                operator: +\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: b\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: b\n"
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

bool semantics_expressions_binary(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            if (x_type_compatible_literal_operator_x_typed_expression())
                break;

            state++;

        case 1:
            if (x_typed_expression_operator_x_type_compatible_literal())
                break;

            state++;

        case 2:
            if (infer_float_literal_x_operator_integer_float_as_x_type())
                break;

            state++;

        case 3:
            if (infer_integer_literal_x_operator_integer_literal_as_x_type())
                break;

            state++;

        case 4:
            if (x_typed_expression_operator_defined_for_x_type_incompatible_literal())
                break;

            state++;

        case 5:
            if (type_incompatible_literal_operator_defined_for_x_x_typed_expression())
                break;

            state++;

        case 6:
            if (x_typed_expression_x_type_compatible_operator_x_typed_expression())
                break;

            state++;

        case 7:
            if (x_typed_expression_x_type_compatible_operator_y_typed_expression())
                break;

            state++;

        case 8:
            if (x_typed_expression_y_type_compatible_operator_y_typed_expression())
                break;

            state++;

        case 9:
            if (x_typed_expression_x_and_y_type_incompatible_operator_y_typed_expression())
                break;

            state++;

        case 10:
            if (x_typed_expression_operator_none_typed_expression())
                break;

            state++;

        case 11:
            if (none_typed_expression_operator_x_typed_expression())
                break;

            state++;

        case 12:
            if (x_typed_expression_operator_tuple_typed_expression())
                break;

            state++;

        case 13:
            if (tuple_typed_expression_operator_x_typed_expression())
                break;

            state++;

        case 14:
            if (pointer_typed_expression_plus_or_minus_uxx_typed_expression())
                break;

            state++;

        case 15:
            if (uxx_typed_expression_plus_or_minus_pointer_typed_expression())
                break;

            state++;

        case 16:
            monomorphic_binary_in_polymorphic_function();
            state++;
            break;

        case 17:
            polymorphic_binary_in_polymorphic_function();
            state++;
            break;

        default:
            return false;
    }

    return true;
}
