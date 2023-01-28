#include "runner.h"

#define INVALID_CODE_POINTS_LENGTH (sizeof(invalid_code_points) / sizeof(invalid_code_points[0]))
struct 
{
    char*   name;
    uint8_t encoded[3];
    uint8_t encoded_length;
} invalid_code_points[] =
{
    { "0000", { 0x00             }, 1 },
    { "0001", { 0x01             }, 1 },
    { "0002", { 0x02             }, 1 },
    { "0003", { 0x03             }, 1 },
    { "0004", { 0x04             }, 1 },
    { "0005", { 0x05             }, 1 },
    { "0006", { 0x06             }, 1 },
    { "0007", { 0x07             }, 1 },
    { "0008", { 0x08             }, 1 },
    { "0009", { 0x09             }, 1 },
    { "000B", { 0x0B             }, 1 },
    { "000C", { 0x0C             }, 1 },
    { "000D", { 0x0D             }, 1 },
    { "000E", { 0x0E             }, 1 },
    { "000F", { 0x0F             }, 1 },
    { "0010", { 0x10             }, 1 },
    { "0011", { 0x11             }, 1 },
    { "0012", { 0x12             }, 1 },
    { "0013", { 0x13             }, 1 },
    { "0014", { 0x14             }, 1 },
    { "0015", { 0x15             }, 1 },
    { "0016", { 0x16             }, 1 },
    { "0017", { 0x17             }, 1 },
    { "0018", { 0x18             }, 1 },
    { "0019", { 0x19             }, 1 },
    { "001A", { 0x1A             }, 1 },
    { "001B", { 0x1B             }, 1 },
    { "001C", { 0x1C             }, 1 },
    { "001D", { 0x1D             }, 1 },
    { "001E", { 0x1E             }, 1 },
    { "001F", { 0x1F             }, 1 },
    { "007F", { 0x7F             }, 1 },
    { "0085", { 0xC2, 0x85       }, 2 },
    { "2028", { 0xE2, 0x80, 0xA8 }, 3 },
    { "2029", { 0xE2, 0x80, 0xA9 }, 3 }
};

// Valid Unicode code points https://www.json.org/JSON_checker/utf8_decode.c
// 0     127
// 128   2047
// 2048  55295
// 57344 65535
// 65536 1114111
//
// Condensed code_point <= 55295 ||
//           code_point >= 57344 && code_point <= 1114111
//
static bool invalid_code_point_in_comment(void)
{    
    static uint8_t index = 0;
    if (index == INVALID_CODE_POINTS_LENGTH)
        return false;

    format_test_name("parsing/tokens/u%s_in_comment", invalid_code_points[index].name);
    format_source_path(0, "%s.owen", test.name);

    if (invalid_code_points[index].encoded_length == 1)
    {
        format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                              "// %c", 
                              invalid_code_points[index].encoded[0]);
    }
    else if (invalid_code_points[index].encoded_length == 2)
    {
        format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                              "// %c%c", 
                              invalid_code_points[index].encoded[0],
                              invalid_code_points[index].encoded[1]);
    }
    else if (invalid_code_points[index].encoded_length == 3)
    {
        format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                              "// %c%c%c", 
                              invalid_code_points[index].encoded[0],
                              invalid_code_points[index].encoded[1],
                              invalid_code_points[index].encoded[2]);
    }
    
    format_command_line_options(test.source_paths[0]);
    format_expectation(&test.expected_error, 
                       "%s:2:1: U+%s not valid in comments.\n",
                       test.source_paths[0],
                       invalid_code_points[index].name);

    index++;
    
    return true;
}

static bool invalid_code_point_in_string(void)
{    
    static uint8_t index = 0;
    if (index == INVALID_CODE_POINTS_LENGTH)
        return false;

    format_test_name("parsing/tokens/u%s_in_string", invalid_code_points[index].name);
    format_source_path(0, "%s.owen", test.name);

    if (invalid_code_points[index].encoded_length == 1)
    {
        format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                              "\"%c\"", 
                              invalid_code_points[index].encoded[0]);
    }
    else if (invalid_code_points[index].encoded_length == 2)
    {
        format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                              "\"%c%c\"", 
                              invalid_code_points[index].encoded[0],
                              invalid_code_points[index].encoded[1]);
    }
    else if (invalid_code_points[index].encoded_length == 3)
    {
        format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                              "\"%c%c%c\"", 
                              invalid_code_points[index].encoded[0],
                              invalid_code_points[index].encoded[1],
                              invalid_code_points[index].encoded[2]);
    }
    
    format_command_line_options(test.source_paths[0]);
    format_expectation(&test.expected_error, 
                       "%s:2:1: U+%s must be escaped in strings and Unicode code point literal.\n",
                       test.source_paths[0],
                       invalid_code_points[index].name);

    index++;
    
    return true;
}

static bool invalid_code_point_in_unicode_code_point_literal(void)
{    
    static uint8_t index = 0;
    if (index == INVALID_CODE_POINTS_LENGTH)
        return false;

    format_test_name("parsing/tokens/u%s_in_unicode_code_point_literal", invalid_code_points[index].name);
    format_source_path(0, "%s.owen", test.name);

    if (invalid_code_points[index].encoded_length == 1)
    {
        format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                              "'%c'", 
                              invalid_code_points[index].encoded[0]);
    }
    else if (invalid_code_points[index].encoded_length == 2)
    {
        format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                              "'%c%c'", 
                              invalid_code_points[index].encoded[0],
                              invalid_code_points[index].encoded[1]);
    }
    else if (invalid_code_points[index].encoded_length == 3)
    {
        format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                              "'%c%c%c'", 
                              invalid_code_points[index].encoded[0],
                              invalid_code_points[index].encoded[1],
                              invalid_code_points[index].encoded[2]);
    }
    
    format_command_line_options(test.source_paths[0]);
    format_expectation(&test.expected_error, 
                       "%s:2:1: U+%s must be escaped in strings and Unicode code point literal.\n",
                       test.source_paths[0],
                       invalid_code_points[index].name);

    index++;
    
    return true;
}

bool parsing_tokens(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            if (invalid_code_point_in_comment())
                break;

            state++;

        case 1:
            if (invalid_code_point_in_string())
                break;

            state++;

        case 2:
            if (invalid_code_point_in_unicode_code_point_literal())
                break;

            state++;

        default:
            return false;
    }

    return true;
}
