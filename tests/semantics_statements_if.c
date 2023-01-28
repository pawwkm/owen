#include "runner.h"

static bool x_typed_condition(void)
{
    static uint8_t index = 0;
    if (index == LAST_TYPE_INDEX)
        return false;

    format_test_name("semantics/statements/if/%s_typed_condition", types[index].file_friendly_name);

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
                          "    if b\n"
                          "    end\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end", 
                          types[index].name);

    format_command_line_options("%s -print-semantics", test.source_paths[0]);
    if (index != BOOL_TYPE_INDEX)
    {
        format_expectation(&test.expected_error, 
                           "%s:25:8: Bool expected but found %s.\n",
                           test.source_paths[0], 
                           types[index].name);
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
                           "        if_statement\n"
                           "            branch\n"
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
                           types[index].name,
                           types[index].name);
    }

    index++;

    return true;
}

static void monomorphic_statement_in_a_polymorphic_function(void)
{
    format_test_name("semantics/statements/if/monomorphic_statement_in_a_polymorphic_function");

    format_source_path(0, "semantics/statements/if/monomorphic_statement_in_a_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T]()\n"
                          "    if I32 b = 0; b != 0\n"
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

    SET_COMMAND_LINE("semantics/statements/if/monomorphic_statement_in_a_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/statements/if/monomorphic_statement_in_a_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        if_statement\n"
                       "            branch\n"
                       "                declaration_statement\n"
                       "                    variable\n"
                       "                        type_reference: I32\n"
                       "                        name: b\n"
                       "                    integer_literal\n"
                       "                        value: 0\n"
                       "                binary\n"
                       "                    operator: !=\n"
                       "                    reference_expression\n"
                       "                        name: b\n"
                       "                    integer_literal\n"
                       "                        value: 0\n"
                       "                assignment_statement\n"
                       "                    operator: =\n"
                       "                    reference_expression\n"
                       "                        name: b\n"
                       "                    integer_literal\n"
                       "                        value: 0\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: U8\n"
                       "        if_statement\n"
                       "            branch\n"
                       "                declaration_statement\n"
                       "                    variable\n"
                       "                        type: I32\n"
                       "                        name: b\n"
                       "                    integer_literal\n"
                       "                        type: I32\n"
                       "                        value: 0\n"
                       "                binary\n"
                       "                    type: Bool\n"
                       "                    operator: !=\n"
                       "                    reference_expression\n"
                       "                        type: I32\n"
                       "                        name: b\n"
                       "                    integer_literal\n"
                       "                        type: I32\n"
                       "                        value: 0\n"
                       "                assignment_statement\n"
                       "                    operator: =\n"
                       "                    reference_expression\n"
                       "                        type: I32\n"
                       "                        name: b\n"
                       "                    integer_literal\n"
                       "                        type: I32\n"
                       "                        value: 0\n"
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

static void polymorphic_statement_in_a_polymorphic_function(void)
{
    format_test_name("semantics/statements/if/polymorphic_statement_in_a_polymorphic_function");

    format_source_path(0, "semantics/statements/if/polymorphic_statement_in_a_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T]()\n"
                          "    if T b = 0; b != 0\n"
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

    SET_COMMAND_LINE("semantics/statements/if/polymorphic_statement_in_a_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/statements/if/polymorphic_statement_in_a_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        if_statement\n"
                       "            branch\n"
                       "                declaration_statement\n"
                       "                    variable\n"
                       "                        type_reference: T\n"
                       "                        name: b\n"
                       "                    integer_literal\n"
                       "                        value: 0\n"
                       "                binary\n"
                       "                    operator: !=\n"
                       "                    reference_expression\n"
                       "                        name: b\n"
                       "                    integer_literal\n"
                       "                        value: 0\n"
                       "                assignment_statement\n"
                       "                    operator: =\n"
                       "                    reference_expression\n"
                       "                        name: b\n"
                       "                    integer_literal\n"
                       "                        value: 0\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: U8\n"
                       "        if_statement\n"
                       "            branch\n"
                       "                declaration_statement\n"
                       "                    variable\n"
                       "                        type: U8\n"
                       "                        name: b\n"
                       "                    integer_literal\n"
                       "                        type: U8\n"
                       "                        value: 0\n"
                       "                binary\n"
                       "                    type: Bool\n"
                       "                    operator: !=\n"
                       "                    reference_expression\n"
                       "                        type: U8\n"
                       "                        name: b\n"
                       "                    integer_literal\n"
                       "                        type: U8\n"
                       "                        value: 0\n"
                       "                assignment_statement\n"
                       "                    operator: =\n"
                       "                    reference_expression\n"
                       "                        type: U8\n"
                       "                        name: b\n"
                       "                    integer_literal\n"
                       "                        type: U8\n"
                       "                        value: 0\n"
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
    format_test_name("semantics/statements/if/redeclare_variable_declared_before_body");

    format_source_path(0, "semantics/statements/if/redeclare_variable_declared_before_body.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    if I32 b = 0; false\n"
                          "        I32 b = 42\n"
                          "    end\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/if/redeclare_variable_declared_before_body.owen");

    format_expectation(&test.expected_error, 
                       "%s:6:13: b is already defined.\n",
                       test.source_paths[0]);
}

static void redeclare_variable_declared_in_the_outer_scope(void)
{
    format_test_name("semantics/statements/if/redeclare_variable_declared_in_the_outer_scope");

    format_source_path(0, "semantics/statements/if/redeclare_variable_declared_in_the_outer_scope.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    I32 b = 0\n"
                          "    \n"
                          "    if false\n"
                          "        I32 b = 42\n"
                          "    end\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/if/redeclare_variable_declared_in_the_outer_scope.owen");

    format_expectation(&test.expected_error, 
                       "%s:8:13: b is already defined.\n",
                       test.source_paths[0]);
}

static void refer_to_variable_declared_before_body_in_the_inner_scope(void)
{
    format_test_name("semantics/statements/if/refer_to_variable_declared_before_body_in_the_inner_scope");

    format_source_path(0, "semantics/statements/if/refer_to_variable_declared_before_body_in_the_inner_scope.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main() : I32\n"
                          "    if I32 b = 0; false\n"
                          "        b = b\n"
                          "    end\n"
                          "\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/if/refer_to_variable_declared_before_body_in_the_inner_scope.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/statements/if/refer_to_variable_declared_before_body_in_the_inner_scope.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        if_statement\n"
                       "            branch\n"
                       "                declaration_statement\n"
                       "                    variable\n"
                       "                        type: I32\n"
                       "                        name: b\n"
                       "                    integer_literal\n"
                       "                        type: I32\n"
                       "                        value: 0\n"
                       "                boolean_literal\n"
                       "                    type: Bool\n"
                       "                    value: false\n"
                       "                assignment_statement\n"
                       "                    operator: =\n"
                       "                    reference_expression\n"
                       "                        type: I32\n"
                       "                        name: b\n"
                       "                    reference_expression\n"
                       "                        type: I32\n"
                       "                        name: b\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void refer_to_variable_declared_in_body_in_the_inner_scope(void)
{
    format_test_name("semantics/statements/if/refer_to_variable_declared_in_body_in_the_inner_scope");

    format_source_path(0, "semantics/statements/if/refer_to_variable_declared_in_body_in_the_inner_scope.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main() : I32\n"
                          "    if false\n"
                          "        I32 b = 0\n"
                          "        b = b\n"
                          "    end\n"
                          "\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/if/refer_to_variable_declared_in_body_in_the_inner_scope.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/statements/if/refer_to_variable_declared_in_body_in_the_inner_scope.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        if_statement\n"
                       "            branch\n"
                       "                boolean_literal\n"
                       "                    type: Bool\n"
                       "                    value: false\n"
                       "                declaration_statement\n"
                       "                    variable\n"
                       "                        type: I32\n"
                       "                        name: b\n"
                       "                    integer_literal\n"
                       "                        type: I32\n"
                       "                        value: 0\n"
                       "                assignment_statement\n"
                       "                    operator: =\n"
                       "                    reference_expression\n"
                       "                        type: I32\n"
                       "                        name: b\n"
                       "                    reference_expression\n"
                       "                        type: I32\n"
                       "                        name: b\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void refer_to_variable_declared_in_the_outer_scope_inside_inner_scope(void)
{
    format_test_name("semantics/statements/if/refer_to_variable_declared_in_the_outer_scope_inside_inner_scope");

    format_source_path(0, "semantics/statements/if/refer_to_variable_declared_in_the_outer_scope_inside_inner_scope.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main() : I32\n"
                          "    I32 a = 0\n"
                          "    \n"
                          "    if false\n"
                          "        a = a\n"
                          "    end\n"
                          "\n"
                          "    return a\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/if/refer_to_variable_declared_in_the_outer_scope_inside_inner_scope.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/statements/if/refer_to_variable_declared_in_the_outer_scope_inside_inner_scope.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        declaration_statement\n"
                       "            variable\n"
                       "                type: I32\n"
                       "                name: a\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n"
                       "        if_statement\n"
                       "            branch\n"
                       "                boolean_literal\n"
                       "                    type: Bool\n"
                       "                    value: false\n"
                       "                assignment_statement\n"
                       "                    operator: =\n"
                       "                    reference_expression\n"
                       "                        type: I32\n"
                       "                        name: a\n"
                       "                    reference_expression\n"
                       "                        type: I32\n"
                       "                        name: a\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: a\n");
}

static void unused_variable_declared_before_body(void)
{
    format_test_name("semantics/statements/if/unused_variable_declared_before_body");

    format_source_path(0, "semantics/statements/if/unused_variable_declared_before_body.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    if I32 b = 0; false\n"
                          "    end\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/if/unused_variable_declared_before_body.owen");
    format_expectation(&test.expected_error, 
                       "%s:5:12: Unused variable.\n",
                       test.source_paths[0]);
}

static void unused_variable_declared_in_body(void)
{
    format_test_name("semantics/statements/if/unused_variable_declared_in_body");

    format_source_path(0, "semantics/statements/if/unused_variable_declared_in_body.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "    if false\n"
                          "        I32 b = 0\n"
                          "    end\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/if/unused_variable_declared_in_body.owen");
    format_expectation(&test.expected_error, 
                       "%s:6:13: Unused variable.\n",
                       test.source_paths[0]);
}

static void refer_to_variable_declared_in_earlier_branch(void)
{
    format_test_name("semantics/statements/if/refer_to_variable_declared_in_earlier_branch");

    format_source_path(0, "semantics/statements/if/refer_to_variable_declared_in_earlier_branch.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main() : I32\n"
                          "    if I32 b = 4; b == 1\n"
                          "        return 1\n"
                          "    elif b == 2\n"
                          "        return 2\n"
                          "    elif b == 3\n"
                          "        return 3\n"
                          "    else\n"
                          "        return b\n"
                          "    end\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/if/refer_to_variable_declared_in_earlier_branch.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/statements/if/refer_to_variable_declared_in_earlier_branch.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        if_statement\n"
                       "            branch\n"
                       "                declaration_statement\n"
                       "                    variable\n"
                       "                        type: I32\n"
                       "                        name: b\n"
                       "                    integer_literal\n"
                       "                        type: I32\n"
                       "                        value: 4\n"
                       "                binary\n"
                       "                    type: Bool\n"
                       "                    operator: ==\n"
                       "                    reference_expression\n"
                       "                        type: I32\n"
                       "                        name: b\n"
                       "                    integer_literal\n"
                       "                        type: I32\n"
                       "                        value: 1\n"
                       "                return_statement\n"
                       "                    integer_literal\n"
                       "                        type: I32\n"
                       "                        value: 1\n"
                       "            branch\n"
                       "                binary\n"
                       "                    type: Bool\n"
                       "                    operator: ==\n"
                       "                    reference_expression\n"
                       "                        type: I32\n"
                       "                        name: b\n"
                       "                    integer_literal\n"
                       "                        type: I32\n"
                       "                        value: 2\n"
                       "                return_statement\n"
                       "                    integer_literal\n"
                       "                        type: I32\n"
                       "                        value: 2\n"
                       "            branch\n"
                       "                binary\n"
                       "                    type: Bool\n"
                       "                    operator: ==\n"
                       "                    reference_expression\n"
                       "                        type: I32\n"
                       "                        name: b\n"
                       "                    integer_literal\n"
                       "                        type: I32\n"
                       "                        value: 3\n"
                       "                return_statement\n"
                       "                    integer_literal\n"
                       "                        type: I32\n"
                       "                        value: 3\n"
                       "            branch\n"
                       "                return_statement\n"
                       "                    reference_expression\n"
                       "                        type: I32\n"
                       "                        name: b\n");
}

static void refer_to_variable_declared_in_branch_body(void)
{
    format_test_name("semantics/statements/if/refer_to_variable_declared_in_branch_body");

    format_source_path(0, "semantics/statements/if/refer_to_variable_declared_in_branch_body.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a() : I32\n"
                          "    if false\n"
                          "        I32 b = 4"
                          "        return b\n"
                          "    else\n"
                          "        return b\n"
                          "    end\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/if/refer_to_variable_declared_in_branch_body.owen");

    format_expectation(&test.expected_error, 
                       "%s:8:16: b is undefined.\n",
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
            format_test_name("semantics/statements/if/dereferenced_%s_typed_condition", types[type_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "function a(%s b)\n"
                                  "    if @b == 1\n"
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
                               "        if_statement\n"
                               "            branch\n"
                               "                binary\n"
                               "                    type: Bool\n"
                               "                    operator: ==\n"
                               "                    dereference\n"
                               "                        type: I32\n"
                               "                        reference_expression\n"
                               "                            type: %s\n"
                               "                            name: b\n"
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
    format_test_name("semantics/statements/if/field_access_to_pointer_readable_condition");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    I32 a\n"
                          "end\n"
                          "\n"
                          "function a(#A b)\n"
                          "    if b.a == 1\n"
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
                       "        if_statement\n"
                       "            branch\n"
                       "                binary\n"
                       "                    type: Bool\n"
                       "                    operator: ==\n"
                       "                    field_access\n"
                       "                        type: I32\n"
                       "                        reference_expression\n"
                       "                            type: #A\n"
                       "                            name: b\n"
                       "                        field: a\n"
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
                       test.source_paths[0]);
}

static void array_access_to_pointer_readable_condition(void)
{
    format_test_name("semantics/statements/if/array_access_to_pointer_readable_condition");

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(#[]I32 b)\n"
                          "    if b[0] == 1\n"
                          "    end\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    format_command_line_options("%s -print-semantics", test.source_paths[0]);
    format_expectation(&test.expected_semantics,
                       "file\n"
                       "    path: semantics/statements/if/array_access_to_pointer_readable_condition.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_parameter\n"
                       "            type: #[]I32\n"
                       "            name: b\n"
                       "        if_statement\n"
                       "            branch\n"
                       "                binary\n"
                       "                    type: Bool\n"
                       "                    operator: ==\n"
                       "                    array_access\n"
                       "                        type: I32\n"
                       "                        reference_expression\n"
                       "                            type: #[]I32\n"
                       "                            name: b\n"
                       "                        integer_literal\n"
                       "                            type: U32\n"
                       "                            value: 0\n"
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
                       test.source_paths[0]);
}

static void all_branches_terminate_but_has_no_else_branch(void)
{
    format_test_name("semantics/statements/if/all_branches_terminate_but_has_no_else_branch");

    format_source_path(0, "semantics/statements/if/all_branches_terminate_but_has_no_else_branch.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a() : I32\n"
                          "    if false\n"
                          "        return 1\n"
                          "    elif false\n"
                          "        return 2\n"
                          "    end\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/if/all_branches_terminate_but_has_no_else_branch.owen");

    format_expectation(&test.expected_error, 
                       "%s:4:10: Missing terminating statement.\n",
                       test.source_paths[0]);
}

static void not_all_branches_terminate_but_has_else_branch(void)
{
    format_test_name("semantics/statements/if/not_all_branches_terminate_but_has_else_branch");

    format_source_path(0, "semantics/statements/if/not_all_branches_terminate_but_has_else_branch.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a() : I32\n"
                          "    if false\n"
                          "    else\n"
                          "        return 2\n"
                          "    end\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/if/not_all_branches_terminate_but_has_else_branch.owen");

    format_expectation(&test.expected_error, 
                       "%s:4:10: Missing terminating statement.\n",
                       test.source_paths[0]);
}

static void non_terminating_if_branch(void)
{
    format_test_name("semantics/statements/if/non_terminating_if_branch");

    format_source_path(0, "semantics/statements/if/non_terminating_if_branch.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a() : I32\n"
                          "    if false\n"
                          "    end\n"
                          "end");

    SET_COMMAND_LINE("semantics/statements/if/non_terminating_if_branch.owen");

    format_expectation(&test.expected_error, 
                       "%s:4:10: Missing terminating statement.\n",
                       test.source_paths[0]);
}

bool semantics_statements_if(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            if (x_typed_condition())
                break;

            state++;

        case 1:
            monomorphic_statement_in_a_polymorphic_function();
            state++;
            break;

        case 2:
            polymorphic_statement_in_a_polymorphic_function();
            state++;
            break;

        case 3:
            redeclare_variable_declared_before_body();
            state++;
            break;

        case 4:
            redeclare_variable_declared_in_the_outer_scope();
            state++;
            break;

        case 5:
            refer_to_variable_declared_before_body_in_the_inner_scope();
            state++;
            break;

        case 6:
            refer_to_variable_declared_in_body_in_the_inner_scope();
            state++;
            break;

        case 7:
            refer_to_variable_declared_in_the_outer_scope_inside_inner_scope();
            state++;
            break;

        case 8:
            unused_variable_declared_before_body();
            state++;
            break;

        case 9:
            unused_variable_declared_in_body();
            state++;
            break;

        case 10:
            refer_to_variable_declared_in_earlier_branch();
            state++;
            break;

        case 11:
            refer_to_variable_declared_in_branch_body();
            state++;
            break;

        case 12:
            if (dereferenced_condition())
                break;

            state++;

        case 13:
            field_access_to_pointer_readable_condition();
            state++;
            break;
            
        case 14:
            array_access_to_pointer_readable_condition();
            state++;
            break;

        case 15:
            all_branches_terminate_but_has_no_else_branch();
            state++;
            break;

        case 16:
            not_all_branches_terminate_but_has_else_branch();
            state++;
            break;

        case 17:
            non_terminating_if_branch();
            state++;
            break;

        default:
            return false;
    }

    return true;
}
