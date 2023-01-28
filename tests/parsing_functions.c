#include "runner.h"

static void first_formal_type_parameters_is_a_non_identifer_type(void)
{
    format_test_name("parsing/functions/first_formal_type_parameters_is_a_non_identifer_type");

    format_source_path(0, "parsing/functions/first_formal_type_parameters_is_a_non_identifer_type.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function some_function[1]\n"
                          "end");

    SET_COMMAND_LINE("parsing/functions/first_formal_type_parameters_is_a_non_identifer_type.owen");
    format_expectation(&test.expected_error, "%s:4:24: Formal type parameter expected.\n", test.source_paths[0]);
}

static void first_non_identifier_formal_parameter_name(void)
{
    format_test_name("parsing/functions/first_non_identifier_formal_parameter_name");

    format_source_path(0, "parsing/functions/first_non_identifier_formal_parameter_name.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function some_function(U32 2)\n"
                          "end");

    SET_COMMAND_LINE("parsing/functions/first_non_identifier_formal_parameter_name.owen");
    format_expectation(&test.expected_error, "%s:4:28: Formal parameter name expected.\n", test.source_paths[0]);
}

static void first_non_identifier_formal_parameter_type(void)
{
    format_test_name("parsing/functions/first_non_identifier_formal_parameter_type");

    format_source_path(0, "parsing/functions/first_non_identifier_formal_parameter_type.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function some_function(2 a)\n"
                          "end");

    SET_COMMAND_LINE("parsing/functions/first_non_identifier_formal_parameter_type.owen");
    format_expectation(&test.expected_error, "%s:4:24: Formal parameter type expected.\n", test.source_paths[0]);
}

static void second_non_identifier_formal_parameter_type(void)
{
    format_test_name("parsing/functions/second_non_identifier_formal_parameter_type");

    format_source_path(0, "parsing/functions/second_non_identifier_formal_parameter_type.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function some_function(I32 a, 2 b)\n"
                          "end");

    SET_COMMAND_LINE("parsing/functions/second_non_identifier_formal_parameter_type.owen");
    format_expectation(&test.expected_error, "%s:4:31: Formal parameter type expected.\n", test.source_paths[0]);
}

static void first_non_identifier_return_type(void)
{
    format_test_name("parsing/functions/first_non_identifier_return_type");

    format_source_path(0, "parsing/functions/first_non_identifier_return_type.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function some_function() : 2\n"
                          "end");

    SET_COMMAND_LINE("parsing/functions/first_non_identifier_return_type.owen");
    format_expectation(&test.expected_error, "%s:4:28: Return type expected.\n", test.source_paths[0]);
}

static void second_non_identifier_return_type(void)
{
    format_test_name("parsing/functions/second_non_identifier_return_type");

    format_source_path(0, "parsing/functions/second_non_identifier_return_type.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function some_function() : I32, 2\n"
                          "end");

    SET_COMMAND_LINE("parsing/functions/second_non_identifier_return_type.owen");
    format_expectation(&test.expected_error, "%s:4:33: Return type expected.\n", test.source_paths[0]);
}

static void non_identifier_function_name(void)
{
    format_test_name("parsing/functions/non_identifier_function_name");

    format_source_path(0, "parsing/functions/non_identifier_function_name.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function 1234\n"
                          "end");

    SET_COMMAND_LINE("parsing/functions/non_identifier_function_name.owen");
    format_expectation(&test.expected_error, "%s:4:10: Function name expected.\n", test.source_paths[0]);
}

static void second_formal_type_parameters_is_a_non_identifer_type(void)
{
    format_test_name("parsing/functions/second_formal_type_parameters_is_a_non_identifer_type");

    format_source_path(0, "parsing/functions/second_formal_type_parameters_is_a_non_identifer_type.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function some_function[T, 1]()\n"
                          "end");

    SET_COMMAND_LINE("parsing/functions/second_formal_type_parameters_is_a_non_identifer_type.owen");
    format_expectation(&test.expected_error, "%s:4:27: Formal type parameter expected.\n", test.source_paths[0]);
}

static void missing_right_square_bracket(void)
{
    format_test_name("parsing/functions/missing_right_square_bracket");

    format_source_path(0, "parsing/functions/missing_right_square_bracket.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T\n"
                          "end");

    SET_COMMAND_LINE("parsing/functions/missing_right_square_bracket.owen");
    format_expectation(&test.expected_error, "%s:5:1: ] expected.\n", test.source_paths[0]);
}

static void missing_left_parenthesis(void)
{
    format_test_name("parsing/functions/missing_left_parenthesis");

    format_source_path(0, "parsing/functions/missing_left_parenthesis.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a I32 b)\n"
                          "end");

    SET_COMMAND_LINE("parsing/functions/missing_left_parenthesis.owen");
    format_expectation(&test.expected_error, "%s:4:12: ( expected.\n", test.source_paths[0]);
}

static void missing_return_type(void)
{
    format_test_name("parsing/functions/missing_return_type");

    format_source_path(0, "parsing/functions/missing_return_type.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b) :\n"
                          "end");

    SET_COMMAND_LINE("parsing/functions/missing_return_type.owen");
    format_expectation(&test.expected_error, "%s:5:1: Return type expected.\n", test.source_paths[0]);
}

static void missing_end_keyword(void)
{
    format_test_name("parsing/functions/missing_end_keyword");

    format_source_path(0, "parsing/functions/missing_end_keyword.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "function");

    SET_COMMAND_LINE("parsing/functions/missing_end_keyword.owen");
    format_expectation(&test.expected_error, "%s:5:1: Missing end keyword in function.\n", test.source_paths[0]);
}

bool parsing_functions(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            first_formal_type_parameters_is_a_non_identifer_type();
            break;

        case 1:
            first_non_identifier_formal_parameter_name();
            break;

        case 2:
            first_non_identifier_formal_parameter_type();
            break;

        case 3:
            second_non_identifier_formal_parameter_type();
            break;

        case 4:
            first_non_identifier_return_type();
            break;

        case 5:
            second_non_identifier_return_type();
            break;

        case 6:
            non_identifier_function_name();
            break;

        case 7:
            second_formal_type_parameters_is_a_non_identifer_type();
            break;

        case 8:
            missing_right_square_bracket();
            break;

        case 9:
            missing_left_parenthesis();
            break;

        case 10:
            missing_return_type();
            break;

        case 11:
            missing_end_keyword();
            break;

        default:
            return false;
    }

    return true;
}
