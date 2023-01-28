#include "runner.h"

static void if_declaration_and_condition_is_missing(void)
{
    format_test_name("parsing/statements/if/if_declaration_and_condition_is_missing");

    format_source_path(0, "parsing/statements/if/if_declaration_and_condition_is_missing.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main()\n"
                          "    if ;");

    SET_COMMAND_LINE("parsing/statements/if/if_declaration_and_condition_is_missing.owen");
    format_expectation(&test.expected_error, "%s:5:8: Declaration or condition expected.\n", test.source_paths[0]);
}

static void elif_declaration_and_condition_is_missing(void)
{
    format_test_name("parsing/statements/if/elif_declaration_and_condition_is_missing");

    format_source_path(0, "parsing/statements/if/elif_declaration_and_condition_is_missing.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main()\n"
                          "    if true\n"
                          "    elif ;");

    SET_COMMAND_LINE("parsing/statements/if/elif_declaration_and_condition_is_missing.owen");
    format_expectation(&test.expected_error, "%s:6:10: Declaration or condition expected.\n", test.source_paths[0]);
}

static void if_declaration_but_condition_is_missing(void)
{
    format_test_name("parsing/statements/if/if_declaration_but_condition_is_missing");

    format_source_path(0, "parsing/statements/if/if_declaration_but_condition_is_missing.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main()\n"
                          "    if I32 a = 1;\n"
                          "    end");

    SET_COMMAND_LINE("parsing/statements/if/if_declaration_but_condition_is_missing.owen");
    format_expectation(&test.expected_error, "%s:6:5: Condition expected.\n", test.source_paths[0]);
}

static void elif_declaration_but_condition_is_missing(void)
{
    format_test_name("parsing/statements/if/elif_declaration_but_condition_is_missing");

    format_source_path(0, "parsing/statements/if/elif_declaration_but_condition_is_missing.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main()\n"
                          "    if true\n"
                          "    elif I32 a = 1;\n"
                          "    end");

    SET_COMMAND_LINE("parsing/statements/if/elif_declaration_but_condition_is_missing.owen");
    format_expectation(&test.expected_error, "%s:7:5: Condition expected.\n", test.source_paths[0]);
}

static void if_missing_semicolon_after_declaration(void)
{
    format_test_name("parsing/statements/if/if_missing_semicolon_after_declaration");

    format_source_path(0, "parsing/statements/if/if_missing_semicolon_after_declaration.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main()\n"
                          "    if I32 a = 1 true");

    SET_COMMAND_LINE("parsing/statements/if/if_missing_semicolon_after_declaration.owen");
    format_expectation(&test.expected_error, "%s:5:18: ; expected after declaration statement.\n", test.source_paths[0]);
}

static void elif_missing_semicolon_after_declaration(void)
{
    format_test_name("parsing/statements/if/elif_missing_semicolon_after_declaration");

    format_source_path(0, "parsing/statements/if/elif_missing_semicolon_after_declaration.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main()\n"
                          "    if true\n"
                          "    elif I32 a = 1 true");

    SET_COMMAND_LINE("parsing/statements/if/elif_missing_semicolon_after_declaration.owen");
    format_expectation(&test.expected_error, "%s:6:20: ; expected after declaration statement.\n", test.source_paths[0]);
}

static void missing_end_keyword(void)
{
    format_test_name("parsing/statements/if/missing_end_keyword");

    format_source_path(0, "parsing/statements/if/missing_end_keyword.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
        "namespace Abc\n"
        "\n"
        "function main()\n"
        "    if b\n"
        "function");

    SET_COMMAND_LINE("parsing/statements/if/missing_end_keyword.owen");
    format_expectation(&test.expected_error, "%s:6:1: Missing end keyword in branch.\n", test.source_paths[0]);
}

bool parsing_statements_if(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            if_declaration_and_condition_is_missing();
            break;

        case 1:
            elif_declaration_and_condition_is_missing();
            break;

        case 2:
            if_declaration_but_condition_is_missing();
            break;

        case 3:
            elif_declaration_but_condition_is_missing();
            break;

        case 4:
            if_missing_semicolon_after_declaration();
            break;

        case 5:
            elif_missing_semicolon_after_declaration();
            break;

        case 6:
            missing_end_keyword();
            break;

        default:
            return false;
    }

    return true;
}
