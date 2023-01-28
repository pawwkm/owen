#include "runner.h"

static void empty_monomorphic_compound(void)
{
    format_test_name("parsing/compounds/empty_monomorphic_compound");

    format_source_path(0, "parsing/compounds/empty_monomorphic_compound.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure Empty\n"
                          "end");

    SET_COMMAND_LINE("parsing/compounds/empty_monomorphic_compound.owen");
    format_expectation(&test.expected_error, "%s:5:1: Field type expected.\n", test.source_paths[0]);
}

static void empty_polymorphic_compound(void)
{
    format_test_name("parsing/compounds/empty_polymorphic_compound");

    format_source_path(0, "parsing/compounds/empty_polymorphic_compound.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure Empty[T]\n"
                          "end");

    SET_COMMAND_LINE("parsing/compounds/empty_polymorphic_compound.owen");
    format_expectation(&test.expected_error, "%s:5:1: Field type expected.\n", test.source_paths[0]);
}

static void missing_first_formal_type_parameter(void)
{
    // Because of backtracking the empty [] are thought to
    // be the start of a dynamic array but end is not a type.
    format_test_name("parsing/compounds/missing_first_formal_type_parameter");

    format_source_path(0, "parsing/compounds/missing_first_formal_type_parameter.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure Empty[]\n"
                          "end");

    SET_COMMAND_LINE("parsing/compounds/missing_first_formal_type_parameter.owen");
    format_expectation(&test.expected_error, "%s:5:1: Base type expected.\n", test.source_paths[0]);
}

static void missing_second_formal_type_parameter(void)
{
    format_test_name("parsing/compounds/missing_second_formal_type_parameter");

    format_source_path(0, "parsing/compounds/missing_second_formal_type_parameter.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure Empty[A, ]\n"
                          "end");

    SET_COMMAND_LINE("parsing/compounds/missing_second_formal_type_parameter.owen");
    format_expectation(&test.expected_error, "%s:4:20: Formal type parameter expected.\n", test.source_paths[0]);
}

static void missing_right_square_bracket(void)
{
    format_test_name("parsing/compounds/missing_right_square_bracket");

    format_source_path(0, "parsing/compounds/missing_right_square_bracket.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure Empty[A\n"
                          "end");

    SET_COMMAND_LINE("parsing/compounds/missing_right_square_bracket.owen");
    format_expectation(&test.expected_error, "%s:5:1: ] expected.\n", test.source_paths[0]);
}

static void missing_end_keyword(void)
{
    format_test_name("parsing/compounds/missing_end_keyword");

    format_source_path(0, "parsing/compounds/missing_end_keyword.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure Empty\n"
                          "    I32 a\n"
                          "function");

    SET_COMMAND_LINE("parsing/compounds/missing_end_keyword.owen");
    format_expectation(&test.expected_error, "%s:6:1: Missing end keyword in compound.\n", test.source_paths[0]);
}

static void invalid_field_type(void)
{
    format_test_name("parsing/compounds/invalid_field_type");

    format_source_path(0, "parsing/compounds/invalid_field_type.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure Empty\n"
                          "    i32 a\n"
                          "end");

    SET_COMMAND_LINE("parsing/compounds/invalid_field_type.owen");
    format_expectation(&test.expected_error, "%s:5:5: Field type expected.\n", test.source_paths[0]);
}

static void invalid_field_name(void)
{
    format_test_name("parsing/compounds/invalid_field_name");

    format_source_path(0, "parsing/compounds/invalid_field_name.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure Empty\n"
                          "    I32 A\n"
                          "end");

    SET_COMMAND_LINE("parsing/compounds/invalid_field_name.owen");
    format_expectation(&test.expected_error, "%s:5:9: Field name expected.\n", test.source_paths[0]);
}

bool parsing_compounds(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            empty_monomorphic_compound();
            break;

        case 1:
            empty_polymorphic_compound();
            break;

        case 2:
            missing_first_formal_type_parameter();
            break;

        case 3:
            missing_second_formal_type_parameter();
            break;

        case 4:
            missing_right_square_bracket();
            break;

        case 5:
            missing_end_keyword();
            break;

        case 6:
            invalid_field_type();
            break;

        case 7:
            invalid_field_name();
            break;

        default:
            return false;
    }

    return true;
}
