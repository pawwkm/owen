#include "runner.h"

static void missing_declaration(void)
{
    format_test_name("parsing/missing_declaration");

    format_source_path(0, "parsing/missing_declaration.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "lol_wut");

    SET_COMMAND_LINE("parsing/missing_declaration.owen");
    format_expectation(&test.expected_error, "%s:4:1: Function, external function, enumeration, structure, union, version or proposition expected.\n", test.source_paths[0]);
}

static void invalid_token(void)
{
    format_test_name("parsing/unexpected_character");

    format_source_path(0, "parsing/unexpected_character.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function some_function()\n"
                          "    \\\n"
                          "end");

    SET_COMMAND_LINE("parsing/unexpected_character.owen");
    format_expectation(&test.expected_error, "%s:5:5: Invalid token starting with U+005C.\n", test.source_paths[0]);
}

bool parsing(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            missing_declaration();
            break;

        case 1:
            invalid_token();
            break;

        default:
            return false;
    }

    return true;
}
