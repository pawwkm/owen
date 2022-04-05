#include "runner.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// ANSI forground colors
#define NORMAL  "\033[0m"
#define SUCCESS "\033[0;32m"
#define FAIL    "\033[0;31m"

static FILE* error_log;

#define DIRECTORIES_LENGTH (sizeof(directories) / sizeof(directories[0]))

Test test;
Test_Result test_result;

typedef struct
{
    char path[MAX_PATH_LENGTH];
    Test_Iterator next;
} Directory;

static const Directory directories[] =
{
    { "parsing/",                                                    parsing                                                    },
    { "parsing/compounds/",                                          parsing_compounds                                          },
    { "parsing/enumerations/",                                       parsing_enumerations                                       },
    { "parsing/expressions/array_access/",                           parsing_expressions_array_access                           },
    { "parsing/expressions/array_initializer/",                      parsing_expressions_array_initializer                      },
    { "parsing/expressions/binary/",                                 parsing_expressions_binary                                 },
    { "parsing/expressions/call/",                                   parsing_expressions_call                                   },
    { "parsing/expressions/field_access/",                           parsing_expressions_field_access                           },
    { "parsing/expressions/field_initializer/",                      parsing_expressions_field_initializer                      },
    { "parsing/expressions/reference/",                              parsing_expressions_reference                              },
    { "parsing/expressions/sizeof/",                                 parsing_expressions_sizeof                                 },
    { "parsing/functions/",                                          parsing_functions                                          },
    { "parsing/namespaces/",                                         parsing_namespaces                                         },
    { "parsing/statements/",                                         NULL                                                       },
    { "parsing/statements/assignment/",                              parsing_statements_assignment                              },
    { "parsing/statements/declaration/",                             parsing_statements_declaration                             },
    { "parsing/statements/for/",                                     parsing_statements_for                                     },
    { "parsing/statements/if/",                                      parsing_statements_if                                      },
    { "parsing/statements/return/",                                  parsing_statements_return                                  },
    { "parsing/statements/while/",                                   parsing_statements_while                                   },
    { "parsing/tokens/",                                             parsing_tokens                                             },
    { "parsing/uses/",                                               parsing_uses                                               },
    { "parsing/types/",                                              parsing_types                                              },
    { "semantics/",                                                  NULL                                                       },
    { "semantics/compounds/",                                        semantics_compounds                                        },
    { "semantics/enumerations/",                                     semantics_enumerations                                     },
    { "semantics/expressions/",                                      NULL                                                       },
    { "semantics/expressions/address_of/",                           semantics_expressions_address_of                           },
    { "semantics/expressions/array_access/",                         semantics_expressions_array_access                         },
    { "semantics/expressions/array_literal/",                        semantics_expressions_array_literal                        },
    { "semantics/expressions/binary/",                               semantics_expressions_binary                               },
    { "semantics/expressions/call/",                                 NULL                                                       },
    { "semantics/expressions/call/explicit_actual_type_parameters/", semantics_expressions_call_explicit_actual_type_parameters },
    { "semantics/expressions/call/function_typed/",                  semantics_expressions_call_function_typed                  },
    { "semantics/expressions/call/implicit_actual_type_parameters/", semantics_expressions_call_implicit_actual_type_parameters },
    { "semantics/expressions/call/monomorphic/",                     semantics_expressions_call_monomorphic                     },
    { "semantics/expressions/cast/",                                 semantics_expressions_cast                                 },
    { "semantics/expressions/compound_literal/",                     semantics_expressions_compound_literal                     },
    { "semantics/expressions/dereference/",                          semantics_expressions_dereference                          },
    { "semantics/expressions/enumeration_constant_access/",          semantics_expressions_enumeration_constant_access          },
    { "semantics/expressions/field_access/",                         semantics_expressions_field_access                         },
    { "semantics/expressions/integer_literal/",                      semantics_expressions_integer_literal                      },
    { "semantics/expressions/negate/",                               semantics_expressions_negate                               },
    { "semantics/expressions/not/",                                  semantics_expressions_not                                  },
    { "semantics/expressions/null_literal/",                         semantics_expressions_null_literal                         },
    { "semantics/expressions/parenthesized/",                        semantics_expressions_parenthesized                        },
    { "semantics/expressions/reference/",                            semantics_expressions_reference                            },
    { "semantics/expressions/sizeof/",                               semantics_expressions_sizeof                               },
    { "semantics/expressions/utf8_string_literal/",                  semantics_expressions_utf8_string_literal                  },
    { "semantics/expressions/unicode_code_point_literal/",           semantics_expressions_unicode_code_point_literal           },
    { "semantics/expressions/uninitialized_literal/",                semantics_expressions_uninitialized_literal                },
    { "semantics/functions/",                                        semantics_functions                                        },
    { "semantics/statements/",                                       NULL                                                       },
    { "semantics/statements/assignment/",                            semantics_statements_assignment                            },
    { "semantics/statements/break/",                                 semantics_statements_break                                 },
    { "semantics/statements/continue/",                              semantics_statements_continue                              },
    { "semantics/statements/declaration/",                           semantics_statements_declaration                           },
    { "semantics/statements/expression/",                            semantics_statements_expression                            },
    { "semantics/statements/for/",                                   semantics_statements_for                                   },
    { "semantics/statements/if/",                                    semantics_statements_if                                    },
    { "semantics/statements/return/",                                semantics_statements_return                                },
    { "semantics/statements/while/",                                 semantics_statements_while                                 },
};

void format_test_name(const char* format, ...)
{
    va_list arguments;
    va_start(arguments, format);

    int encoded_length = vsnprintf(test.name, MAX_PATH_LENGTH, format, arguments);
    if (encoded_length > MAX_PATH_LENGTH)
    {
        fprintf(stderr, "The test.name format \"%s\" exceeds MAX_PATH_LENGTH by %u\n", format, encoded_length - MAX_PATH_LENGTH);
        exit(EXIT_FAILURE);
    }
    else if (encoded_length < 0)
    {
        fprintf(stderr, "The test.name format \"%s\" could not be encoded.\n", format);
        exit(EXIT_FAILURE);
    }

    va_end(arguments);
}

void format_command_line_options(const char* format, ...)
{
    static char command_line[512];
    static uint16_t command_line_length;

    strcpy(command_line, EXE_NAME);
    command_line_length = EXE_NAME_LENGTH;

    va_list arguments;
    va_start(arguments, format);

    int encoded_length = vsnprintf(command_line + EXE_NAME_LENGTH, sizeof(command_line) - EXE_NAME_LENGTH, format, arguments);
    if (encoded_length > MAX_PATH_LENGTH)
    {
        fprintf(stderr, "The format \"%s\" exceeds command_line[512] by %zu\n", format, encoded_length - sizeof(command_line) - EXE_NAME_LENGTH);
        exit(EXIT_FAILURE);
    }
    else if (encoded_length < 0)
    {
        fprintf(stderr, "The format \"%s\" could not be encoded.\n", format);
        exit(EXIT_FAILURE);
    }

    va_end(arguments);

    test.command_line = command_line;
}

void format_source_path(uint8_t source_index, const char* format, ...)
{
    if (source_index + 1 > MAX_OWEN_SOURCES)
    {
        fprintf(stderr, "The format \"%s\" exceeds MAX_OWEN_SOURCES by %u\n", format, source_index + 1 - MAX_OWEN_SOURCES);
        exit(EXIT_FAILURE);
    }

    char (*path)[MAX_PATH_LENGTH] = &test.source_paths[source_index];

    va_list arguments;
    va_start(arguments, format);

    int encoded_length = vsnprintf(*path, MAX_PATH_LENGTH, format, arguments);
    if (encoded_length > MAX_PATH_LENGTH)
    {
        fprintf(stderr, "The format \"%s\" exceeds MAX_PATH_LENGTH by %u\n", format, encoded_length - MAX_PATH_LENGTH);
        exit(EXIT_FAILURE);
    }
    else if (encoded_length < 0)
    {
        fprintf(stderr, "The format \"%s\" could not be encoded.\n", format);
        exit(EXIT_FAILURE);
    }
    
    va_end(arguments);
}

void format_expectation(char (*expectation)[OUTPUT_LENGTH], const char* format, ...)
{
    va_list arguments;
    va_start(arguments, format);

    int encoded_length = vsnprintf(*expectation, OUTPUT_LENGTH, format, arguments);
    if (encoded_length > OUTPUT_LENGTH)
    {
        fprintf(stderr, "The format \"%s\" exceeds OUTPUT_LENGTH by %u\n", format, encoded_length - OUTPUT_LENGTH);
        exit(EXIT_FAILURE);
    }
    else if (encoded_length < 0)
    {
        fprintf(stderr, "The format \"%s\" could not be encoded.\n", format);
        exit(EXIT_FAILURE);
    }

    va_end(arguments);
}

void format_source_file(uint8_t source_index, const char* format, ...)
{
    if (source_index + 1 > MAX_OWEN_SOURCES)
    {
        fprintf(stderr, "The format \"%s\" exceeds MAX_OWEN_SOURCES by %u\n", format, source_index + 1 - MAX_OWEN_SOURCES);
        exit(EXIT_FAILURE);
    }

    char (*path)[MAX_PATH_LENGTH] = &test.source_paths[source_index];

    va_list arguments;
    va_start(arguments, format);

    if (file_exists(path))
    {
        // This makes sure that delete_previous_tests works and that
        // I don't accidentally overwrite a previously generated test 
        // during this run.
        fprintf(stderr, "File already exists %s\n", *path);
        exit(EXIT_FAILURE);
    }

    FILE* file = fopen(*path, "wb");
    if (!file)
    {
        fprintf(stderr, "Could not create %s\n", *path);
        exit(EXIT_FAILURE);
    }

    vfprintf(file, format, arguments);
    va_end(arguments);
    fclose(file);
}

const char* underline(size_t amount)
{
    // 32 ^ + null terminator.
    static char* characters = "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^";
    if (amount > 32)
    {
        fprintf(stderr, "Underline of 32 ^ exceeded.");
        exit(EXIT_FAILURE);
    }

    return characters + 32 - amount;
}

const char* space(size_t amount)
{
    // 32 spaces + null terminator.
    static char* characters = "                                ";
    if (amount > 32)
    {
        fprintf(stderr, "32 spaces exceeded.");
        exit(EXIT_FAILURE);
    }

    return characters + 32 - amount;
}

// TODO: Test harnes vs. directory?
static uint32_t tests_failed;
static uint32_t tests_succeeded;
static uint32_t tests_total;
static void compare_results_to_expectations(void)
{
    bool error_failed     = strcmp(test.expected_error,     test_result.actual_error);
    bool semantics_failed = strcmp(test.expected_semantics, test_result.actual_semantics);
    bool ir_failed        = strcmp(test.expected_ir,        test_result.actual_ir);
    bool pe_failed        = strcmp(test.expected_pe,        test_result.actual_pe);

    if (!error_failed && !semantics_failed && !ir_failed && !pe_failed)
    {
        tests_succeeded++;
        printf(SUCCESS "%s\n", test.name);
        return;
    }

    printf(            FAIL "%s\n", test.name);
    fprintf(error_log,      "%s\n", test.name);
    tests_failed++;

    if (error_failed)
    {
        if (test.expected_error[0] && !test_result.actual_error[0])
        {
            printf(            NORMAL "The compiler was expected to error but didn't:\n" FAIL "%s\n", test.expected_error);
            fprintf(error_log,        "The compiler was expected to error but didn't:\n"      "%s\n", test.expected_error);
        }
        else if (!test.expected_error[0] && test_result.actual_error[0])
        {
            printf(            NORMAL "Unexpected error:\n" FAIL "%s\n", test_result.actual_error);
            fprintf(error_log,        "Unexpected error:\n"      "%s\n", test_result.actual_error);
        }
        else
        {
            printf(            NORMAL "Expected error:\n" SUCCESS "%s\n" NORMAL "but found:\n" FAIL "%s\n", test.expected_error, test_result.actual_error);
            fprintf(error_log,        "Expected error:\n"         "%s\n"        "but found:\n"      "%s\n", test.expected_error, test_result.actual_error);
        }
    }
            
    if(semantics_failed)
    {
        if (test.expected_semantics[0] && !test_result.actual_semantics[0])
        {
            printf(            NORMAL "The compiler was expected to print semantics but didn't:\n" FAIL "%s\n", test.expected_semantics);
            fprintf(error_log,        "The compiler was expected to print semantics but didn't:\n"      "%s\n", test.expected_semantics);
        }
        else if (!test.expected_semantics[0] && test_result.actual_semantics[0])
        {
            printf(            NORMAL "Unexpected semantics:\n" FAIL "%s\n", test_result.actual_semantics);
            fprintf(error_log,        "Unexpected semantics:\n"      "%s\n", test_result.actual_semantics);
        }
        else
        {
            printf(            NORMAL "Expected semantics:\n" SUCCESS "%s\n" NORMAL "but found:\n" FAIL "%s\n", test.expected_semantics, test_result.actual_semantics);
            fprintf(error_log,        "Expected semantics:\n"         "%s\n"        "but found:\n"      "%s\n", test.expected_semantics, test_result.actual_semantics);
        }
    }
    
    if (ir_failed)
    {
        if (test.expected_ir[0] && !test_result.actual_ir[0])
        {
            printf(            NORMAL "The compiler was expected to print ir but didn't:\n" FAIL "%s\n", test.expected_ir);
            fprintf(error_log,        "The compiler was expected to print ir but didn't:\n"      "%s\n", test.expected_ir);
        }
        else if (!test.expected_ir[0] && test_result.actual_ir[0])
        {
            printf(            NORMAL "Unexpected ir:\n" FAIL "%s\n", test_result.actual_ir);
            fprintf(error_log,        "Unexpected ir:\n"      "%s\n", test_result.actual_ir);
        }
        else
        {
            printf(            NORMAL "Expected ir:\n" SUCCESS "%s\n" NORMAL "but found:\n" FAIL "%s\n", test.expected_ir, test_result.actual_ir);
            fprintf(error_log,        "Expected ir:\n"         "%s\n"        "but found:\n"      "%s\n", test.expected_ir, test_result.actual_ir);
        }
    }
            
    if (pe_failed)
    {
        if (test.expected_pe[0] && !test_result.actual_pe[0])
        {
            printf(            NORMAL "The compiler was expected to print a pe but didn't:\n" FAIL "%s\n", test.expected_pe);
            fprintf(error_log,        "The compiler was expected to print a pe but didn't:\n"      "%s\n", test.expected_pe);
        }
        else if (!test.expected_pe[0] && test_result.actual_pe[0])
        {
            printf(            NORMAL "Unexpected pe:\n" FAIL "%s\n", test_result.actual_pe);
            fprintf(error_log,        "Unexpected pe:\n"      "%s\n", test_result.actual_pe);
        }
        else
        {
            printf(            NORMAL "Expected pe:\n" SUCCESS "%s\n" NORMAL "but found:\n" FAIL "%s\n", test.expected_pe, test_result.actual_pe);
            fprintf(error_log,        "Expected pe:\n"         "%s\n"        "but found:\n"      "%s\n", test.expected_pe, test_result.actual_pe);
        }
    }
}

static void run_tests(const char* selected_directory)
{
    if (selected_directory)
    {
        bool directory_exists = false;
        for (uint8_t i = 0; i < DIRECTORIES_LENGTH; i++)
        {
            if (!strcmp(selected_directory, directories[i].path))
            {
                directory_exists = true;
                break;
            }
        }

        if (!directory_exists)
        {
            fprintf(stderr, "Invalid directory. Use one of:\n");
            for (uint8_t i = 0; i < DIRECTORIES_LENGTH; i++)
                fprintf(stderr, "    %s\n", directories[i].path);

            exit(EXIT_FAILURE);
        }
    }

    error_log = fopen("errors.log", "wb");
    if (!error_log)
    {
        fprintf(stderr, "Could not create errors.log file.\n");
        exit(EXIT_FAILURE);
    }

    for (uint8_t i = 0; i < DIRECTORIES_LENGTH; i++)
    {
        const Directory* directory = &directories[i];
        if (!directory->next || selected_directory && strstr(directory->path, selected_directory) != directory->path)
            continue;

        ensure_directory_exists(&directory->path);
        while (directory->next())
        {
            // TODO: Figure out how to use timeouts in run_test().
            run_test();
            compare_results_to_expectations();
            tests_total++;

            test = (Test){ 0 };
            test_result = (Test_Result){ 0 };
        }
    }

    fclose(error_log);
    printf(NORMAL "Total     "            "%u\n", tests_total);
    printf(NORMAL "Succeeded " SUCCESS    "%u\n", tests_succeeded);

    if (tests_failed)
        printf(NORMAL "Failed    "    FAIL    "%u\n", tests_failed);
    else
        printf(NORMAL "Failed    "    SUCCESS "%u\n", tests_failed);

    printf(NORMAL);
}

static void delete_previous_tests(void)
{
    for (uint8_t d = 0; d < DIRECTORIES_LENGTH; d++)
    {
        const Directory* directory = &directories[d];

        bool first_slash_occured = false;
        for (uint8_t c = 0; c < MAX_PATH_LENGTH; c++)
        {
            if (directory->path[c])
            {
                if (directory->path[c] == '/')
                {
                    if (first_slash_occured)
                        break;
                    else
                        first_slash_occured = true;
                }
            }
            else
            {
                delete_directory(&directory->path);
                break;
            }
        }
    }
}

int main(int argc, char* argv[])
{
    char* selected_directory = NULL;
    if (argc > 2)
    {
        fprintf(stderr, "usage: runner [directory]\n");
        for (uint8_t i = 0; i < DIRECTORIES_LENGTH; i++)
            fprintf(stderr, "    %s\n", directories[i].path);

        return EXIT_FAILURE;
    }
    else if (argc == 2)
        selected_directory = argv[1];

    platform_specific_initialization();
    delete_previous_tests();
    run_tests(selected_directory);

    return EXIT_SUCCESS;
}
