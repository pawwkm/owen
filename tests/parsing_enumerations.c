#include "runner.h"

static void invalid_name(void)
{
    format_test_name("parsing/enumerations/invalid_name");

    format_source_path(0, "parsing/enumerations/invalid_name.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration 1 : I32");

    SET_COMMAND_LINE("parsing/enumerations/invalid_name.owen");
    format_expectation(&test.expected_error, "%s:4:13: Type expected.\n", test.source_paths[0]);
}

static void missing_colon(void)
{
    format_test_name("parsing/enumerations/missing_colon");

    format_source_path(0, "parsing/enumerations/missing_colon.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A I32");

    SET_COMMAND_LINE("parsing/enumerations/missing_colon.owen");
    format_expectation(&test.expected_error, "%s:4:15: : expected.\n", test.source_paths[0]);
}

static void invalid_underlying_type(void)
{
    format_test_name("parsing/enumerations/invalid_underlying_type");

    format_source_path(0, "parsing/enumerations/invalid_underlying_type.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : 1");

    SET_COMMAND_LINE("parsing/enumerations/invalid_underlying_type.owen");
    format_expectation(&test.expected_error, "%s:4:17: Type expected.\n", test.source_paths[0]);
}

static void missing_constant_expression(void)
{
    format_test_name("parsing/enumerations/missing_constant_expression");

    format_source_path(0, "parsing/enumerations/missing_constant_expression.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : I32\n"
                          "    a =\n"
                          "end");

    SET_COMMAND_LINE("parsing/enumerations/missing_constant_expression.owen");
    format_expectation(&test.expected_error, "%s:6:1: Expression expected.\n", test.source_paths[0]);
}


static void missing_end_keyword(void)
{
    format_test_name("parsing/enumerations/missing_end_keyword");

    format_source_path(0, "parsing/enumerations/missing_end_keyword.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : I32\n"
                          "\n"
                          "function");

    SET_COMMAND_LINE("parsing/enumerations/missing_end_keyword.owen");
    format_expectation(&test.expected_error, "%s:6:1: Missing end keyword in enumeration.\n", test.source_paths[0]);
}

bool parsing_enumerations(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            invalid_name();
            break;

        case 1:
            missing_colon();
            break;

        case 2:
            invalid_underlying_type();
            break;

        case 3:
            missing_constant_expression();
            break;

        case 4:
            missing_end_keyword();
            break;

        default:
            return false;
    }

    return true;
}
