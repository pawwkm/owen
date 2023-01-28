#include "runner.h"

static void missing_rhs_square_bracket_in_fixed_array(void)
{
    format_test_name("parsing/types/missing_rhs_square_bracket_in_fixed_array");

    format_source_path(0, "parsing/types/missing_rhs_square_bracket_in_fixed_array.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a([3 I32 a)\n"
                          "end");

    SET_COMMAND_LINE("parsing/types/missing_rhs_square_bracket_in_fixed_array.owen");
    format_expectation(&test.expected_error, "%s:4:15: ] expected.\n", test.source_paths[0]);
}

static void missing_rhs_square_bracket_in_dynamic_array(void)
{
    format_test_name("parsing/types/missing_rhs_square_bracket_in_dynamic_array");

    format_source_path(0, "parsing/types/missing_rhs_square_bracket_in_dynamic_array.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a([ I32 a)\n"
                          "end");

    SET_COMMAND_LINE("parsing/types/missing_rhs_square_bracket_in_dynamic_array.owen");
    format_expectation(&test.expected_error, "%s:4:14: ] expected.\n", test.source_paths[0]);
}

static void missing_rhs_parenthesis_in_function_type(void)
{
    format_test_name("parsing/types/missing_rhs_parenthesis_in_function_type");

    format_source_path(0, "parsing/types/missing_rhs_parenthesis_in_function_type.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(Function(I32 a)\n"
                          "end");

    SET_COMMAND_LINE("parsing/types/missing_rhs_parenthesis_in_function_type.owen");
    format_expectation(&test.expected_error, "%s:4:25: ) expected.\n", test.source_paths[0]);
}

static void missing_return_type_in_function_type_without_parameters(void)
{
    format_test_name("parsing/types/missing_return_type_in_function_type_without_parameters");

    format_source_path(0, "parsing/types/missing_return_type_in_function_type_without_parameters.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(Function() :)\n"
                          "end");

    SET_COMMAND_LINE("parsing/types/missing_return_type_in_function_type_without_parameters.owen");
    format_expectation(&test.expected_error, "%s:4:24: Return type expected.\n", test.source_paths[0]);
}

static void missing_return_type_in_function_type_with_parameters(void)
{
    format_test_name("parsing/types/missing_return_type_in_function_type_with_parameters");

    format_source_path(0, "parsing/types/missing_return_type_in_function_type_with_parameters.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(Function(I32) :)\n"
                          "end");

    SET_COMMAND_LINE("parsing/types/missing_return_type_in_function_type_with_parameters.owen");
    format_expectation(&test.expected_error, "%s:4:27: Return type expected.\n", test.source_paths[0]);
}

static void missing_rhs_square_bracket_in_compound_type(void)
{
    format_test_name("parsing/types/missing_rhs_square_bracket_in_compound_type");

    format_source_path(0, "parsing/types/missing_rhs_square_bracket_in_compound_type.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(A[B)\n"
                          "end");

    SET_COMMAND_LINE("parsing/types/missing_rhs_square_bracket_in_compound_type.owen");
    format_expectation(&test.expected_error, "%s:4:15: ] expected.\n", test.source_paths[0]);
}

static void missing_type_parameter_in_compound_type(void)
{
    format_test_name("parsing/types/missing_type_parameter_in_compound_type");

    format_source_path(0, "parsing/types/missing_type_parameter_in_compound_type.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(A[B, ])\n"
                          "end");

    SET_COMMAND_LINE("parsing/types/missing_type_parameter_in_compound_type.owen");
    format_expectation(&test.expected_error, "%s:4:17: Type expected.\n", test.source_paths[0]);
}

bool parsing_types(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            missing_rhs_square_bracket_in_fixed_array();
            break;

        case 1:
            missing_rhs_square_bracket_in_dynamic_array();
            break;

        case 2:
            missing_rhs_parenthesis_in_function_type();
            break;

        case 3:
            missing_return_type_in_function_type_without_parameters();
            break;

        case 4:
            missing_return_type_in_function_type_with_parameters();
            break;

        case 5:
            missing_rhs_square_bracket_in_compound_type();
            break;

        case 6:
            missing_type_parameter_in_compound_type();
            break;

        default:
            return false;
    }

    return true;
}
