#include "runner.h"

static bool x_typed_expression(void)
{    
    static uint8_t index = 0;
    if (index == TYPES_LENGTH)
        return false;

    format_test_name("semantics/expressions/not/%s_typed_expression", types[index].file_friendly_name);

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
                          "function b(%s c) : Bool\n"
                          "    return !c\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end", 
                          types[index].name);

    if (index == BOOL_TYPE_INDEX)
    {
        format_command_line_options("%s -print-semantics", test.source_paths[0]);
        format_expectation(&test.expected_semantics, 
                           "file\n"
                           "    path: semantics/expressions/not/bool_typed_expression.owen\n"
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
                           "            type: Bool\n"
                           "            name: c\n"
                           "        return_type: Bool\n"
                           "        return_statement\n"
                           "            not\n"
                           "                type: Bool\n"
                           "                reference_expression\n"
                           "                    type: Bool\n"
                           "                    name: c\n"
                           "    function\n"
                           "        name: main\n"
                           "        return_type: I32\n"
                           "        return_statement\n"
                           "            integer_literal\n"
                           "                type: I32\n"
                           "                value: 0\n");
    }
    else
    {
        format_command_line_options(test.source_paths[0]);
        format_expectation(&test.expected_error, 
                           "%s:25:13: Bool expected but found %s.\n",
                           test.source_paths[0],
                           types[index].name);
    }

    index++;
    
    return true;
}

bool semantics_expressions_not(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            if (x_typed_expression())
                break;

            state++;

        default:
            return false;
    }

    return true;
}
