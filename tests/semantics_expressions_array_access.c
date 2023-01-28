#include "runner.h"

static bool integer_typed_index_expression(void)
{
    static uint8_t type_index = FIRST_INTEGER_TYPE_INDEX;
    if (type_index > LAST_INTEGER_TYPE_INDEX)
        return false;

    format_test_name("semantics/expressions/array_access/%s_typed_index_expression", types[type_index].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a([]I32 b, %s c) : I32\n"
                          "    return b[c]\n"
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
                       "    function\n"
                       "        name: a\n"
                       "        formal_parameter\n"
                       "            type: []I32\n"
                       "            name: b\n"
                       "        formal_parameter\n"
                       "            type: %s\n"
                       "            name: c\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            array_access\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: []I32\n"
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
                       types[type_index].name,
                       types[type_index].name);

    type_index++;
    return true;
}

static bool enumeration_constant_access_with_x_as_the_underlying_type(void)
{
    static uint8_t type_index = FIRST_INTEGER_TYPE_INDEX;
    if (type_index > LAST_INTEGER_TYPE_INDEX)
        return false;

    format_test_name("semantics/expressions/array_access/enumeration_constant_access_with_%s_as_the_underlying_type", types[type_index].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : %s\n"
                          "end\n"
                          "\n"
                          "function a([]I32 b, A c) : I32\n"
                          "    return b[c]\n"
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
                       "    function\n"
                       "        name: a\n"
                       "        formal_parameter\n"
                       "            type: []I32\n"
                       "            name: b\n"
                       "        formal_parameter\n"
                       "            type: A\n"
                       "            name: c\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            array_access\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: []I32\n"
                       "                    name: b\n"
                       "                reference_expression\n"
                       "                    type: A\n"
                       "                    name: c\n"
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

static void integer_literal_index(void)
{
    format_test_name("semantics/expressions/array_access/integer_literal_index");

    format_source_path(0, "semantics/expressions/array_access/integer_literal_index.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a([]I32 b) : I32\n"
                          "    return b[0]\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/array_access/integer_literal_index.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/array_access/integer_literal_index.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_parameter\n"
                       "            type: []I32\n"
                       "            name: b\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            array_access\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: []I32\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    type: U32\n"
                       "                    value: 0\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void indexing_non_array_type(void)
{
    format_test_name("semantics/expressions/array_access/indexing_non_array_type");

    format_source_path(0, "semantics/expressions/array_access/indexing_non_array_type.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b) : I32\n"
                          "    return b[0]\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/array_access/indexing_non_array_type.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:12: Array type expected but found I32.\n",
                       test.source_paths[0]);
}

static void non_integer_or_enumeration_typed_index_expression(void)
{
    format_test_name("semantics/expressions/array_access/non_integer_or_enumeration_typed_index_expression");

    format_source_path(0, "semantics/expressions/array_access/non_integer_or_enumeration_typed_index_expression.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a([]I32 b, F32 c) : I32\n"
                          "    return b[c]\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/array_access/non_integer_or_enumeration_typed_index_expression.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:14: Integer or enumeration type expected but found F32.\n",
                       test.source_paths[0]);
}

static void monomorphic_array_access_in_polymorphic_function(void)
{
    format_test_name("semantics/expressions/array_access/monomorphic_array_access_in_polymorphic_function");

    format_source_path(0, "semantics/expressions/array_access/monomorphic_array_access_in_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T]([]I32 b) : I32\n"
                          "    return b[0]\n"
                          "end\n"
                          "\n"
                          "function c([]I32 d) : I32\n"
                          "    return a[I32](d)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/array_access/monomorphic_array_access_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/array_access/monomorphic_array_access_in_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: []I32\n"
                       "            name: b\n"
                       "        return_type_reference: I32\n"
                       "        return_statement\n"
                       "            array_access\n"
                       "                reference_expression\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    value: 0\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: []I32\n"
                       "            name: b\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            array_access\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: []I32\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    type: U32\n"
                       "                    value: 0\n"
                       "    function\n"
                       "        name: c\n"
                       "        formal_parameter\n"
                       "            type: []I32\n"
                       "            name: d\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: Function([]I32) : I32\n"
                       "                    name: a\n"
                       "                    actual_type_parameter: I32\n"
                       "                reference_expression\n"
                       "                    type: []I32\n"
                       "                    name: d\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n" );
}

static void polymorphic_array_access_in_polymorphic_function(void)
{
    format_test_name("semantics/expressions/array_access/polymorphic_array_access_in_polymorphic_function");

    format_source_path(0, "semantics/expressions/array_access/polymorphic_array_access_in_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T]([]T b) : T\n"
                          "    return b[0]\n"
                          "end\n"
                          "\n"
                          "function c([]I32 d) : I32\n"
                          "    return a(d)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/array_access/polymorphic_array_access_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/array_access/polymorphic_array_access_in_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: []T\n"
                       "            name: b\n"
                       "        return_type_reference: T\n"
                       "        return_statement\n"
                       "            array_access\n"
                       "                reference_expression\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    value: 0\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: []I32\n"
                       "            name: b\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            array_access\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: []I32\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    type: U32\n"
                       "                    value: 0\n"
                       "    function\n"
                       "        name: c\n"
                       "        formal_parameter\n"
                       "            type: []I32\n"
                       "            name: d\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: Function([]I32) : I32\n"
                       "                    name: a\n"
                       "                reference_expression\n"
                       "                    type: []I32\n"
                       "                    name: d\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void access_array_through_pointer(void)
{
    format_test_name("semantics/expressions/array_access/access_array_through_pointer");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(#[]I32 b) : I32\n"
                          "    return b[0]\n"
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
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            array_access\n"
                       "                type: I32\n"
                       "                reference_expression\n"
                       "                    type: #[]I32\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    type: U32\n"
                       "                    value: 0\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n" ,
                       test.source_paths[0]);
}

bool semantics_expressions_array_access(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            if (integer_typed_index_expression())
                break;

            state++;

        case 1:
            if (enumeration_constant_access_with_x_as_the_underlying_type())
                break;

            state++;

        case 2:
            integer_literal_index();
            state++;
            break;

        case 3:
            indexing_non_array_type();
            state++;
            break;

        case 4:
            non_integer_or_enumeration_typed_index_expression();
            state++;
            break;

        case 5:
            monomorphic_array_access_in_polymorphic_function();
            state++;
            break;

        case 6:
            polymorphic_array_access_in_polymorphic_function();
            state++;
            break;

        case 7:
            access_array_through_pointer();
            state++;
            break;

        default:
            return false;
    }

    return true;
}
