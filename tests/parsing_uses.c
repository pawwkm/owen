#include "runner.h"

static void non_identifier_namespace(void)
{
    format_test_name("parsing/uses/non_identifier_namespace");

    format_source_path(0, "parsing/uses/non_identifier_namespace.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "use 1324");

    SET_COMMAND_LINE("parsing/uses/non_identifier_namespace.owen");
    format_expectation(&test.expected_error, "%s:4:5: Namespace expected.\n", test.source_paths[0]);
}

bool parsing_uses(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            non_identifier_namespace();
            break;

        default:
            return false;
    }

    return true;
}
