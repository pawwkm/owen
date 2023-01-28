#include "runner.h"

static bool x_typed_expression(void)
{    
    static uint8_t index = 0;
    if (index == TYPES_LENGTH)
        return false;

    format_test_name("semantics/expressions/negate/%s_typed_expression", types[index].file_friendly_name);

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
                          "function b(%s c) : %s\n"
                          "    return -c\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end", 
                          types[index].name,
                          types[index].name);

    format_command_line_options("%s -print-semantics", test.source_paths[0]);
    if (index >= FIRST_SIGNED_NUMBER_TYPE_INDEX && index <= LAST_SIGNED_NUMBER_TYPE_INDEX || index >= FIRST_FLOAT_TYPE_INDEX && index <= LAST_FLOAT_TYPE_INDEX)
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
                           "        name: b\n"
                           "        formal_parameter\n"
                           "            type: %s\n"
                           "            name: c\n"
                           "        return_type: %s\n"
                           "        return_statement\n"
                           "            negate\n"
                           "                type: %s\n"
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
                           types[index].name,
                           types[index].name,
                           types[index].name,
                           types[index].name);
    }
    else if (is_noalias_type(types[index].name))
    {
        format_expectation(&test.expected_error,
                           "%s:24:%u: Only Pointerless formal parameter types can be noalias qualified.\n",
                           test.source_paths[0],
                           18 + strlen(types[index].name));
    }
    else
    {
        format_expectation(&test.expected_error, 
                           "%s:25:13: Operator not defined for %s.\n",
                           test.source_paths[0],
                           types[index].name);
    }

    index++;
    
    return true;
}

static bool x_typed_literal(void)
{    
    static uint8_t index = 0;
    while (true)
    {
        if (index == TYPES_LENGTH)
            return false;

        bool is_valid_case = index >= FIRST_SIGNED_NUMBER_TYPE_INDEX && index <= LAST_SIGNED_NUMBER_TYPE_INDEX || index >= FIRST_FLOAT_TYPE_INDEX && index <= LAST_FLOAT_TYPE_INDEX;
        if (is_valid_case)
        {
            format_test_name("semantics/expressions/negate/%s_typed_literal", types[index].file_friendly_name);
            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "function b() : %s\n"
                                  "    return -%s\n"
                                  "end\n"
                                  "\n"
                                  "function main() : I32\n"
                                  "    return 0\n"
                                  "end",
                                  types[index].name,
                                  index >= FIRST_FLOAT_TYPE_INDEX && index <= LAST_FLOAT_TYPE_INDEX ? "1.0" : "1");

            format_command_line_options("%s -print-semantics", test.source_paths[0]);
            format_expectation(&test.expected_semantics, 
                               "file\n"
                               "    path: %s\n"
                               "    namespace: Abc\n"
                               "    function\n"
                               "        name: b\n"
                               "        return_type: %s\n"
                               "        return_statement\n"
                               "            %s_literal\n"
                               "                type: %s\n"
                               "                value: -1\n"
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

        index++;
        if (is_valid_case)
            break;
    }
    
    return true;
}

bool semantics_expressions_negate(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            if (x_typed_expression())
                break;

            state++;

        case 1:
            if (x_typed_literal())
                break;

            state++;

        default:
            return false;
    }

    return true;
}
