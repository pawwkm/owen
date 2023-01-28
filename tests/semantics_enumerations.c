#include "runner.h"

static void enumerations_with_the_same_name_in_different_namespaces(void)
{
    format_test_name("semantics/enumerations/enumerations_with_the_same_name_in_different_namespaces");

    format_source_path(0, "semantics/enumerations/enumerations_with_the_same_name_in_different_namespaces-1.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : I32\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    format_source_path(1, "semantics/enumerations/enumerations_with_the_same_name_in_different_namespaces-2.owen");
    format_source_file(1, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Def\n"
                          "\n"
                          "enumeration A : I32\n"
                          "end");

    SET_COMMAND_LINE("semantics/enumerations/enumerations_with_the_same_name_in_different_namespaces-1.owen "
                     "semantics/enumerations/enumerations_with_the_same_name_in_different_namespaces-2.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/enumerations/enumerations_with_the_same_name_in_different_namespaces-1.owen\n"
                       "    namespace: Abc\n"
                       "    enumeration\n"
                       "        name: A\n"
                       "        underlying_type: I32\n"
                       "        is_public: false\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n"
                       "file\n"
                       "    path: semantics/enumerations/enumerations_with_the_same_name_in_different_namespaces-2.owen\n"
                       "    namespace: Def\n"
                       "    enumeration\n"
                       "        name: A\n"
                       "        underlying_type: I32\n"
                       "        is_public: false\n");
}

static void enumerations_with_the_same_name_in_the_same_file(void)
{
    format_test_name("semantics/enumerations/enumerations_with_the_same_name_in_the_same_file");

    format_source_path(0, "semantics/enumerations/enumerations_with_the_same_name_in_the_same_file.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : I32\n"
                          "end\n"
                          "\n"
                          "enumeration A : I32\n"
                          "end");

    SET_COMMAND_LINE("semantics/enumerations/enumerations_with_the_same_name_in_the_same_file.owen");
    format_expectation(&test.expected_error, 
                       "semantics/enumerations/enumerations_with_the_same_name_in_the_same_file.owen:4:13: Matching declarations for A in scope.\n"
                       "Declarations:\n"
                       "    semantics/enumerations/enumerations_with_the_same_name_in_the_same_file.owen:4:13: A\n"
                       "    semantics/enumerations/enumerations_with_the_same_name_in_the_same_file.owen:7:13: A\n");
}

static void enumerations_with_the_same_name_in_the_same_namespaces(void)
{
    format_test_name("semantics/enumerations/enumerations_with_the_same_name_in_the_same_namespaces");

    format_source_path(0, "semantics/enumerations/enumerations_with_the_same_name_in_the_same_namespaces-1.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : I32\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    format_source_path(1, "semantics/enumerations/enumerations_with_the_same_name_in_the_same_namespaces-2.owen");
    format_source_file(1, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Def\n"
                          "\n"
                          "enumeration A : I32\n"
                          "end");

    SET_COMMAND_LINE("semantics/enumerations/enumerations_with_the_same_name_in_the_same_namespaces-1.owen "
                     "semantics/enumerations/enumerations_with_the_same_name_in_the_same_namespaces-2.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/enumerations/enumerations_with_the_same_name_in_the_same_namespaces-1.owen\n"
                       "    namespace: Abc\n"
                       "    enumeration\n"
                       "        name: A\n"
                       "        underlying_type: I32\n"
                       "        is_public: false\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n"
                       "file\n"
                       "    path: semantics/enumerations/enumerations_with_the_same_name_in_the_same_namespaces-2.owen\n"
                       "    namespace: Def\n"
                       "    enumeration\n"
                       "        name: A\n"
                       "        underlying_type: I32\n"
                       "        is_public: false\n");
}

static void enumerations_with_the_same_name_in_the_same_namespaces_where_one_is_public(void)
{
    format_test_name("semantics/enumerations/enumerations_with_the_same_name_in_the_same_namespaces_where_one_is_public");

    format_source_path(0, "semantics/enumerations/enumerations_with_the_same_name_in_the_same_namespaces_where_one_is_public-1.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "public enumeration A : I32\n"
                          "end");

    format_source_path(1, "semantics/enumerations/enumerations_with_the_same_name_in_the_same_namespaces_where_one_is_public-2.owen");
    format_source_file(1, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : I32\n"
                          "end");

    SET_COMMAND_LINE("semantics/enumerations/enumerations_with_the_same_name_in_the_same_namespaces_where_one_is_public-1.owen "
                     "semantics/enumerations/enumerations_with_the_same_name_in_the_same_namespaces_where_one_is_public-2.owen");

    format_expectation(&test.expected_error, 
                       "semantics/enumerations/enumerations_with_the_same_name_in_the_same_namespaces_where_one_is_public-2.owen:4:13: Matching declarations for A in scope.\n"
                       "Declarations:\n"
                       "    semantics/enumerations/enumerations_with_the_same_name_in_the_same_namespaces_where_one_is_public-2.owen:4:13: A\n"
                       "    semantics/enumerations/enumerations_with_the_same_name_in_the_same_namespaces_where_one_is_public-1.owen:4:20: A\n");
}

static bool x_typed_constant_overflowing_the_underlying_type(void)
{
    static uint8_t index = FIRST_INTEGER_TYPE_INDEX;
    if (index > LAST_INTEGER_TYPE_INDEX)
        return false;

    format_test_name("semantics/enumerations/%s_constant_overflowing_the_underlying_type", types[index].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : %s\n"
                          "    a = %s\n"
                          "end",
                          types[index].name,
                          types[index].overflow_value);

    format_command_line_options("%s", test.source_paths[0]);
    format_expectation(&test.expected_error, 
                       "%s:5:9: %s overflows %s.\n",
                       test.source_paths[0], types[index].overflow_value, types[index].name);

    index++;
    return true;
}

static bool x_typed_constant_underflowing_the_underlying_type(void)
{
    static uint8_t index = FIRST_SIGNED_NUMBER_TYPE_INDEX;
    if (index > LAST_SIGNED_NUMBER_TYPE_INDEX)
        return false;

    format_test_name("semantics/enumerations/%s_constant_underflowing_the_underlying_type", types[index].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : %s\n"
                          "    a = %s\n"
                          "end",
                          types[index].name,
                          types[index].underflow_value);

    format_command_line_options("%s", test.source_paths[0]);
    format_expectation(&test.expected_error, 
                       "%s:5:9: %s underflows %s.\n",
                       test.source_paths[0], types[index].underflow_value, types[index].name);

    index++;
    return true;
}

static bool x_typed_explicitly_positive_valued_constant_followed_by_implicitly_valued_constant(void)
{
    static uint8_t index = FIRST_INTEGER_TYPE_INDEX;
    if (index > LAST_INTEGER_TYPE_INDEX)
        return false;

    format_test_name("semantics/enumerations/%s_typed_explicitly_positive_valued_constant_followed_by_implicitly_valued_constant", types[index].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : %s\n"
                          "    a = 4\n"
                          "    b\n"
                          "end"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end",
                          types[index].name);

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
                       "            value: 4\n"
                       "        constant\n"
                       "            name: b\n"
                       "            value: 5\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n",
                       test.source_paths[0],
                       types[index].name);

    index++;
    return true;
}

static bool x_typed_explicitly_negative_valued_constant_followed_by_implicitly_valued_constant(void)
{
    static uint8_t index = FIRST_SIGNED_NUMBER_TYPE_INDEX;
    if (index > LAST_SIGNED_NUMBER_TYPE_INDEX)
        return false;

    format_test_name("semantics/enumerations/%s_typed_explicitly_negative_valued_constant_followed_by_implicitly_valued_constant", types[index].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : %s\n"
                          "    a = -4\n"
                          "    b\n"
                          "end"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end",
                          types[index].name);

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
                       "            value: -4\n"
                       "        constant\n"
                       "            name: b\n"
                       "            value: -3\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n",
                       test.source_paths[0],
                       types[index].name);

    index++;
    return true;
}

static bool x_typed_implicitly_valued_constant_between_explicitly_valued_constants(void)
{
    static uint8_t index = FIRST_INTEGER_TYPE_INDEX;
    if (index > LAST_INTEGER_TYPE_INDEX)
        return false;

    format_test_name("semantics/enumerations/%s_typed_implicitly_valued_constant_between_explicitly_valued_constants", types[index].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : %s\n"
                          "    a = 2\n"
                          "    b\n"
                          "    c = 6\n"
                          "end"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end",
                          types[index].name);

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
                       "            value: 2\n"
                       "        constant\n"
                       "            name: b\n"
                       "            value: 3\n"
                       "        constant\n"
                       "            name: c\n"
                       "            value: 6\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n",
                       test.source_paths[0],
                       types[index].name);

    index++;
    return true;
}

static bool x_typed_implicitly_valued_constants_only(void)
{
    static uint8_t index = FIRST_INTEGER_TYPE_INDEX;
    if (index > LAST_INTEGER_TYPE_INDEX)
        return false;

    format_test_name("semantics/enumerations/%s_typed_implicitly_valued_constants_only", types[index].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : %s\n"
                          "    a\n"
                          "    b\n"
                          "    c\n"
                          "end"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end",
                          types[index].name);

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
                       "            value: 0\n"
                       "        constant\n"
                       "            name: b\n"
                       "            value: 1\n"
                       "        constant\n"
                       "            name: c\n"
                       "            value: 2\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n",
                       test.source_paths[0],
                       types[index].name);

    index++;
    return true;
}

static bool x_typed_implicitly_valued_constants_overflow_underlying_type(void)
{
    static uint8_t index = FIRST_INTEGER_TYPE_INDEX;
    if (index > LAST_INTEGER_TYPE_INDEX)
        return false;

    format_test_name("semantics/enumerations/%s_typed_implicitly_valued_constants_overflow_underlying_type", types[index].file_friendly_name);

    format_source_path(0, "%s.owen", test.name);
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : %s\n"
                          "    a = %s\n"
                          "    b\n"
                          "end",
                          types[index].name,
                          types[index].max_value);

    format_command_line_options("%s", test.source_paths[0]);
    format_expectation(&test.expected_error,
                       "%s:6:5: b overflows %s.\n",
                       test.source_paths[0], types[index].name);

    index++;
    return true;
}

bool semantics_enumerations(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            enumerations_with_the_same_name_in_different_namespaces();
            state++;
            break;

        case 1:
            enumerations_with_the_same_name_in_the_same_file();
            state++;
            break;

        case 2:
            enumerations_with_the_same_name_in_the_same_namespaces();
            state++;
            break;

        case 3:
            enumerations_with_the_same_name_in_the_same_namespaces_where_one_is_public();
            state++;
            break;

        case 4:
            if (x_typed_constant_overflowing_the_underlying_type())
                break;

            state++;
            
        case 5:
            if (x_typed_constant_underflowing_the_underlying_type())
                break;

            state++;

        case 6:
            if (x_typed_explicitly_positive_valued_constant_followed_by_implicitly_valued_constant())
                break;

            state++;

        case 7:
            if (x_typed_explicitly_negative_valued_constant_followed_by_implicitly_valued_constant())
                break;

            state++;

        case 8:
            if (x_typed_implicitly_valued_constant_between_explicitly_valued_constants())
                break;

            state++;

        case 9:
            if (x_typed_implicitly_valued_constants_only())
                break;

            state++;

        case 10:
            if (x_typed_implicitly_valued_constants_overflow_underlying_type())
                break;

            state++;

        default:
            return false;
    }

    return true;
}
