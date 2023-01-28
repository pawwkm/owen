#include "runner.h"

static bool x_typed_parameter_where_y_type_is_expected(void)
{
    static uint8_t parameter_type = 0;
    static uint8_t return_type = 0;

    if (return_type > LAST_TYPE_INDEX)
        return false;

    format_test_name("semantics/expressions/address_of/%s_typed_parameter_where_%s_type_is_expected", types[parameter_type].file_friendly_name, types[return_type].file_friendly_address_of_name);

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
                          "function a(%s b) : %s\n"
                          "    return #b\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end\n",
                          types[parameter_type].name,
                          types[return_type].address_of_name);

    format_command_line_options("%s -print-semantics", test.source_paths[0]);
    if (compatible_types(types[return_type].address_of_name, types[parameter_type].address_of_name) && !is_noalias_type(types[return_type].name))
    {
        if (!strcmp(types[parameter_type].address_of_name, "#Union[I32]"))
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
                               "        return_type: #Union[I32]\n"
                               "        return_statement\n"
                               "            address_of\n"
                               "                type: #Union[I32]\n"
                               "                reference_expression\n"
                               "                    type: Union[I32]\n"
                               "                    name: b\n"
                               "    function\n"
                               "        name: main\n"
                               "        return_type: I32\n"
                               "        return_statement\n"
                               "            integer_literal\n"
                               "                type: I32\n"
                               "                value: 0\n",
                               test.source_paths[0]);
        }
        else if (!strcmp(types[parameter_type].address_of_name, "#Structure[I32]"))
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
                               "        return_type: #Structure[I32]\n"
                               "        return_statement\n"
                               "            address_of\n"
                               "                type: #Structure[I32]\n"
                               "                reference_expression\n"
                               "                    type: Structure[I32]\n"
                               "                    name: b\n"
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
                               "        return_statement\n"
                               "            address_of\n"
                               "                type: %s\n"
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
                               types[parameter_type].name,
                               types[return_type].address_of_name,
                               types[parameter_type].address_of_name,
                               types[parameter_type].name);
        }
    }
    else if (is_noalias_type(types[return_type].address_of_name))
    {
        format_expectation(&test.expected_error, "%s:24:%u: Only Pointerless formal parameter types can be noalias qualified.\n",
                           test.source_paths[0],
                           18 + strlen(types[parameter_type].name));
    }
    else
    {
        format_expectation(&test.expected_error, "%s:25:12: %s expected but found %s.\n",
                           test.source_paths[0],
                           types[return_type].address_of_name,
                           types[parameter_type].address_of_name);
    }

    parameter_type++;
    if (parameter_type > LAST_TYPE_INDEX)
    {
        parameter_type = 0;
        return_type++;
    }

    return true;
}

static bool x_typed_variable_where_y_type_is_expected(void)
{
    static uint8_t parameter_type = 0;
    static uint8_t return_type = 0;

    if (return_type == TYPES_LENGTH)
        return false;

    format_test_name("semantics/expressions/address_of/%s_typed_variable_where_%s_type_is_expected", types[parameter_type].file_friendly_name, types[return_type].file_friendly_name);

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
                          "function a(%s b) : %s\n"
                          "    %s c = #b\n"
                          "    return c\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end\n",
                          types[parameter_type].name,
                          types[return_type].address_of_name,
                          types[return_type].address_of_name);

    format_command_line_options("%s -print-semantics", test.source_paths[0]);
    if (compatible_types(types[return_type].address_of_name, types[parameter_type].address_of_name) && !is_noalias_type(types[return_type].name))
    {
        if (!strcmp(types[parameter_type].address_of_name, "#Union[I32]"))
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
                               "        return_type: #Union[I32]\n"
                               "        declaration_statement\n"
                               "            variable\n"
                               "                type: #Union[I32]\n"
                               "                name: c\n"
                               "            address_of\n"
                               "                type: #Union[I32]\n"
                               "                reference_expression\n"
                               "                    type: Union[I32]\n"
                               "                    name: b\n"
                               "        return_statement\n"
                               "            reference_expression\n"
                               "                type: #Union[I32]\n"
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
        else if (!strcmp(types[parameter_type].address_of_name, "#Structure[I32]"))
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
                               "        return_type: #Structure[I32]\n"
                               "        declaration_statement\n"
                               "            variable\n"
                               "                type: #Structure[I32]\n"
                               "                name: c\n"
                               "            address_of\n"
                               "                type: #Structure[I32]\n"
                               "                reference_expression\n"
                               "                    type: Structure[I32]\n"
                               "                    name: b\n"
                               "        return_statement\n"
                               "            reference_expression\n"
                               "                type: #Structure[I32]\n"
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
                               "        declaration_statement\n"
                               "            variable\n"
                               "                type: %s\n"
                               "                name: c\n"
                               "            address_of\n"
                               "                type: %s\n"
                               "                reference_expression\n"
                               "                    type: %s\n"
                               "                    name: b\n"
                               "        return_statement\n"
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
                               types[parameter_type].name,
                               types[return_type].address_of_name,
                               types[return_type].address_of_name,
                               types[parameter_type].address_of_name,
                               types[parameter_type].name,
                               types[return_type].address_of_name);
        }
    }
    else if (is_noalias_type(types[return_type].address_of_name))
    {
        format_expectation(&test.expected_error, "%s:24:%u: Only Pointerless formal parameter types can be noalias qualified.\n",
                           test.source_paths[0],
                           18 + strlen(types[parameter_type].name));
    }
    else
    {
        format_expectation(&test.expected_error, "%s:25:%u: %s expected but found %s.\n",
                           test.source_paths[0],
                           10 + strlen(types[return_type].address_of_name),
                           types[return_type].address_of_name,
                           types[parameter_type].address_of_name);
    }

    parameter_type++;
    if (parameter_type > LAST_TYPE_INDEX)
    {
        parameter_type = 0;
        return_type++;
    }

    return true;
}

bool semantics_expressions_address_of(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            if (x_typed_parameter_where_y_type_is_expected())
                break;

            state++;

        case 1:
            if (x_typed_variable_where_y_type_is_expected())
                break;

            state++;

        default:
            return false;
    }

    return true;
}
