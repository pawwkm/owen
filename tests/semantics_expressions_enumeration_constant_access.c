#include "runner.h"

static void access_constant_of_enumeration(void)
{
    format_test_name("semantics/expressions/enumeration_constant_access/access_constant_of_enumeration");

    format_source_path(0, "semantics/expressions/enumeration_constant_access/access_constant_of_enumeration.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : I32\n"
                          "    a\n"
                          "end\n"
                          "\n"
                          "function do_stuff() : A\n"
                          "    return A.a\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/enumeration_constant_access/access_constant_of_enumeration.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/enumeration_constant_access/access_constant_of_enumeration.owen\n"
                       "    namespace: Abc\n"
                       "    enumeration\n"
                       "        name: A\n"
                       "        underlying_type: I32\n"
                       "        is_public: false\n"
                       "        constant\n"
                       "            name: a\n"
                       "            value: 0\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        return_type: A\n"
                       "        return_statement\n"
                       "            enumeration_constant_access\n"
                       "                type: A\n"
                       "                value: a\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static bool access_constant_of_non_enumeration_type(void)
{
    static uint8_t type_index = 0;
    while (true)
    {
        if (type_index == FIRST_POINTER_TYPE_INDEX)
            return false;

        bool is_valid_case = strcmp("Enumeration", types[type_index].name) && 
                             strcmp("Union[I32]", types[type_index].name) &&
                             strcmp("Structure[I32]", types[type_index].name);
        if (is_valid_case)
        {
            format_test_name("semantics/expressions/enumeration_constant_access/access_constant_of_%s", types[type_index].file_friendly_name);

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "enumeration Enumeration : I32\n"
                                  "    a\n"
                                  "end\n"
                                  "\n"
                                  "union Union\n"
                                  "    I32 a\n"
                                  "end\n"
                                  "\n"
                                  "structure Structure\n"
                                  "    I32 a\n"
                                  "end\n"
                                  "\n"
                                  "function a() : %s\n"
                                  "    return %s.x\n"
                                  "end",
                                  types[type_index].name,
                                  types[type_index].name);

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error, 
                               "%s:17:12: %s is not an enumeration.\n",
                               test.source_paths[0],
                               types[type_index].name);
        }

        type_index++;
        if (is_valid_case)
            return true;
    }
}

static void monomorphic_access_in_polymorphic_function(void)
{
    format_test_name("semantics/expressions/enumeration_constant_access/monomorphic_access_in_polymorphic_function");

    format_source_path(0, "semantics/expressions/enumeration_constant_access/monomorphic_access_in_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : U8\n"
                          "    a\n"
                          "end\n"
                          "\n"
                          "function a[T]() : A\n"
                          "    return A.a\n"
                          "end\n"
                          "\n"
                          "function c() : A\n"
                          "    return a[I32]()\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/enumeration_constant_access/monomorphic_access_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/enumeration_constant_access/monomorphic_access_in_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    enumeration\n"
                       "        name: A\n"
                       "        underlying_type: U8\n"
                       "        is_public: false\n"
                       "        constant\n"
                       "            name: a\n"
                       "            value: 0\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        return_type_reference: A\n"
                       "        return_statement\n"
                       "            enumeration_constant_access\n"
                       "                value: a\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        return_type: A\n"
                       "        return_statement\n"
                       "            enumeration_constant_access\n"
                       "                type: A\n"
                       "                value: a\n"
                       "    function\n"
                       "        name: c\n"
                       "        return_type: A\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: A\n"
                       "                reference_expression\n"
                       "                    type: Function() : A\n"
                       "                    name: a\n"
                       "                    actual_type_parameter: I32\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void polymorphic_access_in_polymorphic_function(void)
{
    format_test_name("semantics/expressions/enumeration_constant_access/polymorphic_access_in_polymorphic_function");

    format_source_path(0, "semantics/expressions/enumeration_constant_access/polymorphic_access_in_polymorphic_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : U8\n"
                          "    a\n"
                          "end\n"
                          "\n"
                          "function a[T]() : T\n"
                          "    return T.a\n"
                          "end\n"
                          "\n"
                          "function c() : A\n"
                          "    return a[A]()\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/enumeration_constant_access/polymorphic_access_in_polymorphic_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/enumeration_constant_access/polymorphic_access_in_polymorphic_function.owen\n"
                       "    namespace: Abc\n"
                       "    enumeration\n"
                       "        name: A\n"
                       "        underlying_type: U8\n"
                       "        is_public: false\n"
                       "        constant\n"
                       "            name: a\n"
                       "            value: 0\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        return_type_reference: T\n"
                       "        return_statement\n"
                       "            enumeration_constant_access\n"
                       "                value: a\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: A\n"
                       "        return_type: A\n"
                       "        return_statement\n"
                       "            enumeration_constant_access\n"
                       "                type: A\n"
                       "                value: a\n"
                       "    function\n"
                       "        name: c\n"
                       "        return_type: A\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: A\n"
                       "                reference_expression\n"
                       "                    type: Function() : A\n"
                       "                    name: a\n"
                       "                    actual_type_parameter: A\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

bool semantics_expressions_enumeration_constant_access(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            access_constant_of_enumeration();
            state++;
            break;

        case 1:
            if (access_constant_of_non_enumeration_type())
                break;
            
            state++;

        case 2: 
            monomorphic_access_in_polymorphic_function();
            state++;
            break;

        case 3:
            polymorphic_access_in_polymorphic_function();
            state++;
            break;

        default:
            return false;
    }

    return true;
}
