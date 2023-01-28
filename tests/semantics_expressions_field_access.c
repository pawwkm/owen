#include "runner.h"

static bool access_field_of_non_compound_type(void)
{
    static uint8_t type_index = 0;
    while (true)
    {
        if (type_index == TYPES_LENGTH)
            return false;

        bool is_valid_case = !types[type_index].default_value_name || strcmp("Compound", types[type_index].default_value_name);
        if (is_valid_case)
        {
            format_test_name("semantics/expressions/field_access/access_field_of_%s", types[type_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "enumeration Enumeration : I32\n"
                                  "    a\n"
                                  "end\n"
                                  "\n"
                                  "function a(%s b) : Enumeration\n"
                                  "    return b.a\n"
                                  "end",
                                  types[type_index].name);

            format_command_line_options(test.source_paths[0]);

            if (type_index >= FIRST_POINTER_TYPE_INDEX && type_index <= LAST_POINTER_TYPE_INDEX)
            {
                format_expectation(&test.expected_error, 
                                   "%s:9:12: %s is not a pointer to a compound or array type.\n",
                                   test.source_paths[0],
                                   types[type_index].name);
            }
            else if (type_index == FIXED_ARRAY_TYPE_INDEX)
            {
                format_expectation(&test.expected_error, 
                                   "%s:9:14: length expected.\n",
                                   test.source_paths[0]);
            }
            else if (type_index == DYNAMIC_ARRAY_TYPE_INDEX)
            {
                format_expectation(&test.expected_error, 
                                   "%s:9:14: length, capacity or elements expected.\n",
                                   test.source_paths[0]);
            }
            else
            {
                format_expectation(&test.expected_error, 
                                   "%s:9:12: %s is not a compound type.\n",
                                   test.source_paths[0],
                                   types[type_index].name);
            }
        }

        type_index++;
        if (is_valid_case)
            return true;
    }
}

static void access_field_of_monomorphic_structure(void)
{
    format_test_name("semantics/expressions/field_access/access_field_of_monomorphic_structure");

    format_source_path(0, "semantics/expressions/field_access/access_field_of_monomorphic_structure.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    I32 b\n"
                          "end\n"
                          "\n"
                          "function do_stuff(A a) : I32\n"
                          "    return a.b\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/field_access/access_field_of_monomorphic_structure.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/field_access/access_field_of_monomorphic_structure.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: b\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_parameter\n"
                       "            type: A\n"
                       "            name: a\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            field_access\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: A\n"
                       "                    name: a\n"
                       "                field: b\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void access_field_of_polymorphic_structure(void)
{
    format_test_name("semantics/expressions/field_access/access_field_of_polymorphic_structure");

    format_source_path(0, "semantics/expressions/field_access/access_field_of_polymorphic_structure.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A[T]\n"
                          "    T b\n"
                          "end\n"
                          "\n"
                          "function do_stuff(A[I32] a) : I32\n"
                          "    return a.b\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/field_access/access_field_of_polymorphic_structure.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/field_access/access_field_of_polymorphic_structure.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        formal_type_parameter: T\n"
                       "        field\n"
                       "            type_reference: T\n"
                       "            name: b\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        actual_type_parameter: I32\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: b\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_parameter\n"
                       "            type: A[I32]\n"
                       "            name: a\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            field_access\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: A[I32]\n"
                       "                    name: a\n"
                       "                field: b\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void access_field_of_monomorphic_union(void)
{
    format_test_name("semantics/expressions/field_access/access_field_of_monomorphic_union");

    format_source_path(0, "semantics/expressions/field_access/access_field_of_monomorphic_union.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "union A\n"
                          "    I32 b\n"
                          "end\n"
                          "\n"
                          "function do_stuff(A a) : I32\n"
                          "    return a.b\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/field_access/access_field_of_monomorphic_union.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/field_access/access_field_of_monomorphic_union.owen\n"
                       "    namespace: Abc\n"
                       "    union\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: b\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_parameter\n"
                       "            type: A\n"
                       "            name: a\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            field_access\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: A\n"
                       "                    name: a\n"
                       "                field: b\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void access_field_of_polymorphic_union(void)
{
    format_test_name("semantics/expressions/field_access/access_field_of_polymorphic_union");

    format_source_path(0, "semantics/expressions/field_access/access_field_of_polymorphic_union.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "union A[T]\n"
                          "    T b\n"
                          "end\n"
                          "\n"
                          "function do_stuff(A[I32] a) : I32\n"
                          "    return a.b\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/field_access/access_field_of_polymorphic_union.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/field_access/access_field_of_polymorphic_union.owen\n"
                       "    namespace: Abc\n"
                       "    union\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        formal_type_parameter: T\n"
                       "        field\n"
                       "            type_reference: T\n"
                       "            name: b\n"
                       "    union\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        actual_type_parameter: I32\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: b\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_parameter\n"
                       "            type: A[I32]\n"
                       "            name: a\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            field_access\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: A[I32]\n"
                       "                    name: a\n"
                       "                field: b\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void access_length_field_of_array_types(void)
{
    format_test_name("semantics/expressions/field_access/access_length_field_of_array_types");

    format_source_path(0, "semantics/expressions/field_access/access_length_field_of_array_types.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a([]I32 b, [2]I32 c) : U32, U32\n"
                          "    return b.length, c.length\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/field_access/access_length_field_of_array_types.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/field_access/access_length_field_of_array_types.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_parameter\n"
                       "            type: []I32\n"
                       "            name: b\n"
                       "        formal_parameter\n"
                       "            type: [2]I32\n"
                       "            name: c\n"
                       "        return_type: U32\n"
                       "        return_type: U32\n"
                       "        return_statement\n"
                       "            field_access\n"
                       "                type: U32\n"
                       "                reference_expression\n"
                       "                    type: []I32\n"
                       "                    name: b\n"
                       "                field: length\n"
                       "            field_access\n"
                       "                type: U32\n"
                       "                reference_expression\n"
                       "                    type: [2]I32\n"
                       "                    name: c\n"
                       "                field: length\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void access_capacity_field_of_dynamic_array_type(void)
{
    format_test_name("semantics/expressions/field_access/access_capacity_field_of_dynamic_array_type");

    format_source_path(0, "semantics/expressions/field_access/access_capacity_field_of_dynamic_array_type.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a([]I32 b) : U32\n"
                          "    return b.capacity\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/field_access/access_capacity_field_of_dynamic_array_type.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/field_access/access_capacity_field_of_dynamic_array_type.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_parameter\n"
                       "            type: []I32\n"
                       "            name: b\n"
                       "        return_type: U32\n"
                       "        return_statement\n"
                       "            field_access\n"
                       "                type: U32\n"
                       "                reference_expression\n"
                       "                    type: []I32\n"
                       "                    name: b\n"
                       "                field: capacity\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void access_elements_field_of_dynamic_array_type(void)
{
    format_test_name("semantics/expressions/field_access/access_elements_field_of_dynamic_array_type");

    format_source_path(0, "semantics/expressions/field_access/access_elements_field_of_dynamic_array_type.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a([]I32 b) : #I32\n"
                          "    return b.elements\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/field_access/access_elements_field_of_dynamic_array_type.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/field_access/access_elements_field_of_dynamic_array_type.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_parameter\n"
                       "            type: []I32\n"
                       "            name: b\n"
                       "        return_type: #I32\n"
                       "        return_statement\n"
                       "            field_access\n"
                       "                type: #I32\n"
                       "                reference_expression\n"
                       "                    type: []I32\n"
                       "                    name: b\n"
                       "                field: elements\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void access_undefined_field_of_dynamic_array_type(void)
{
    format_test_name("semantics/expressions/field_access/access_undefined_field_of_dynamic_array_type");

    format_source_path(0, "semantics/expressions/field_access/access_undefined_field_of_dynamic_array_type.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a([]I32 b) : #I32\n"
                          "    return b.asdf\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/field_access/access_undefined_field_of_dynamic_array_type.owen");
    format_expectation(&test.expected_error, 
                       "semantics/expressions/field_access/access_undefined_field_of_dynamic_array_type.owen:5:14: length, capacity or elements expected.\n");
}

static void access_undefined_field_of_fixed_array_type(void)
{
    format_test_name("semantics/expressions/field_access/access_undefined_field_of_fixed_array_type");

    format_source_path(0, "semantics/expressions/field_access/access_undefined_field_of_fixed_array_type.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a([2]I32 b) : #I32\n"
                          "    return b.asdf\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/field_access/access_undefined_field_of_fixed_array_type.owen");
    format_expectation(&test.expected_error, 
                       "semantics/expressions/field_access/access_undefined_field_of_fixed_array_type.owen:5:14: length expected.\n");
}

static void access_field_of_pointer_to_compound(void)
{
    format_test_name("semantics/expressions/field_access/access_field_of_pointer_to_compound");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    I32 a\n"
                          "end\n"
                          "\n"
                          "function a(#A b) : I32\n"
                          "    return b.a\n"
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
                       "        return_type: I32\n"
                       "        return_statement\n"
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

static void access_field_of_pointer_to_dynamic_array(void)
{
    format_test_name("semantics/expressions/field_access/access_field_of_pointer_to_dynamic_array");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(#[]I32 b) : U32\n"
                          "    return b.length\n"
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
                       "        return_type: U32\n"
                       "        return_statement\n"
                       "            field_access\n"
                       "                type: U32\n"
                       "                reference_expression\n"
                       "                    type: #[]I32\n"
                       "                    name: b\n"
                       "                field: length\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n",
                       test.source_paths[0]);
}

static void access_field_of_readonly_pointer_to_dynamic_array(void)
{
    format_test_name("semantics/expressions/field_access/access_field_readonly_of_pointer_to_dynamic_array");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(readonly #[]I32 b) : U32\n"
                          "    return b.length\n"
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
                       "            type: readonly #[]I32\n"
                       "            name: b\n"
                       "        return_type: U32\n"
                       "        return_statement\n"
                       "            field_access\n"
                       "                type: U32\n"
                       "                reference_expression\n"
                       "                    type: readonly #[]I32\n"
                       "                    name: b\n"
                       "                field: length\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n",
                       test.source_paths[0]);
}

bool semantics_expressions_field_access(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            if (access_field_of_non_compound_type())
                break;

            state++;

        case 1:
            access_field_of_monomorphic_structure();
            state++;
            break;

        case 2:
            access_field_of_polymorphic_structure();
            state++;
            break;

        case 3:
            access_field_of_monomorphic_union();
            state++;
            break;

        case 4:
            access_field_of_polymorphic_union();
            state++;
            break;

        case 5:
            access_length_field_of_array_types();
            state++;
            break;

        case 6:
            access_capacity_field_of_dynamic_array_type();
            state++;
            break;

        case 7:
            access_elements_field_of_dynamic_array_type();
            state++;
            break;

        case 8:
            access_undefined_field_of_dynamic_array_type();
            state++;
            break;

        case 9:
            access_undefined_field_of_fixed_array_type();
            state++;
            break;

        case 10:
            access_field_of_pointer_to_compound();
            state++;
            break;
                    
        case 11:
            access_field_of_pointer_to_dynamic_array();
            state++;
            break;
        
        case 12:
            access_field_of_readonly_pointer_to_dynamic_array();
            state++;
            break;
          
        default:
            return false;
    }

    return true;
}
