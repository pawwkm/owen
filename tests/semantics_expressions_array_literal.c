#include "runner.h"

static bool initialize_non_pointer_x_typed_element_as_non_pointer_x_typed_expression(void)
{
    static uint8_t index = 0;
    while (index < FIRST_POINTER_TYPE_INDEX && !types[index].default_value_name)
        index++;

    if (index == FIRST_POINTER_TYPE_INDEX)
        return false;

    format_test_name("semantics/expressions/array_literal/initialize_%s_typed_element_as_%s_typed_expression", types[index].file_friendly_name, types[index].file_friendly_name);

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
                          "function a(%s b) : [2]%s\n"
                          "    return { b }\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end", 
                          types[index].name,
                          types[index].name);

    format_command_line_options("%s -print-semantics", test.source_paths[0]);
    if (index == POLYMORPHIC_STRUCTURE_TYPE_INDEX)
    {
        format_expectation(&test.expected_semantics, 
                           "file\n"
                           "    path: semantics/expressions/array_literal/initialize_polymorphic_structure_typed_element_as_polymorphic_structure_typed_expression.owen\n"
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
                           "        return_type: [2]Structure[I32]\n"
                           "        return_statement\n"
                           "            array_literal\n"
                           "                type: [2]Structure[I32]\n"
                           "                element_initializer\n"
                           "                    index: 0\n"
                           "                    reference_expression\n"
                           "                        type: Structure[I32]\n"
                           "                        name: b\n"
                           "    function\n"
                           "        name: main\n"
                           "        return_type: I32\n"
                           "        return_statement\n"
                           "            integer_literal\n"
                           "                type: I32\n"
                           "                value: 0\n");
    }
    else if (index == POLYMORPHIC_UNION_TYPE_INDEX)
    {
        format_expectation(&test.expected_semantics, 
                           "file\n"
                           "    path: semantics/expressions/array_literal/initialize_polymorphic_union_typed_element_as_polymorphic_union_typed_expression.owen\n"
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
                           "        return_type: [2]Union[I32]\n"
                           "        return_statement\n"
                           "            array_literal\n"
                           "                type: [2]Union[I32]\n"
                           "                element_initializer\n"
                           "                    index: 0\n"
                           "                    reference_expression\n"
                           "                        type: Union[I32]\n"
                           "                        name: b\n"
                           "    function\n"
                           "        name: main\n"
                           "        return_type: I32\n"
                           "        return_statement\n"
                           "            integer_literal\n"
                           "                type: I32\n"
                           "                value: 0\n" );
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
                           "        return_type: [2]%s\n"
                           "        return_statement\n"
                           "            array_literal\n"
                           "                type: [2]%s\n"
                           "                element_initializer\n"
                           "                    index: 0\n"
                           "                    reference_expression\n"
                           "                        type: %s\n"
                           "                        name: b\n"
                           "    function\n"
                           "        name: main\n"
                           "        return_type: I32\n"
                           "        return_statement\n"
                           "            integer_literal\n"
                           "                type: I32\n"
                           "                value: 0\n" ,
                           test.source_paths[0],
                           types[index].name,
                           types[index].name,
                           types[index].name,
                           types[index].name);
    }

    index++;
    return true;
}

static bool initialize_pointer_x_typed_element_as_x_type_compatible_expression(void)
{
    static uint8_t expression_type_index = FIRST_POINTER_TYPE_INDEX;
    static uint8_t element_type_index = FIRST_POINTER_TYPE_INDEX;

    while (true)
    {
        if (expression_type_index > LAST_POINTER_TYPE_INDEX)
            return false;

        bool is_valid_case = compatible_types(types[element_type_index].name, types[expression_type_index].name);
        if (is_valid_case)
        {
            format_test_name("semantics/expressions/array_literal/initialize_%s_typed_element_as_%s_compatible_expression", types[element_type_index].file_friendly_name, types[expression_type_index].file_friendly_name);
            format_source_path(0, "%s.owen", test.name);
            format_command_line_options("%s -print-semantics", test.source_paths[0]);
            
            if (element_type_index == READONLY_TYPE_INDEX)
            {
                format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                      "namespace Abc\n"
                                      "\n"
                                      "function a(%s b) : readonly [2]#I32\n"
                                      "     return { b }\n"
                                      "end\n"
                                      "\n"
                                      "function main() : I32\n"
                                      "    return 0\n"
                                      "end", 
                                      types[expression_type_index].name);
                
                format_expectation(&test.expected_semantics, 
                                   "file\n"
                                   "    path: %s\n"
                                   "    namespace: Abc\n"
                                   "    function\n"
                                   "        name: a\n"
                                   "        formal_parameter\n"
                                   "            type: %s\n"
                                   "            name: b\n"
                                   "        return_type: readonly [2]#I32\n"
                                   "        return_statement\n"
                                   "            array_literal\n"
                                   "                type: readonly [2]#I32\n"
                                   "                element_initializer\n"
                                   "                    index: 0\n"
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
                                   types[expression_type_index].name);
            }
            else if (element_type_index == NOALIAS_TYPE_INDEX)
            {
                format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                      "namespace Abc\n"
                                      "\n"
                                      "function a(%s b) : noalias [2]#I32\n"
                                      "     return { b }\n"
                                      "end\n"
                                      "\n"
                                      "function main() : I32\n"
                                      "    return 0\n"
                                      "end", 
                                      types[expression_type_index].name);
                
                format_expectation(&test.expected_error, 
                                   "%s:4:%u: Only Pointerless formal parameter types can be noalias qualified.\n",
                                   test.source_paths[0], 
                                   18 + strlen(types[expression_type_index].name), 
                                   types[expression_type_index].name);
            }
            else if (element_type_index == READONLY_NOALIAS_TYPE_INDEX)
            {
                format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                      "namespace Abc\n"
                                      "\n"
                                      "function a(%s b) : readonly noalias [2]#I32\n"
                                      "     return { b }\n"
                                      "end\n"
                                      "\n"
                                      "function main() : I32\n"
                                      "    return 0\n"
                                      "end", 
                                      types[expression_type_index].name);

                format_expectation(&test.expected_error, 
                                   "%s:4:%u: Only Pointerless formal parameter types can be noalias qualified.\n",
                                   test.source_paths[0], 
                                   18 + strlen(types[expression_type_index].name), 
                                   types[expression_type_index].name);
            }
            else
            {
                format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                      "namespace Abc\n"
                                      "\n"
                                      "function a(%s b) : [2]%s\n"
                                      "    return { b }\n"
                                      "end\n"
                                      "\n"
                                      "function main() : I32\n"
                                      "    return 0\n"
                                      "end", 
                                      types[expression_type_index].name,
                                      types[element_type_index].name);

                format_expectation(&test.expected_semantics, 
                                   "file\n"
                                   "    path: %s\n"
                                   "    namespace: Abc\n"
                                   "    function\n"
                                   "        name: a\n"
                                   "        formal_parameter\n"
                                   "            type: %s\n"
                                   "            name: b\n"
                                   "        return_type: [2]%s\n"
                                   "        return_statement\n"
                                   "            array_literal\n"
                                   "                type: [2]%s\n"
                                   "                element_initializer\n"
                                   "                    index: 0\n"
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
                                   types[element_type_index].name,
                                   types[expression_type_index].name,
                                   types[expression_type_index].name);
            }
        }

        if (element_type_index > LAST_POINTER_TYPE_INDEX)
        {
            element_type_index = FIRST_POINTER_TYPE_INDEX;
            expression_type_index++;
        }
        else
            element_type_index++;

        if (is_valid_case)
            return true;
    }

    return false;
}

static bool initialize_non_pointer_x_typed_element_as_non_pointer_y_typed_expression(void)
{
    static uint8_t element_type_index = 0;
    static uint8_t expression_type_index = 0;

    while (true)
    {
        if (element_type_index == FIRST_POINTER_TYPE_INDEX)
            return false;

        bool is_valid_case = element_type_index != expression_type_index;
        if (is_valid_case)
        {
            format_test_name("semantics/expressions/array_literal/initialize_%s_typed_element_as_%s_typed_expression", types[element_type_index].file_friendly_name, types[expression_type_index].file_friendly_name);

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
                                  "    [2]%s c = { b }\n"
                                  "end", 
                                  types[expression_type_index].name,
                                  types[element_type_index].name);

            format_command_line_options("%s", test.source_paths[0]);
            format_expectation(&test.expected_error, 
                               "%s:25:%u: %s expected but found %s.\n",
                               test.source_paths[0], 
                               15 + strlen(types[element_type_index].name), 
                               types[element_type_index].name, 
                               types[expression_type_index].name);
        }

        if (expression_type_index == FIRST_POINTER_TYPE_INDEX)
        {
            expression_type_index = 0;
            element_type_index++;
        }
        else
            expression_type_index++;

        if (is_valid_case)
            return true;
    }

    return false;
}

static bool initialize_x_typed_element_as_non_typed_expression(void)
{
    static uint8_t index = 0;
    if (index == LAST_TYPE_INDEX)
        return false;

    format_test_name("semantics/expressions/array_literal/initialize_%s_typed_element_as_non_typed_expression", types[index].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    if (index == READONLY_TYPE_INDEX)
    {
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
                              "function f()\n"
                              "end\n"
                              "\n"
                              "function a()\n"
                              "    readonly [2]#I32 b = { f() }\n"
                              "end");
    }
    else if (index == NOALIAS_TYPE_INDEX)
    {
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
                              "function f()\n"
                              "end\n"
                              "\n"
                              "function a()\n"
                              "    noalias [2]#I32 b = { f() }\n"
                              "end");
    }
    else if (index == READONLY_NOALIAS_TYPE_INDEX)
    {
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
                              "function f()\n"
                              "end\n"
                              "\n"
                              "function a()\n"
                              "    readonly noalias [2]#I32 b = { f() }\n"
                              "end");
    }
    else
    {
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
                              "function f()\n"
                              "end\n"
                              "\n"
                              "function a()\n"
                              "    [2]%s b = { f() }\n"
                              "end", 
                              types[index].name);
    }
    
    format_command_line_options("%s", test.source_paths[0]);

    if (is_noalias_type(types[index].name))
    {
        format_expectation(&test.expected_error, 
                           "%s:28:5: Only Pointerless formal parameter types can be noalias qualified.\n",
                           test.source_paths[0]);
    }
    else
    {
        format_expectation(&test.expected_error, 
                           "%s:28:%u: %s expected but found none.\n",
                           test.source_paths[0],
                           16 + strlen(types[index].name),
                           types[index].name);
    }
    
    index++;
    return true;
}

static bool initialize_x_typed_element_as_tuple_typed_expression(void)
{
    static uint8_t index = 0;
    if (index == LAST_TYPE_INDEX)
        return false;

    format_test_name("semantics/expressions/array_literal/initialize_%s_typed_element_as_tuple_typed_expression", types[index].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    if (index == READONLY_TYPE_INDEX)
    {
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
                              "function f() : I32, I32\n"
                              "    return 0, 0\n"
                              "end\n"
                              "\n"
                              "function a()\n"
                              "    readonly [2]#I32 b = { f() }\n"
                              "end");
        
        format_expectation(&test.expected_error, 
                           "%s:29:%u: %s expected but found I32, I32.\n",
                           test.source_paths[0],
                           16 + strlen(types[index].name),
                           types[index].name);
    }
    else if (index == NOALIAS_TYPE_INDEX)
    {
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
                              "function f() : I32, I32\n"
                              "    return 0, 0\n"
                              "end\n"
                              "\n"
                              "function a()\n"
                              "    noalias [2]#I32 b = { f() }\n"
                              "end");
        
        format_expectation(&test.expected_error, 
                           "%s:29:5: Only Pointerless formal parameter types can be noalias qualified.\n",
                           test.source_paths[0]);
        
    }
    else if (index == READONLY_NOALIAS_TYPE_INDEX)
    {
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
                              "function f() : I32, I32\n"
                              "    return 0, 0\n"
                              "end\n"
                              "\n"
                              "function a()\n"
                              "    readonly noalias [2]#I32 b = { f() }\n"
                              "end");
        
        format_expectation(&test.expected_error, 
                           "%s:29:5: Only Pointerless formal parameter types can be noalias qualified.\n",
                           test.source_paths[0]);
    }
    else
    {
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
                              "function f() : I32, I32\n"
                              "    return 0, 0\n"
                              "end\n"
                              "\n"
                              "function a()\n"
                              "    [2]%s b = { f() }\n"
                              "end",
                              types[index].name);

        format_expectation(&test.expected_error, 
                           "%s:29:%u: %s expected but found I32, I32.\n",
                           test.source_paths[0],
                           16 + strlen(types[index].name),
                           types[index].name);
    }
    
    format_command_line_options("%s", test.source_paths[0]);
    
    index++;
    return true;
}

static bool initialize_pointer_x_typed_element_as_x_type_incompatible_expression(void)
{
    static uint8_t element_type_index = FIRST_POINTER_TYPE_INDEX;
    static uint8_t expression_type_index = FIRST_POINTER_TYPE_INDEX;

    while (true)
    {
        if (element_type_index > LAST_POINTER_TYPE_INDEX)
            return false;

        bool is_valid_case = !compatible_types(types[element_type_index].name, types[expression_type_index].name);
        if (is_valid_case)
        {
            format_test_name("semantics/expressions/array_literal/initialize_%s_typed_element_as_%s_typed_expression", types[element_type_index].file_friendly_name, types[expression_type_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_command_line_options("%s", test.source_paths[0]);

            if (element_type_index == READONLY_TYPE_INDEX)
            {
                format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                      "namespace Abc\n"
                                      "\n"
                                      "function a(%s b)\n"
                                      "    readonly [2]#I32 c = { b }\n"
                                      "end",
                                      types[expression_type_index].name);
            }
            else if (element_type_index == NOALIAS_TYPE_INDEX)
            {
                format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                      "namespace Abc\n"
                                      "\n"
                                      "function a(%s b)\n"
                                      "    noalias [2]#I32 c = { b }\n"
                                      "end",
                                      types[expression_type_index].name);
            }
            else if (element_type_index == READONLY_TYPE_INDEX)
            {
                format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                      "namespace Abc\n"
                                      "\n"
                                      "function a(%s b)\n"
                                      "    readonly noalias [2]#I32 c = { b }\n"
                                      "end",
                                      types[expression_type_index].name);
            }
            else
            {
                format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                      "namespace Abc\n"
                                      "\n"
                                      "function a(%s b)\n"
                                      "    [2]%s c = { b }\n"
                                      "end", 
                                      types[expression_type_index].name,
                                      types[element_type_index].name);
            }
            
            if (is_noalias_type(types[element_type_index].name))
            {
                format_expectation(&test.expected_error, 
                                   "%s:5:5: Only Pointerless formal parameter types can be noalias qualified.\n",
                                   test.source_paths[0]);
            }
            else
            {
                format_expectation(&test.expected_error, 
                                   "%s:5:%u: %s expected but found %s.\n",
                                   test.source_paths[0],
                                   15 + strlen(types[element_type_index].name),
                                   types[element_type_index].name, 
                                   types[expression_type_index].name,
                                   types[element_type_index].name);
            }
            
        }

        if (expression_type_index == LAST_POINTER_TYPE_INDEX)
        {
            expression_type_index = FIRST_POINTER_TYPE_INDEX;
            element_type_index++;
        }
        else
            expression_type_index++;

        if (is_valid_case)
            return true;
    }

    return false;
}

static bool initialize_x_typed_element_as_x_type_compatible_literal(void)
{
    static uint8_t index = 0;

    while (true)
    {
        if (index == LAST_TYPE_INDEX)
            return false;

        bool is_valid_case = types[index].file_friendly_default_value_name != NULL;
        if (is_valid_case)
        {
            format_test_name("semantics/expressions/array_literal/initialize_%s_typed_element_as_%s_type_compatible_literal", types[index].file_friendly_name, types[index].file_friendly_default_value_name, types[index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
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
                                  "function main() : I32\n"
                                  "    [2]%s b = { %s }\n"
                                  "    b = b\n"
                                  "    return 0\n"
                                  "end", 
                                  types[index].name,
                                  types[index].default_value);

            format_command_line_options("%s -print-semantics", test.source_paths[0]);
            if (index == POLYMORPHIC_STRUCTURE_TYPE_INDEX)
            {
                format_expectation(&test.expected_semantics, 
                                   "file\n"
                                   "    path: semantics/expressions/array_literal/initialize_polymorphic_structure_typed_element_as_compound_type_compatible_literal.owen\n"
                                   "    namespace: Abc\n"
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
                                   "        name: main\n"
                                   "        return_type: I32\n"
                                   "        declaration_statement\n"
                                   "            variable\n"
                                   "                type: [2]Structure[I32]\n"
                                   "                name: b\n"
                                   "            array_literal\n"
                                   "                type: [2]Structure[I32]\n"
                                   "                element_initializer\n"
                                   "                    index: 0\n"
                                   "                    compound_literal\n"
                                   "                        type: Structure[I32]\n"
                                   "                        field_initializer\n"
                                   "                            type: I32\n"
                                   "                            name: a\n"
                                   "                            integer_literal\n"
                                   "                                type: I32\n"
                                   "                                value: 0\n"
                                   "        assignment_statement\n"
                                   "            operator: =\n"
                                   "            reference_expression\n"
                                   "                type: [2]Structure[I32]\n"
                                   "                name: b\n"
                                   "            reference_expression\n"
                                   "                type: [2]Structure[I32]\n"
                                   "                name: b\n"
                                   "        return_statement\n"
                                   "            integer_literal\n"
                                   "                type: I32\n"
                                   "                value: 0\n");
            }
            else if (index == POLYMORPHIC_UNION_TYPE_INDEX)
            {
                format_expectation(&test.expected_semantics, 
                                   "file\n"
                                   "    path: semantics/expressions/array_literal/initialize_polymorphic_union_typed_element_as_compound_type_compatible_literal.owen\n"
                                   "    namespace: Abc\n"
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
                                   "        name: main\n"
                                   "        return_type: I32\n"
                                   "        declaration_statement\n"
                                   "            variable\n"
                                   "                type: [2]Union[I32]\n"
                                   "                name: b\n"
                                   "            array_literal\n"
                                   "                type: [2]Union[I32]\n"
                                   "                element_initializer\n"
                                   "                    index: 0\n"
                                   "                    compound_literal\n"
                                   "                        type: Union[I32]\n"
                                   "                        field_initializer\n"
                                   "                            type: I32\n"
                                   "                            name: a\n"
                                   "                            integer_literal\n"
                                   "                                type: I32\n"
                                   "                                value: 0\n"
                                   "        assignment_statement\n"
                                   "            operator: =\n"
                                   "            reference_expression\n"
                                   "                type: [2]Union[I32]\n"
                                   "                name: b\n"
                                   "            reference_expression\n"
                                   "                type: [2]Union[I32]\n"
                                   "                name: b\n"
                                   "        return_statement\n"
                                   "            integer_literal\n"
                                   "                type: I32\n"
                                   "                value: 0\n");
            }
            else if (index == FIXED_ARRAY_TYPE_INDEX)
            {
                format_expectation(&test.expected_semantics, 
                                   "file\n"
                                   "    path: semantics/expressions/array_literal/initialize_fixed_array_typed_element_as_array_type_compatible_literal.owen\n"
                                   "    namespace: Abc\n"
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
                                   "        name: main\n"
                                   "        return_type: I32\n"
                                   "        declaration_statement\n"
                                   "            variable\n"
                                   "                type: [2][4]I32\n"
                                   "                name: b\n"
                                   "            array_literal\n"
                                   "                type: [2][4]I32\n"
                                   "                element_initializer\n"
                                   "                    index: 0\n"
                                   "                    array_literal\n"
                                   "                        type: [4]I32\n"
                                   "                        element_initializer\n"
                                   "                            index: 0\n"
                                   "                            integer_literal\n"
                                   "                                type: I32\n"
                                   "                                value: 0\n"
                                   "        assignment_statement\n"
                                   "            operator: =\n"
                                   "            reference_expression\n"
                                   "                type: [2][4]I32\n"
                                   "                name: b\n"
                                   "            reference_expression\n"
                                   "                type: [2][4]I32\n"
                                   "                name: b\n"
                                   "        return_statement\n"
                                   "            integer_literal\n"
                                   "                type: I32\n"
                                   "                value: 0\n");
            }
            else if (types[index].default_value_name && !strcmp("Compound", types[index].default_value_name))
            {
                format_expectation(&test.expected_semantics, 
                                   "file\n"
                                   "    path: %s\n"
                                   "    namespace: Abc\n"
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
                                   "        name: main\n"
                                   "        return_type: I32\n"
                                   "        declaration_statement\n"
                                   "            variable\n"
                                   "                type: [2]%s\n"
                                   "                name: b\n"
                                   "            array_literal\n"
                                   "                type: [2]%s\n"
                                   "                element_initializer\n"
                                   "                    index: 0\n"
                                   "                    compound_literal\n"
                                   "                        type: %s\n"
                                   "                        field_initializer\n"
                                   "                            type: I32\n"
                                   "                            name: a\n"
                                   "                            integer_literal\n"
                                   "                                type: I32\n"
                                   "                                value: 0\n"
                                   "        assignment_statement\n"
                                   "            operator: =\n"
                                   "            reference_expression\n"
                                   "                type: [2]%s\n"
                                   "                name: b\n"
                                   "            reference_expression\n"
                                   "                type: [2]%s\n"
                                   "                name: b\n"
                                   "        return_statement\n"
                                   "            integer_literal\n"
                                   "                type: I32\n"
                                   "                value: 0\n",
                                   test.source_paths[0],
                                   types[index].name,
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
                                   "        name: main\n"
                                   "        return_type: I32\n"
                                   "        declaration_statement\n"
                                   "            variable\n"
                                   "                type: [2]%s\n"
                                   "                name: b\n"
                                   "            array_literal\n"
                                   "                type: [2]%s\n"
                                   "                element_initializer\n"
                                   "                    index: 0\n"
                                   "                    %s_literal\n"
                                   "                        type: %s\n"
                                   "                        value: 0\n"
                                   "        assignment_statement\n"
                                   "            operator: =\n"
                                   "            reference_expression\n"
                                   "                type: [2]%s\n"
                                   "                name: b\n"
                                   "            reference_expression\n"
                                   "                type: [2]%s\n"
                                   "                name: b\n"
                                   "        return_statement\n"
                                   "            integer_literal\n"
                                   "                type: I32\n"
                                   "                value: 0\n",
                                   test.source_paths[0],
                                   types[index].name,
                                   types[index].name,
                                   types[index].file_friendly_default_value_name,
                                   types[index].name,
                                   types[index].name, 
                                   types[index].name);
            }
        }

        index++;
        if (is_valid_case)
            return true;
    }

    return false;
}

static bool initialize_x_typed_element_as_x_type_incompatible_literal(void)
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
            format_test_name("semantics/expressions/array_literal/initialize_%s_typed_element_as_%s_type_compatible_literal", types[rhs_index].file_friendly_name, types[lhs_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
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
                                  "    [2]%s b = { %s }\n"
                                  "end", 
                                  types[lhs_index].name, types[rhs_index].default_value);

            format_command_line_options("%s", test.source_paths[0]);
            format_expectation(&test.expected_error, 
                               "%s:21:%u: Cannot infer literal as %s.\n", 
                               test.source_paths[0],
                               15 + strlen(types[lhs_index].name),
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
    format_test_name("semantics/expressions/array_literal/monomorphic_literal_in_polymorphic_function");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T](I32 b) : [2]T\n"
                          "    return { b }\n"
                          "end\n"
                          "\n"
                          "function c(I32 d) : [2]I32\n"
                          "    return a[I32](d)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/array_literal/monomorphic_literal_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/array_literal/monomorphic_literal_in_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: I32\n"
                       "            name: b\n"
                       "        return_type_reference: [2]T\n"
                       "        return_statement\n"
                       "            array_literal\n"
                       "                element_initializer\n"
                       "                    reference_expression\n"
                       "                        name: b\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: b\n"
                       "        return_type: [2]I32\n"
                       "        return_statement\n"
                       "            array_literal\n"
                       "                type: [2]I32\n"
                       "                element_initializer\n"
                       "                    index: 0\n"
                       "                    reference_expression\n"
                       "                        type: I32\n"
                       "                        name: b\n"
                       "    function\n"
                       "        name: c\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: d\n"
                       "        return_type: [2]I32\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: [2]I32\n"
                       "                reference_expression\n"
                       "                    type: Function(I32) : [2]I32\n"
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

static void integer_literal_index_is_out_of_bounds_by_one(void)
{
    format_test_name("semantics/expressions/array_literal/integer_literal_index_is_out_of_bounds_by_one");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a() : [2]I32\n"
                          "    return { [2] = 1 }\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/array_literal/integer_literal_index_is_out_of_bounds_by_one.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:15: 2 is out of bounds in [2]I32.\n",
                       test.source_paths[0]);
}

static void integer_literal_index_is_out_of_bounds_by_two(void)
{
    format_test_name("semantics/expressions/array_literal/integer_literal_index_is_out_of_bounds_by_two");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a() : [2]I32\n"
                          "    return { [3] = 1 }\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/array_literal/integer_literal_index_is_out_of_bounds_by_two.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:15: 3 is out of bounds in [2]I32.\n",
                       test.source_paths[0]);
}

static void integer_literal_index_overflows_u32(void)
{
    format_test_name("semantics/expressions/array_literal/integer_literal_index_overflows_u32");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a() : [2]I32\n"
                          "    return { [18446744073709551615] = 1 }\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/array_literal/integer_literal_index_overflows_u32.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:15: 18446744073709551615 overflows U32.\n",
                       test.source_paths[0]);
}

static void enumeration_constant_access_index_is_out_of_bounds_by_one(void)
{
    format_test_name("semantics/expressions/array_literal/enumeration_constant_access_index_is_out_of_bounds_by_one");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : U8\n"
                          "    a = 2\n"
                          "end\n"
                          "\n"
                          "function a() : [2]I32\n"
                          "    return { [A.a] = 1 }\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/array_literal/enumeration_constant_access_index_is_out_of_bounds_by_one.owen");
    format_expectation(&test.expected_error, 
                       "%s:9:15: 2 is out of bounds in [2]I32.\n",
                       test.source_paths[0]);
}

static void enumeration_constant_access_index_is_out_of_bounds_by_two(void)
{
    format_test_name("semantics/expressions/array_literal/enumeration_constant_access_index_is_out_of_bounds_by_two");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : U8\n"
                          "    a = 3\n"
                          "end\n"
                          "\n"
                          "function a() : [2]I32\n"
                          "    return { [A.a] = 1 }\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/array_literal/enumeration_constant_access_index_is_out_of_bounds_by_two.owen");
    format_expectation(&test.expected_error, 
                       "%s:9:15: 3 is out of bounds in [2]I32.\n",
                       test.source_paths[0]);
}

static void enumeration_constant_access_index_overflows_u32(void)
{
    format_test_name("semantics/expressions/array_literal/enumeration_constant_access_index_overflows_u32");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : U64\n"
                          "    a = 18446744073709551615\n"
                          "end\n"
                          "\n"
                          "function a() : [2]I32\n"
                          "    return { [A.a] = 1 }\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/array_literal/enumeration_constant_access_index_overflows_u32.owen");
    format_expectation(&test.expected_error, 
                       "%s:9:15: 18446744073709551615 is out of bounds in [2]I32.\n",
                       test.source_paths[0]);
}

static bool enumeration_constant_access_index_inside_bound_where_x_as_the_underlying_type(void)
{
    static uint8_t type_index = FIRST_INTEGER_TYPE_INDEX;
    if (type_index > LAST_INTEGER_TYPE_INDEX)
        return false;

    format_test_name("semantics/expressions/array_literal/enumeration_constant_access_index_inside_bound_where_%s_as_the_underlying_type", types[type_index].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : %s\n"
                          "    a = 1\n"
                          "end\n"
                          "\n"
                          "function a() : [2]I32\n"
                          "    return { [A.a] = 1 }\n"
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
                       "    enumeration\n"
                       "        name: A\n"
                       "        underlying_type: %s\n"
                       "        is_public: false\n"
                       "        constant\n"
                       "            name: a\n"
                       "            value: 1\n"
                       "    function\n"
                       "        name: a\n"
                       "        return_type: [2]I32\n"
                       "        return_statement\n"
                       "            array_literal\n"
                       "                type: [2]I32\n"
                       "                element_initializer\n"
                       "                    index: 1\n"
                       "                    enumeration_constant_access\n"
                       "                        type: A\n"
                       "                        value: a\n"
                       "                    integer_literal\n"
                       "                        type: I32\n"
                       "                        value: 1\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n",
                       test.source_paths[0],
                       types[type_index].name);

    type_index++;
    return true;
}

static bool enumeration_constant_access_index_overflow_bound_where_x_as_the_underlying_type(void)
{
    static uint8_t type_index = FIRST_INTEGER_TYPE_INDEX;
    if (type_index > LAST_INTEGER_TYPE_INDEX)
        return false;

    format_test_name("semantics/expressions/array_literal/enumeration_constant_access_index_overflow_bound_where_%s_as_the_underlying_type", types[type_index].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : %s\n"
                          "    a = 10\n"
                          "end\n"
                          "\n"
                          "function a() : [2]I32\n"
                          "    return { [A.a] = 1 }\n"
                          "end",
                          types[type_index].name);

    format_command_line_options(test.source_paths[0]);
    format_expectation(&test.expected_error, 
                       "%s:9:15: 10 is out of bounds in [2]I32.\n",
                       test.source_paths[0]);

    type_index++;
    return true;
}

static bool enumeration_constant_access_index_underflow_bound_where_x_as_the_underlying_type(void)
{
    static uint8_t type_index = FIRST_SIGNED_NUMBER_TYPE_INDEX;
    if (type_index > LAST_SIGNED_NUMBER_TYPE_INDEX)
        return false;

    format_test_name("semantics/expressions/array_literal/enumeration_constant_access_index_underflow_bound_where_%s_as_the_underlying_type", types[type_index].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : %s\n"
                          "    a = -1\n"
                          "end\n"
                          "\n"
                          "function a() : [2]I32\n"
                          "    return { [A.a] = 1 }\n"
                          "end",
                          types[type_index].name);

    format_command_line_options(test.source_paths[0]);
    format_expectation(&test.expected_error, 
                       "%s:9:15: -1 is out of bounds in [2]I32.\n",
                       test.source_paths[0]);

    type_index++;
    return true;
}

static void reinitialize_element(void)
{
    format_test_name("semantics/expressions/array_literal/reinitialize_element");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a() : [2]I32\n"
                          "    return { [1] = 1, [1] = 1 }\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/array_literal/reinitialize_element.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:20: Element at index 1 already initialized.\n",
                        test.source_paths[0]);
}

static void explicit_out_of_order_element_initializers_followed_by_implicit_indexed_initializer(void)
{
    format_test_name("semantics/expressions/array_literal/explicit_out_of_order_element_initializers_followed_by_implicit_indexed_initializer");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a() : [4]I32\n"
                          "    return { [2] = 1, [0] = 1, 1 }\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/array_literal/explicit_out_of_order_element_initializers_followed_by_implicit_indexed_initializer.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/array_literal/explicit_out_of_order_element_initializers_followed_by_implicit_indexed_initializer.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        return_type: [4]I32\n"
                       "        return_statement\n"
                       "            array_literal\n"
                       "                type: [4]I32\n"
                       "                element_initializer\n"
                       "                    index: 2\n"
                       "                    integer_literal\n"
                       "                        type: U32\n"
                       "                        value: 2\n"
                       "                    integer_literal\n"
                       "                        type: I32\n"
                       "                        value: 1\n"
                       "                element_initializer\n"
                       "                    index: 0\n"
                       "                    integer_literal\n"
                       "                        type: U32\n"
                       "                        value: 0\n"
                       "                    integer_literal\n"
                       "                        type: I32\n"
                       "                        value: 1\n"
                       "                element_initializer\n"
                       "                    index: 1\n"
                       "                    integer_literal\n"
                       "                        type: I32\n"
                       "                        value: 1\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void implicit_indexed_initializers_inside_bounds(void)
{
    format_test_name("semantics/expressions/array_literal/implicit_indexed_initializers_inside_bounds");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a() : [3]I32\n"
                          "    return { 3, 4, 5 }\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/array_literal/implicit_indexed_initializers_inside_bounds.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/array_literal/implicit_indexed_initializers_inside_bounds.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        return_type: [3]I32\n"
                       "        return_statement\n"
                       "            array_literal\n"
                       "                type: [3]I32\n"
                       "                element_initializer\n"
                       "                    index: 0\n"
                       "                    integer_literal\n"
                       "                        type: I32\n"
                       "                        value: 3\n"
                       "                element_initializer\n"
                       "                    index: 1\n"
                       "                    integer_literal\n"
                       "                        type: I32\n"
                       "                        value: 4\n"
                       "                element_initializer\n"
                       "                    index: 2\n"
                       "                    integer_literal\n"
                       "                        type: I32\n"
                       "                        value: 5\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void implicit_indexed_initializers_outside_bounds(void)
{
    format_test_name("semantics/expressions/array_literal/implicit_indexed_initializers_outside_bounds");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a() : [1]I32\n"
                          "    return { 3, 4 }\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/array_literal/implicit_indexed_initializers_outside_bounds.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:17: 1 is out of bounds in [1]I32.\n",
                       test.source_paths[0]);
}

static void implicit_indexed_initializer_overflows_u32(void)
{
    format_test_name("semantics/expressions/array_literal/implicit_indexed_initializer_overflows_u32");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a() : [4294967295]I32\n"
                          "    return { [4294967294] = 0, 1 }\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/array_literal/implicit_indexed_initializer_overflows_u32.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:32: 4294967295 is out of bounds in [4294967295]I32.\n",
                       test.source_paths[0]);
}

bool semantics_expressions_array_literal(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            if (initialize_non_pointer_x_typed_element_as_non_pointer_x_typed_expression())
                break;
            
            state++;
            
        case 1:
            if (initialize_pointer_x_typed_element_as_x_type_compatible_expression())
                break;
            
            state++;

        case 2:
            if (initialize_non_pointer_x_typed_element_as_non_pointer_y_typed_expression())
                break;

            state++;

        case 3:
            if (initialize_x_typed_element_as_non_typed_expression())
                break;

            state++;

        case 4:
            if (initialize_x_typed_element_as_tuple_typed_expression())
                break;

            state++;

        case 5:
            if (initialize_pointer_x_typed_element_as_x_type_incompatible_expression())
                break;

            state++;

        case 6:
            if (initialize_x_typed_element_as_x_type_compatible_literal())
                break;

            state++;

        case 7:
            if (initialize_x_typed_element_as_x_type_incompatible_literal())
                break;

            state++;

        case 8:
            monomorphic_literal_in_polymorphic_function();
            state++;
            break;

        case 9:
            integer_literal_index_is_out_of_bounds_by_one();
            state++;
            break;

        case 10:
            integer_literal_index_is_out_of_bounds_by_two();
            state++;
            break;

        case 11:
            integer_literal_index_overflows_u32();
            state++;
            break;

        case 12:
            enumeration_constant_access_index_is_out_of_bounds_by_one();
            state++;
            break;

        case 13:
            enumeration_constant_access_index_is_out_of_bounds_by_two();
            state++;
            break;

        case 14:
            enumeration_constant_access_index_overflows_u32();
            state++;
            break;

        case 15:
            if (enumeration_constant_access_index_inside_bound_where_x_as_the_underlying_type())
                break;

            state++;

        case 16:
            if (enumeration_constant_access_index_overflow_bound_where_x_as_the_underlying_type())
                break;

            state++;

        case 17:
            if (enumeration_constant_access_index_underflow_bound_where_x_as_the_underlying_type())
                break;

            state++;

        case 18:
            reinitialize_element();
            state++;
            break;

        case 19:
            explicit_out_of_order_element_initializers_followed_by_implicit_indexed_initializer();
            state++;
            break;

        case 20:
            implicit_indexed_initializers_inside_bounds();
            state++;
            break;

        case 21:
            implicit_indexed_initializers_outside_bounds();
            state++;
            break;

        case 22:
            implicit_indexed_initializer_overflows_u32();
            state++;
            break;

        default:
            return false;
    }

    return true;
}
