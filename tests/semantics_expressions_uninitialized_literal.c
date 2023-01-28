#include "runner.h"

static bool x_typed(void)
{    
    static uint8_t index = 0;
    if (index == TYPES_LENGTH)
        return false;

    format_test_name("semantics/expressions/uninitialized_literal/%s_typed", types[index].file_friendly_name);

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

static void nested(void)
{
    format_test_name("semantics/expressions/uninitialized_literal/nested");

    format_source_path(0, "semantics/expressions/uninitialized_literal/nested.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function c(I32 d) : I32\n"
                          "    return d + ---\n"
                          "end\n");

    SET_COMMAND_LINE("semantics/expressions/uninitialized_literal/nested.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:16: Unitialized literals cannot be nested.\n",
                       test.source_paths[0]);
}

bool semantics_expressions_uninitialized_literal(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            if (x_typed())
                break;

            state++;

        case 1:
            nested();
            state++;
            break;

        default:
            return false;
    }

    return true;
}
