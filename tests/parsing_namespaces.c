#include "runner.h"

static void missing_namespace(void)
{
    format_test_name("parsing/namespaces/missing_namespace");

    format_source_path(0, "parsing/namespaces/missing_namespace.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "this_is_not_the_namespace_keyword Abc");

    SET_COMMAND_LINE("parsing/namespaces/missing_namespace.owen");
    format_expectation(&test.expected_error, "%s:2:1: Namespace declaration expected.\n", test.source_paths[0]);
}

static void non_identifier_namespace(void)
{
    format_test_name("parsing/namespaces/non_identifier_namespace");

    format_source_path(0, "parsing/namespaces/non_identifier_namespace.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace 1324");

    SET_COMMAND_LINE("parsing/namespaces/non_identifier_namespace.owen");
    format_expectation(&test.expected_error, "%s:2:11: Namespace expected.\n", test.source_paths[0]);
}

bool parsing_namespaces(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            missing_namespace();
            break;

        case 1:
            non_identifier_namespace();
            break;

        default:
            return false;
    }

    return true;
}
