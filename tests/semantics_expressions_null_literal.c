#include "runner.h"

static bool x_typed_literal(void)
{
    static uint8_t index;
    if (index > LAST_TYPE_INDEX)
        return false;

    format_test_name("semantics/expressions/null_literal/%s_typed_literal", types[index].file_friendly_name);

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
                          "    b = null\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end",
                          types[index].name);

    format_command_line_options("%s -print-semantics", test.source_paths[0]);
    if (is_readonly_type(types[index].name))
    {
        format_expectation(&test.expected_error,
                           "%s:25:5: Assigning to readonly expression.\n",
                           test.source_paths[0]);
    }
    else if (index >= FIRST_POINTER_TYPE_INDEX && index <= LAST_POINTER_TYPE_INDEX)
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
                           "            operator: =\n"
                           "            reference_expression\n"
                           "                type: %s\n"
                           "                name: b\n"
                           "            null_literal\n"
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
                           types[index].name,
                           types[index].name);
    }
    else
    {
        format_expectation(&test.expected_error, 
                           "%s:25:9: Pointer type expected but found %s.\n",
                           test.source_paths[0],
                           types[index].name);
    }

    index++;

    return true;
}

bool semantics_expressions_null_literal(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            if (x_typed_literal())
                break;

            state++;

        default:
            return false;
    }

    return true;
}
