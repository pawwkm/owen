#include "runner.h"

static bool x_typed_literal_overflows(void)
{
    static uint8_t type_index = FIRST_INTEGER_TYPE_INDEX;
    while (true)
    {
        if (type_index > LAST_INTEGER_TYPE_INDEX)
            return false;

        format_test_name("semantics/expressions/integer_literal/%s_typed_literal_overflows", types[type_index].file_friendly_name);

        format_source_path(0, "%s.owen", test.name);
        format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                              "namespace Abc\n"
                              "\n"
                              "function a() : %s\n"
                              "    return %s\n"
                              "end",
                              types[type_index].name,
                              types[type_index].overflow_value);

        format_command_line_options(test.source_paths[0]);
        format_expectation(&test.expected_error, 
                           "%s:5:12: %s overflows %s.\n",
                           test.source_paths[0],
                           types[type_index].overflow_value,
                           types[type_index].name);

        type_index++;
        return true;
    }
}

static bool x_typed_literal_underflows(void)
{
    static uint8_t type_index = FIRST_SIGNED_NUMBER_TYPE_INDEX;
    while (true)
    {
        if (type_index > LAST_SIGNED_NUMBER_TYPE_INDEX)
            return false;

        format_test_name("semantics/expressions/integer_literal/%s_typed_literal_underflows", types[type_index].file_friendly_name);

        format_source_path(0, "%s.owen", test.name);
        format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                "namespace Abc\n"
                                "\n"
                                "function a() : %s\n"
                                "    return %s\n"
                                "end",
                                types[type_index].name,
                                types[type_index].underflow_value);

        format_command_line_options(test.source_paths[0]);
        format_expectation(&test.expected_error, 
                            "%s:5:12: %s underflows %s.\n",
                            test.source_paths[0],
                            types[type_index].underflow_value,
                            types[type_index].name);

        type_index++;
        return true;
    }
}

static bool x_typed_literal_with_leading_zeroes(void)
{
    static uint8_t type_index = FIRST_INTEGER_TYPE_INDEX;
    while (true)
    {
        if (type_index > LAST_INTEGER_TYPE_INDEX)
            return false;

        format_test_name("semantics/expressions/integer_literal/%s_typed_literal_with_leading_zeroes", types[type_index].file_friendly_name);

        format_source_path(0, "%s.owen", test.name);
        format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                              "namespace Abc\n"
                              "\n"
                              "function a() : %s\n"
                              "    return 0001\n"
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
                            "        return_type: %s\n"
                            "        return_statement\n"
                            "            integer_literal\n"
                            "                type: %s\n"
                            "                value: 1\n"
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
}

static bool x_typed_binary_literal(void)
{
    static uint8_t type_index = FIRST_INTEGER_TYPE_INDEX;
    while (true)
    {
        if (type_index > LAST_INTEGER_TYPE_INDEX)
            return false;

        format_test_name("semantics/expressions/integer_literal/%s_typed_binary_literal", types[type_index].file_friendly_name);

        format_source_path(0, "%s.owen", test.name);
        format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                "namespace Abc\n"
                                "\n"
                                "function a() : %s\n"
                                "    return 0b1011\n"
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
                           "        return_type: %s\n"
                           "        return_statement\n"
                           "            integer_literal\n"
                           "                type: %s\n"
                           "                value: 11\n"
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
}

static bool x_typed_decimal_literal(void)
{
    static uint8_t type_index = FIRST_INTEGER_TYPE_INDEX;
    while (true)
    {
        if (type_index > LAST_INTEGER_TYPE_INDEX)
            return false;

        format_test_name("semantics/expressions/integer_literal/%s_typed_decimal_literal", types[type_index].file_friendly_name);

        format_source_path(0, "%s.owen", test.name);
        format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                "namespace Abc\n"
                                "\n"
                                "function a() : %s\n"
                                "    return 14\n"
                                "end\n"
                                "\n"
                                "function main() : I32\n"
                                "    return 0\n"
                                "end\n",
                                types[type_index].name);

        format_command_line_options("%s -print-semantics", test.source_paths[0]);
        format_expectation(&test.expected_semantics, 
                           "file\n"
                           "    path: %s\n"
                           "    namespace: Abc\n"
                           "    function\n"
                           "        name: a\n"
                           "        return_type: %s\n"
                           "        return_statement\n"
                           "            integer_literal\n"
                           "                type: %s\n"
                           "                value: 14\n"
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
}

static bool x_typed_hexadecimal_literal(void)
{
    static uint8_t type_index = FIRST_INTEGER_TYPE_INDEX;
    while (true)
    {
        if (type_index > LAST_INTEGER_TYPE_INDEX)
            return false;

        format_test_name("semantics/expressions/integer_literal/%s_typed_hexadecimal_literal", types[type_index].file_friendly_name);

        format_source_path(0, "%s.owen", test.name);
        format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                "namespace Abc\n"
                                "\n"
                                "function a() : %s\n"
                                "    return 0x1F\n"
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
                           "        return_type: %s\n"
                           "        return_statement\n"
                           "            integer_literal\n"
                           "                type: %s\n"
                           "                value: 31\n"
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
}

static bool x_typed_maximum_literal(void)
{
    static uint8_t type_index = FIRST_INTEGER_TYPE_INDEX;
    while (true)
    {
        if (type_index > LAST_INTEGER_TYPE_INDEX)
            return false;

        format_test_name("semantics/expressions/integer_literal/%s_typed_maximum_literal", types[type_index].file_friendly_name);

        format_source_path(0, "%s.owen", test.name);
        format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                              "namespace Abc\n"
                              "\n"
                              "function a() : %s\n"
                              "    return %s\n"
                              "end\n"
                              "\n"
                              "function main() : I32\n"
                              "    return 0\n"
                              "end",
                              types[type_index].name,
                              types[type_index].max_value);

        format_command_line_options("%s -print-semantics", test.source_paths[0]);
        format_expectation(&test.expected_semantics, 
                            "file\n"
                            "    path: %s\n"
                            "    namespace: Abc\n"
                            "    function\n"
                            "        name: a\n"
                            "        return_type: %s\n"
                            "        return_statement\n"
                            "            integer_literal\n"
                            "                type: %s\n"
                            "                value: %s\n"
                            "    function\n"
                            "        name: main\n"
                            "        return_type: I32\n"
                            "        return_statement\n"
                            "            integer_literal\n"
                            "                type: I32\n"
                            "                value: 0\n",
                            test.source_paths[0],
                            types[type_index].name,
                            types[type_index].name,
                            types[type_index].max_value);

        type_index++;
        return true;
    }
}

static bool x_typed_minimum_literal(void)
{
    static uint8_t type_index = FIRST_INTEGER_TYPE_INDEX;
    while (true)
    {
        if (type_index > LAST_INTEGER_TYPE_INDEX)
            return false;

        format_test_name("semantics/expressions/integer_literal/%s_typed_minimum_literal", types[type_index].file_friendly_name);

        format_source_path(0, "%s.owen", test.name);
        format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                              "namespace Abc\n"
                              "\n"
                              "function a() : %s\n"
                              "    return %s\n"
                              "end\n"
                              "\n"
                              "function main() : I32\n"
                              "    return 0\n"
                              "end",
                              types[type_index].name,
                              types[type_index].min_value);

        format_command_line_options("%s -print-semantics", test.source_paths[0]);
        format_expectation(&test.expected_semantics, 
                            "file\n"
                            "    path: %s\n"
                            "    namespace: Abc\n"
                            "    function\n"
                            "        name: a\n"
                            "        return_type: %s\n"
                            "        return_statement\n"
                            "            integer_literal\n"
                            "                type: %s\n"
                            "                value: %s\n"
                            "    function\n"
                            "        name: main\n"
                            "        return_type: I32\n"
                            "        return_statement\n"
                            "            integer_literal\n"
                            "                type: I32\n"
                            "                value: 0\n",
                            test.source_paths[0],
                            types[type_index].name,
                            types[type_index].name,
                            types[type_index].min_value);

        type_index++;
        return true;
    }
}

static bool x_typed_multiple_zeroes_binary_literal_with_seperators(void)
{
    static uint8_t type_index = FIRST_INTEGER_TYPE_INDEX;
    while (true)
    {
        if (type_index > LAST_INTEGER_TYPE_INDEX)
            return false;

        format_test_name("semantics/expressions/integer_literal/%s_typed_multiple_zeroes_binary_literal_with_seperators", types[type_index].file_friendly_name);

        format_source_path(0, "%s.owen", test.name);
        format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                              "namespace Abc\n"
                              "\n"
                              "function a() : %s\n"
                              "    return 0b0_000\n"
                              "end"
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
                            "        return_type: %s\n"
                            "        return_statement\n"
                            "            integer_literal\n"
                            "                type: %s\n"
                            "                value: 0\n"
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
}

static bool x_typed_multiple_zeroes_decimal_literal_with_seperators(void)
{
    static uint8_t type_index = FIRST_INTEGER_TYPE_INDEX;
    while (true)
    {
        if (type_index > LAST_INTEGER_TYPE_INDEX)
            return false;

        format_test_name("semantics/expressions/integer_literal/%s_typed_multiple_zeroes_decimal_literal_with_seperators", types[type_index].file_friendly_name);

        format_source_path(0, "%s.owen", test.name);
        format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                              "namespace Abc\n"
                              "\n"
                              "function a() : %s\n"
                              "    return 0_000\n"
                              "end"
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
                            "        return_type: %s\n"
                            "        return_statement\n"
                            "            integer_literal\n"
                            "                type: %s\n"
                            "                value: 0\n"
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
}

static bool x_typed_multiple_zeroes_decimal_literal_without_seperators(void)
{
    static uint8_t type_index = FIRST_INTEGER_TYPE_INDEX;
    while (true)
    {
        if (type_index > LAST_INTEGER_TYPE_INDEX)
            return false;

        format_test_name("semantics/expressions/integer_literal/%s_typed_multiple_zeroes_decimal_literal_without_seperators", types[type_index].file_friendly_name);

        format_source_path(0, "%s.owen", test.name);
        format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                              "namespace Abc\n"
                              "\n"
                              "function a() : %s\n"
                              "    return 000\n"
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
                            "        return_type: %s\n"
                            "        return_statement\n"
                            "            integer_literal\n"
                            "                type: %s\n"
                            "                value: 0\n"
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
}

static bool x_typed_single_zero_binary_digit_literal(void)
{
    static uint8_t type_index = FIRST_UNSIGNED_NUMBER_TYPE_INDEX;
    while (true)
    {
        if (type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)
            return false;

        format_test_name("semantics/expressions/integer_literal/%s_typed_single_zero_binary_digit_literal", types[type_index].file_friendly_name);

        format_source_path(0, "%s.owen", test.name);
        format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                              "namespace Abc\n"
                              "\n"
                              "function a() : %s\n"
                              "    return 0b0\n"
                              "end"
                              "\n"
                              "function main() : I32\n"
                              "    return 0\n"
                              "end",
                              types[type_index].name);

        format_command_line_options(test.source_paths[0]);
        format_expectation(&test.expected_semantics, 
                           "file\n"
                           "    path: %s\n"
                           "    namespace: Abc\n"
                           "    function\n"
                           "        name: a\n"
                           "        return_type: %s\n"
                           "        return_statement\n"
                           "            integer_literal\n"
                           "                type: %s\n"
                           "                value: 0\n"
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
}

static bool x_typed_single_zero_decimal_digit_literal(void)
{
    static uint8_t type_index = FIRST_UNSIGNED_NUMBER_TYPE_INDEX;
    while (true)
    {
        if (type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)
            return false;

        format_test_name("semantics/expressions/integer_literal/%s_typed_single_zero_decimal_digit_literal", types[type_index].file_friendly_name);

        format_source_path(0, "%s.owen", test.name);
        format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                              "namespace Abc\n"
                              "\n"
                              "function a() : %s\n"
                              "    return 0\n"
                              "end",
                              types[type_index].name);

        format_command_line_options(test.source_paths[0]);
        format_expectation(&test.expected_semantics, 
                           "file\n"
                           "    path: %s\n"
                           "    namespace: Abc\n"
                           "    function\n"
                           "        name: a\n"
                           "        return_type: %s\n"
                           "        return_statement\n"
                           "            integer_literal\n"
                           "                type: %s\n"
                           "                value: 0\n",
                           test.source_paths[0],
                           types[type_index].name,
                           types[type_index].name);

        type_index++;
        return true;
    }
}

static bool x_typed_single_zero_hexadecimal_digit_literal(void)
{
    static uint8_t type_index = FIRST_UNSIGNED_NUMBER_TYPE_INDEX;
    while (true)
    {
        if (type_index > LAST_UNSIGNED_NUMBER_TYPE_INDEX)
            return false;

        format_test_name("semantics/expressions/integer_literal/%s_typed_single_zero_hexadecimal_digit_literal", types[type_index].file_friendly_name);

        format_source_path(0, "%s.owen", test.name);
        format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                              "namespace Abc\n"
                              "\n"
                              "function a() : %s\n"
                              "    return 0x0\n"
                              "end\n"
                              "\n"
                              "function main() : I32\n"
                              "    return 0\n"
                              "end",
                              types[type_index].name);

        format_command_line_options(test.source_paths[0]);
        format_expectation(&test.expected_semantics, 
                           "file\n"
                           "    path: %s\n"
                           "    namespace: Abc\n"
                           "    function\n"
                           "        name: a\n"
                           "        return_type: %s\n"
                           "        return_statement\n"
                           "            integer_literal\n"
                           "                type: %s\n"
                           "                value: 0\n"
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
}


bool semantics_expressions_integer_literal(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            if (x_typed_literal_overflows())
                break;

            state++;

        case 1:
            if (x_typed_literal_underflows())
                break;

            state++;

        case 2:
            if (x_typed_literal_with_leading_zeroes())
                break;

            state++; 

        case 3:
            if (x_typed_binary_literal())
                break;

            state++;

        case 4:
            if (x_typed_decimal_literal())
                break;

            state++;

        case 5:
            if (x_typed_hexadecimal_literal())
                break;

            state++;

        case 6:
            if (x_typed_maximum_literal())
                break;

            state++;

        case 7:
            if (x_typed_minimum_literal())
                break;

            state++;

        case 8:
            if (x_typed_multiple_zeroes_binary_literal_with_seperators())
                break;

            state++;

        case 9:
            if (x_typed_multiple_zeroes_decimal_literal_with_seperators())
                break;

            state++;

        case 10:
            if (x_typed_multiple_zeroes_decimal_literal_without_seperators())
                break;

            state++;

        case 11:
            if (x_typed_multiple_zeroes_decimal_literal_without_seperators())
                break;

            state++;

        default:
            return false;
    }

    return true;
}
