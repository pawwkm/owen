#include "runner.h"

static void missing_end_keyword(void)
{
    format_test_name("parsing/statements/for/missing_end_keyword");

    format_source_path(0, "parsing/statements/for/missing_end_keyword.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main()\n"
                          "    for I32 i = 0; true; i += 1\n"
                          "        break\n"
                          "\n"
                          "    public\n"
                          "end");

    SET_COMMAND_LINE("parsing/statements/for/missing_end_keyword.owen");
    format_expectation(&test.expected_error, "%s:8:5: Missing end keyword in for loop.\n", test.source_paths[0]);
}

static void missing_semicolon_after_condition(void)
{
    format_test_name("parsing/statements/for/missing_semicolon_after_condition");

    format_source_path(0, "parsing/statements/for/missing_semicolon_after_condition.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main()\n"
                          "    for I32 i = 0; true i += 1");

    SET_COMMAND_LINE("parsing/statements/for/missing_semicolon_after_condition.owen");
    format_expectation(&test.expected_error, "%s:5:25: ; expected after condition.\n", test.source_paths[0]);
}

static void missing_semicolon_after_pre_assignment_statement(void)
{
    format_test_name("parsing/statements/for/missing_semicolon_after_pre_assignment_statement");

    format_source_path(0, "parsing/statements/for/missing_semicolon_after_pre_assignment_statement.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main()\n"
                          "    for I32 i = 0 true; i += 1");

    SET_COMMAND_LINE("parsing/statements/for/missing_semicolon_after_pre_assignment_statement.owen");
    format_expectation(&test.expected_error, "%s:5:19: ; expected after declaration statement.\n", test.source_paths[0]);
}

static void missing_condition(void)
{
    format_test_name("parsing/statements/for/missing_condition");

    format_source_path(0, "parsing/statements/for/missing_condition.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main()\n"
                          "    for I32 i = 0; ; i += 1");

    SET_COMMAND_LINE("parsing/statements/for/missing_condition.owen");
    format_expectation(&test.expected_error, "%s:5:20: Condition expected.\n", test.source_paths[0]);
}

bool parsing_statements_for(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            missing_end_keyword();
            break;

        case 1:
            missing_semicolon_after_condition();
            break;

        case 2:
            missing_semicolon_after_pre_assignment_statement();
            break;
        
        case 3:
            missing_condition();
            break;

        default:
            return false;
    }

    return true;
}
