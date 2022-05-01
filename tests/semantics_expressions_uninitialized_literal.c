#include "runner.h"

static bool x_typed(void)
{    
    static uint8_t index = 0;
    if (index == TYPES_LENGTH)
        return false;

    format_test_name("semantics/expressions/uninitialized_literal/%s_typed", types[index].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated by " __FILE__ " " __FUNCSIG__ "\n"
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

    if (!strcmp("Union[I32]", types[index].name))
    {
        format_expectation(&test.expected_semantics, 
                           "file" NEW_LINE
                           "    path: %s" NEW_LINE
                           "    namespace: Abc" NEW_LINE
                           "    enumeration" NEW_LINE
                           "        name: Enumeration" NEW_LINE
                           "        underlying_type: I32" NEW_LINE
                           "        is_public: false" NEW_LINE
                           "        constant" NEW_LINE
                           "            name: a" NEW_LINE
                           "            value: 0" NEW_LINE
                           "    structure" NEW_LINE
                           "        name: Structure" NEW_LINE
                           "        is_public: false" NEW_LINE
                           "        field" NEW_LINE
                           "            type: I32" NEW_LINE
                           "            name: a" NEW_LINE
                           "    structure" NEW_LINE
                           "        name: Structure" NEW_LINE
                           "        is_public: false" NEW_LINE
                           "        formal_type_parameter: T" NEW_LINE
                           "        field" NEW_LINE
                           "            type_reference: T" NEW_LINE
                           "            name: a" NEW_LINE
                           "    union" NEW_LINE
                           "        name: Union" NEW_LINE
                           "        is_public: false" NEW_LINE
                           "        field" NEW_LINE
                           "            type: I32" NEW_LINE
                           "            name: a" NEW_LINE
                           "    union" NEW_LINE
                           "        name: Union" NEW_LINE
                           "        is_public: false" NEW_LINE
                           "        formal_type_parameter: T" NEW_LINE
                           "        field" NEW_LINE
                           "            type_reference: T" NEW_LINE
                           "            name: a" NEW_LINE
                           "    union" NEW_LINE
                           "        name: Union" NEW_LINE
                           "        is_public: false" NEW_LINE
                           "        actual_type_parameter: I32" NEW_LINE
                           "        field" NEW_LINE
                           "            type: I32" NEW_LINE
                           "            name: a" NEW_LINE
                           "    function" NEW_LINE
                           "        name: a" NEW_LINE
                           "        return_type: Union[I32]" NEW_LINE
                           "        declaration_statement" NEW_LINE
                           "            variable" NEW_LINE
                           "                type: Union[I32]" NEW_LINE
                           "                name: b" NEW_LINE
                           "            uninitialized_literal" NEW_LINE
                           "                type: Union[I32]" NEW_LINE
                           "        return_statement" NEW_LINE
                           "            reference_expression" NEW_LINE
                           "                type: Union[I32]" NEW_LINE
                           "                name: b" NEW_LINE
                           "    function" NEW_LINE
                           "        name: main" NEW_LINE
                           "        return_type: I32" NEW_LINE
                           "        return_statement" NEW_LINE
                           "            integer_literal" NEW_LINE
                           "                type: I32" NEW_LINE
                           "                value: 0" NEW_LINE,
                           test.source_paths[0]);
    }
    else if (!strcmp("Structure[I32]", types[index].name))
    {
        format_expectation(&test.expected_semantics, 
                           "file" NEW_LINE
                           "    path: %s" NEW_LINE
                           "    namespace: Abc" NEW_LINE
                           "    enumeration" NEW_LINE
                           "        name: Enumeration" NEW_LINE
                           "        underlying_type: I32" NEW_LINE
                           "        is_public: false" NEW_LINE
                           "        constant" NEW_LINE
                           "            name: a" NEW_LINE
                           "            value: 0" NEW_LINE
                           "    structure" NEW_LINE
                           "        name: Structure" NEW_LINE
                           "        is_public: false" NEW_LINE
                           "        field" NEW_LINE
                           "            type: I32" NEW_LINE
                           "            name: a" NEW_LINE
                           "    structure" NEW_LINE
                           "        name: Structure" NEW_LINE
                           "        is_public: false" NEW_LINE
                           "        formal_type_parameter: T" NEW_LINE
                           "        field" NEW_LINE
                           "            type_reference: T" NEW_LINE
                           "            name: a" NEW_LINE
                           "    structure" NEW_LINE
                           "        name: Structure" NEW_LINE
                           "        is_public: false" NEW_LINE
                           "        actual_type_parameter: I32" NEW_LINE
                           "        field" NEW_LINE
                           "            type: I32" NEW_LINE
                           "            name: a" NEW_LINE
                           "    union" NEW_LINE
                           "        name: Union" NEW_LINE
                           "        is_public: false" NEW_LINE
                           "        field" NEW_LINE
                           "            type: I32" NEW_LINE
                           "            name: a" NEW_LINE
                           "    union" NEW_LINE
                           "        name: Union" NEW_LINE
                           "        is_public: false" NEW_LINE
                           "        formal_type_parameter: T" NEW_LINE
                           "        field" NEW_LINE
                           "            type_reference: T" NEW_LINE
                           "            name: a" NEW_LINE
                           "    function" NEW_LINE
                           "        name: a" NEW_LINE
                           "        return_type: Structure[I32]" NEW_LINE
                           "        declaration_statement" NEW_LINE
                           "            variable" NEW_LINE
                           "                type: Structure[I32]" NEW_LINE
                           "                name: b" NEW_LINE
                           "            uninitialized_literal" NEW_LINE
                           "                type: Structure[I32]" NEW_LINE
                           "        return_statement" NEW_LINE
                           "            reference_expression" NEW_LINE
                           "                type: Structure[I32]" NEW_LINE
                           "                name: b" NEW_LINE
                           "    function" NEW_LINE
                           "        name: main" NEW_LINE
                           "        return_type: I32" NEW_LINE
                           "        return_statement" NEW_LINE
                           "            integer_literal" NEW_LINE
                           "                type: I32" NEW_LINE
                           "                value: 0" NEW_LINE,
                           test.source_paths[0]);
    }
    else
    {
        format_expectation(&test.expected_semantics, 
                           "file" NEW_LINE
                           "    path: %s" NEW_LINE
                           "    namespace: Abc" NEW_LINE
                           "    enumeration" NEW_LINE
                           "        name: Enumeration" NEW_LINE
                           "        underlying_type: I32" NEW_LINE
                           "        is_public: false" NEW_LINE
                           "        constant" NEW_LINE
                           "            name: a" NEW_LINE
                           "            value: 0" NEW_LINE
                           "    structure" NEW_LINE
                           "        name: Structure" NEW_LINE
                           "        is_public: false" NEW_LINE
                           "        field" NEW_LINE
                           "            type: I32" NEW_LINE
                           "            name: a" NEW_LINE
                           "    structure" NEW_LINE
                           "        name: Structure" NEW_LINE
                           "        is_public: false" NEW_LINE
                           "        formal_type_parameter: T" NEW_LINE
                           "        field" NEW_LINE
                           "            type_reference: T" NEW_LINE
                           "            name: a" NEW_LINE
                           "    union" NEW_LINE
                           "        name: Union" NEW_LINE
                           "        is_public: false" NEW_LINE
                           "        field" NEW_LINE
                           "            type: I32" NEW_LINE
                           "            name: a" NEW_LINE
                           "    union" NEW_LINE
                           "        name: Union" NEW_LINE
                           "        is_public: false" NEW_LINE
                           "        formal_type_parameter: T" NEW_LINE
                           "        field" NEW_LINE
                           "            type_reference: T" NEW_LINE
                           "            name: a" NEW_LINE
                           "    function" NEW_LINE
                           "        name: a" NEW_LINE
                           "        return_type: %s" NEW_LINE
                           "        declaration_statement" NEW_LINE
                           "            variable" NEW_LINE
                           "                type: %s" NEW_LINE
                           "                name: b" NEW_LINE
                           "            uninitialized_literal" NEW_LINE
                           "                type: %s" NEW_LINE
                           "        return_statement" NEW_LINE
                           "            reference_expression" NEW_LINE
                           "                type: %s" NEW_LINE
                           "                name: b" NEW_LINE
                           "    function" NEW_LINE
                           "        name: main" NEW_LINE
                           "        return_type: I32" NEW_LINE
                           "        return_statement" NEW_LINE
                           "            integer_literal" NEW_LINE
                           "                type: I32" NEW_LINE
                           "                value: 0" NEW_LINE,
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
    format_source_file(0, "// Generated by " __FILE__ " " __FUNCSIG__ "\n"
                          "namespace Abc\n"
                          "\n"
                          "function c(I32 d) : I32\n"
                          "    return d + ---\n"
                          "end\n");

    SET_COMMAND_LINE("semantics/expressions/uninitialized_literal/nested.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:16: Unitialized literals cannot be nested." NEW_LINE,
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
