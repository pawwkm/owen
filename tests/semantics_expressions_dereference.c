#include "runner.h"

static bool x_typed_expression(void)
{
    static uint8_t type_index = 0;
    if (type_index > LAST_TYPE_INDEX)
        return false;

    format_test_name("semantics/expressions/dereference/%s_typed_expression", types[type_index].file_friendly_name);

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
                          "function a(%s b) : I32\n"
                          "    return @b\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end",
                          types[type_index].name);

    format_command_line_options("%s -print-semantics", test.source_paths[0]);
    if (type_index >= FIRST_POINTER_TYPE_INDEX && type_index <= LAST_POINTER_TYPE_INDEX)
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
                           "        return_type: I32\n"
                           "        return_statement\n"
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
    else
    {
        format_expectation(&test.expected_error, 
                           "%s:25:13: Pointer type expected but found %s.\n",
                           test.source_paths[0],
                           types[type_index].name);
    }

    type_index++;

    return true;
}

bool semantics_expressions_dereference(void)
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
