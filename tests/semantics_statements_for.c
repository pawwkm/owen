#include "runner.h"

static void bool_typed_condition(void)
{
    format_test_name("semantics/statements/for/bool_typed_condition");

    format_source_path(0, "semantics/statements/for/bool_typed_condition.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(Bool b)\n"
                          "    for I32 c; b; c += 1\n"
                          "    end\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/for/bool_typed_condition.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/statements/for/bool_typed_condition.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_parameter\n"
                       "            type: Bool\n"
                       "            name: b\n"
                       "        loop_statement\n"
                       "            declaration_statement\n"
                       "                variable\n"
                       "                    type: I32\n"
                       "                    name: c\n"
                       "            reference_expression\n"
                       "                type: Bool\n"
                       "                name: b\n"
                       "            assignment_statement\n"
                       "                operator: +=\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: c\n"
                       "                integer_literal\n"
                       "                    type: I32\n"
                       "                    value: 1\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void monomorphic_statement_in_a_polymorphic_function(void)
{
    format_test_name("semantics/statements/for/monomorphic_statement_in_a_polymorphic_function");

    format_source_path(0, "semantics/statements/for/monomorphic_statement_in_a_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T]()\n"
                          "    for I32 b = 0; b != 0; b += 1\n"
                          "        b = 0\n"
                          "    end\n"
                          "end\n"
                          "\n"
                          "function c()\n"
                          "    a[U8]()\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/for/monomorphic_statement_in_a_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/statements/for/monomorphic_statement_in_a_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        loop_statement\n"
                       "            declaration_statement\n"
                       "                variable\n"
                       "                    type_reference: I32\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    value: 0\n"
                       "            binary\n"
                       "                operator: !=\n"
                       "                reference_expression\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    value: 0\n"
                       "            assignment_statement\n"
                       "                operator: +=\n"
                       "                reference_expression\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    value: 1\n"
                       "            assignment_statement\n"
                       "                operator: =\n"
                       "                reference_expression\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    value: 0\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: U8\n"
                       "        loop_statement\n"
                       "            declaration_statement\n"
                       "                variable\n"
                       "                    type: I32\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    type: I32\n"
                       "                    value: 0\n"
                       "            binary\n"
                       "                type: Bool\n"
                       "                operator: !=\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    type: I32\n"
                       "                    value: 0\n"
                       "            assignment_statement\n"
                       "                operator: +=\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    type: I32\n"
                       "                    value: 1\n"
                       "            assignment_statement\n"
                       "                operator: =\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    type: I32\n"
                       "                    value: 0\n"
                       "    function\n"
                       "        name: c\n"
                       "        call_expression\n"
                       "            type: none\n"
                       "            reference_expression\n"
                       "                type: Function()\n"
                       "                name: a\n"
                       "                actual_type_parameter: U8\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static bool non_bool_typed_condition(void)
{
    static uint8_t index = 0;
    while (true)
    {
        if (index == LAST_TYPE_INDEX)
            return false;

        bool is_valid_case = strcmp("Bool", types[index].name);
        if (is_valid_case)
        {
            format_test_name("semantics/statements/for/%s_typed_condition", types[index].file_friendly_name);

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
                                  "    for I32 c = 0; b; c += 1\n"
                                  "    end\n"
                                  "end", 
                                  types[index].name);

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error, 
                               "%s:25:20: Bool expected but found %s.\n",
                               test.source_paths[0], 
                               types[index].name);
        }

        index++;
        if (is_valid_case)
            return true;
    }

    return false;
}

static void polymorphic_statement_in_a_polymorphic_function(void)
{
    format_test_name("semantics/statements/for/polymorphic_statement_in_a_polymorphic_function");

    format_source_path(0, "semantics/statements/for/polymorphic_statement_in_a_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T]()\n"
                          "    for T b = 0; b != 0; b += 1\n"
                          "        b = 0"
                          "    end\n"
                          "end\n"
                          "\n"
                          "function c()\n"
                          "    a[U8]()\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/for/polymorphic_statement_in_a_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/statements/for/polymorphic_statement_in_a_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        loop_statement\n"
                       "            declaration_statement\n"
                       "                variable\n"
                       "                    type_reference: T\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    value: 0\n"
                       "            binary\n"
                       "                operator: !=\n"
                       "                reference_expression\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    value: 0\n"
                       "            assignment_statement\n"
                       "                operator: +=\n"
                       "                reference_expression\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    value: 1\n"
                       "            assignment_statement\n"
                       "                operator: =\n"
                       "                reference_expression\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    value: 0\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: U8\n"
                       "        loop_statement\n"
                       "            declaration_statement\n"
                       "                variable\n"
                       "                    type: U8\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    type: U8\n"
                       "                    value: 0\n"
                       "            binary\n"
                       "                type: Bool\n"
                       "                operator: !=\n"
                       "                reference_expression\n"
                       "                    type: U8\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    type: U8\n"
                       "                    value: 0\n"
                       "            assignment_statement\n"
                       "                operator: +=\n"
                       "                reference_expression\n"
                       "                    type: U8\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    type: U8\n"
                       "                    value: 1\n"
                       "            assignment_statement\n"
                       "                operator: =\n"
                       "                reference_expression\n"
                       "                    type: U8\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    type: U8\n"
                       "                    value: 0\n"
                       "    function\n"
                       "        name: c\n"
                       "        call_expression\n"
                       "            type: none\n"
                       "            reference_expression\n"
                       "                type: Function()\n"
                       "                name: a\n"
                       "                actual_type_parameter: U8\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void redeclare_variable_declared_before_body(void)
{
    format_test_name("semantics/statements/for/redeclare_variable_declared_before_body");

    format_source_path(0, "semantics/statements/for/redeclare_variable_declared_before_body.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    for I32 b = 0; false; b += 1\n"
                          "        I32 b = 42\n"
                          "    end\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/for/redeclare_variable_declared_before_body.owen");

    format_expectation(&test.expected_error, 
                       "%s:6:13: b is already defined.\n",
                       test.source_paths[0]);
}

static void redeclare_variable_declared_in_the_outer_scope(void)
{
    format_test_name("semantics/statements/for/redeclare_variable_declared_in_the_outer_scope");

    format_source_path(0, "semantics/statements/for/redeclare_variable_declared_in_the_outer_scope.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    I32 b\n"
                          "    \n"
                          "    for I32 c; false; c += 1\n"
                          "        I32 b\n"
                          "    end\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/for/redeclare_variable_declared_in_the_outer_scope.owen");

    format_expectation(&test.expected_error, 
                       "%s:8:13: b is already defined.\n",
                       test.source_paths[0]);
}

static void refer_to_variable_declared_before_body_in_the_inner_scope(void)
{
    format_test_name("semantics/statements/for/refer_to_variable_declared_before_body_in_the_inner_scope");

    format_source_path(0, "semantics/statements/for/refer_to_variable_declared_before_body_in_the_inner_scope.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main() : I32\n"
                          "    for I32 b; false; b += 1\n"
                          "        b = 2\n"
                          "    end\n"
                          "\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/for/refer_to_variable_declared_before_body_in_the_inner_scope.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/statements/for/refer_to_variable_declared_before_body_in_the_inner_scope.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        loop_statement\n"
                       "            declaration_statement\n"
                       "                variable\n"
                       "                    type: I32\n"
                       "                    name: b\n"
                       "            boolean_literal\n"
                       "                type: Bool\n"
                       "                value: false\n"
                       "            assignment_statement\n"
                       "                operator: +=\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    type: I32\n"
                       "                    value: 1\n"
                       "            assignment_statement\n"
                       "                operator: =\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    type: I32\n"
                       "                    value: 2\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void refer_to_variable_declared_in_body_in_the_inner_scope(void)
{
    format_test_name("semantics/statements/for/refer_to_variable_declared_in_body_in_the_inner_scope");

    format_source_path(0, "semantics/statements/for/refer_to_variable_declared_in_body_in_the_inner_scope.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main() : I32\n"
                          "    for I32 b; false; b += 1\n"
                          "        I32 c\n"
                          "        c = c\n"
                          "    end\n"
                          "\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/for/refer_to_variable_declared_in_body_in_the_inner_scope.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/statements/for/refer_to_variable_declared_in_body_in_the_inner_scope.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        loop_statement\n"
                       "            declaration_statement\n"
                       "                variable\n"
                       "                    type: I32\n"
                       "                    name: b\n"
                       "            boolean_literal\n"
                       "                type: Bool\n"
                       "                value: false\n"
                       "            assignment_statement\n"
                       "                operator: +=\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    type: I32\n"
                       "                    value: 1\n"
                       "            declaration_statement\n"
                       "                variable\n"
                       "                    type: I32\n"
                       "                    name: c\n"
                       "            assignment_statement\n"
                       "                operator: =\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: c\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: c\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void refer_to_variable_declared_in_the_outer_scope_inside_inner_scope(void)
{
    format_test_name("semantics/statements/for/refer_to_variable_declared_in_the_outer_scope_inside_inner_scope");

    format_source_path(0, "semantics/statements/for/refer_to_variable_declared_in_the_outer_scope_inside_inner_scope.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main() : I32\n"
                          "    I32 a\n"
                          "    \n"
                          "    for I32 b; false; b += 1\n"
                          "        a = a\n"
                          "    end\n"
                          "\n"
                          "    return a\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/for/refer_to_variable_declared_in_the_outer_scope_inside_inner_scope.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/statements/for/refer_to_variable_declared_in_the_outer_scope_inside_inner_scope.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        declaration_statement\n"
                       "            variable\n"
                       "                type: I32\n"
                       "                name: a\n"
                       "        loop_statement\n"
                       "            declaration_statement\n"
                       "                variable\n"
                       "                    type: I32\n"
                       "                    name: b\n"
                       "            boolean_literal\n"
                       "                type: Bool\n"
                       "                value: false\n"
                       "            assignment_statement\n"
                       "                operator: +=\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: b\n"
                       "                integer_literal\n"
                       "                    type: I32\n"
                       "                    value: 1\n"
                       "            assignment_statement\n"
                       "                operator: =\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: a\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: a\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: a\n");
}

static void unused_variable_declared_before_body(void)
{
    format_test_name("semantics/statements/for/unused_variable_declared_before_body");

    format_source_path(0, "semantics/statements/for/unused_variable_declared_before_body.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b)\n"
                          "    for I32 c; false; b += 1\n"
                          "    end\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/for/unused_variable_declared_before_body.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:13: Unused variable.\n",
                       test.source_paths[0]);
}

static void unused_variable_declared_in_body(void)
{
    format_test_name("semantics/statements/for/unused_variable_declared_in_body");

    format_source_path(0, "semantics/statements/for/unused_variable_declared_in_body.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    for I32 b; false; b += 1\n"
                          "        I32 c\n"
                          "    end\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/for/unused_variable_declared_in_body.owen");
    format_expectation(&test.expected_error, 
                       "%s:6:13: Unused variable.\n",
                       test.source_paths[0]);
}

static bool dereferenced_condition(void)
{
    static uint8_t type_index = 0;
    while (true)
    {
        if (type_index > LAST_TYPE_INDEX)
            return false;

        bool is_valid_case = type_index >= FIRST_POINTER_TYPE_INDEX && type_index <= LAST_POINTER_TYPE_INDEX;
        if (is_valid_case)
        {
            format_test_name("semantics/statements/for/dereferenced_%s_typed_condition", types[type_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "function a(%s b)\n"
                                  "    for I32 i = 0; @b == i; i += 1\n"
                                  "    end\n"
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
                               "            type: %s\n"
                               "            name: b\n"
                               "        loop_statement\n"
                               "            declaration_statement\n"
                               "                variable\n"
                               "                    type: I32\n"
                               "                    name: i\n"
                               "                integer_literal\n"
                               "                    type: I32\n"
                               "                    value: 0\n"
                               "            binary\n"
                               "                type: Bool\n"
                               "                operator: ==\n"
                               "                dereference\n"
                               "                    type: I32\n"
                               "                    reference_expression\n"
                               "                        type: %s\n"
                               "                        name: b\n"
                               "                reference_expression\n"
                               "                    type: I32\n"
                               "                    name: i\n"
                               "            assignment_statement\n"
                               "                operator: +=\n"
                               "                reference_expression\n"
                               "                    type: I32\n"
                               "                    name: i\n"
                               "                integer_literal\n"
                               "                    type: I32\n"
                               "                    value: 1\n"
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

        type_index++;
        if (is_valid_case)
            return true;
    }
}

static void field_access_to_pointer_readable_condition(void)
{
    format_test_name("semantics/statements/for/field_access_to_pointer_readable_condition");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    I32 a\n"
                          "end\n"
                          "\n"
                          "function a(#A b)\n"
                          "    for I32 i = 0; b.a == i; i += 1\n"
                          "    end\n"
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
                       "        loop_statement\n"
                       "            declaration_statement\n"
                       "                variable\n"
                       "                    type: I32\n"
                       "                    name: i\n"
                       "                integer_literal\n"
                       "                    type: I32\n"
                       "                    value: 0\n"
                       "            binary\n"
                       "                type: Bool\n"
                       "                operator: ==\n"
                       "                field_access\n"
                       "                    type: I32\n"
                       "                    reference_expression\n"
                       "                        type: #A\n"
                       "                        name: b\n"
                       "                    field: a\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: i\n"
                       "            assignment_statement\n"
                       "                operator: +=\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: i\n"
                       "                integer_literal\n"
                       "                    type: I32\n"
                       "                    value: 1\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n",
                       test.source_paths[0]);
}

static void array_access_to_pointer_readable_condition(void)
{
    format_test_name("semantics/statements/for/array_access_to_pointer_readable_condition");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(#[]I32 b)\n"
                          "    for I32 i = 0; b[0] == i; i += 1\n"
                          "    end\n"
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
                       "        loop_statement\n"
                       "            declaration_statement\n"
                       "                variable\n"
                       "                    type: I32\n"
                       "                    name: i\n"
                       "                integer_literal\n"
                       "                    type: I32\n"
                       "                    value: 0\n"
                       "            binary\n"
                       "                type: Bool\n"
                       "                operator: ==\n"
                       "                array_access\n"
                       "                    type: I32\n"
                       "                    reference_expression\n"
                       "                        type: #[]I32\n"
                       "                        name: b\n"
                       "                    integer_literal\n"
                       "                        type: U32\n"
                       "                        value: 0\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: i\n"
                       "            assignment_statement\n"
                       "                operator: +=\n"
                       "                reference_expression\n"
                       "                    type: I32\n"
                       "                    name: i\n"
                       "                integer_literal\n"
                       "                    type: I32\n"
                       "                    value: 1\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n",
                       test.source_paths[0]);
}

bool semantics_statements_for(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            bool_typed_condition();
            state++;
            break;

        case 1:
            monomorphic_statement_in_a_polymorphic_function();
            state++;
            break;

        case 2:
            if (non_bool_typed_condition())
                break;

            state++;

        case 3:
            polymorphic_statement_in_a_polymorphic_function();
            state++;
            break;

        case 4:
            redeclare_variable_declared_before_body();
            state++;
            break;

        case 5:
            redeclare_variable_declared_in_the_outer_scope();
            state++;
            break;

        case 6:
            refer_to_variable_declared_before_body_in_the_inner_scope();
            state++;
            break;

        case 7:
            refer_to_variable_declared_in_body_in_the_inner_scope();
            state++;
            break;

        case 8:
            refer_to_variable_declared_in_the_outer_scope_inside_inner_scope();
            state++;
            break;

        case 9:
            unused_variable_declared_before_body();
            state++;
            break;

        case 10:
            unused_variable_declared_in_body();
            state++;
            break;

        case 11:
            if (dereferenced_condition())
                break;

            state++;

        case 12:
            field_access_to_pointer_readable_condition();
            state++;
            break;

        case 13:
            array_access_to_pointer_readable_condition();
            state++;
            break;

        default:
            return false;
    }

    return true;
}
