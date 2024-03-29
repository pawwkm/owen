#include "runner.h"

static void missing_semicolon_after_declaration(void)
{
    format_test_name("parsing/statements/while/missing_semicolon_after_declaration");

    format_source_path(0, "parsing/statements/while/missing_semicolon_after_declaration.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main()\n"
                          "    while I32 a = 1 true");

    SET_COMMAND_LINE("parsing/statements/while/missing_semicolon_after_declaration.owen");
    format_expectation(&test.expected_error, "%s:5:21: ; expected after declaration statement.\n", test.source_paths[0]);
}

static void while_declaration_and_condition_is_missing(void)
{
    format_test_name("parsing/statements/while/while_declaration_and_condition_is_missing");

    format_source_path(0, "parsing/statements/while/while_declaration_and_condition_is_missing.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main()\n"
                          "    while ;");

    SET_COMMAND_LINE("parsing/statements/while/while_declaration_and_condition_is_missing.owen");
    format_expectation(&test.expected_error,"%s:5:11: Declaration or condition expected.\n", test.source_paths[0]);
}

static void missing_condition_after_declaration(void)
{
    format_test_name("parsing/statements/while/missing_condition_after_declaration");

    format_source_path(0, "parsing/statements/while/missing_condition_after_declaration.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main()\n"
                          "    while I32 a; .");

    SET_COMMAND_LINE("parsing/statements/while/missing_condition_after_declaration.owen");
    format_expectation(&test.expected_error,"%s:5:18: Condition expected.\n", test.source_paths[0]);
}

static void missing_end_keyword(void)
{
    format_test_name("parsing/statements/while/missing_end_keyword");

    format_source_path(0, "parsing/statements/while/missing_end_keyword.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main()\n"
                          "    while b\n"
                          "function");

    SET_COMMAND_LINE("parsing/statements/while/missing_end_keyword.owen");
    format_expectation(&test.expected_error,"%s:6:1: Missing end keyword in while loop.\n", test.source_paths[0]);
}

bool parsing_statements_while(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            missing_semicolon_after_declaration();
            break;

        case 1:
            while_declaration_and_condition_is_missing();
            break;

        case 2:
            missing_condition_after_declaration();
            break;

        case 3:
            missing_end_keyword();
            break;

        default:
            return false;
    }

    return true;
}
