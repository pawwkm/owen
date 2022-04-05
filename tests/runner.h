#pragma once

#ifdef _WIN64
#define NEW_LINE "\r\n"
#else
#define NEW_LINE "\n"
#endif

#define MAX_PATH_LENGTH 255

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef bool (*Test_Iterator)(void);

#define MAX_OWEN_SOURCES 2
#define OUTPUT_LENGTH 6500

#define EXE_NAME "owen "
#define EXE_NAME_LENGTH 5

#define SET_COMMAND_LINE(COMMAND_LINE_OPTIONS) test.command_line = EXE_NAME COMMAND_LINE_OPTIONS

typedef struct
{
    char name[MAX_PATH_LENGTH];
    char* command_line;

    char source_paths[MAX_OWEN_SOURCES][MAX_PATH_LENGTH];

    char expected_error[OUTPUT_LENGTH];
    char expected_semantics[OUTPUT_LENGTH];
    char expected_ir[OUTPUT_LENGTH];
    char expected_pe[OUTPUT_LENGTH];

    int32_t expected_exit_code;
} Test;

typedef struct
{
    bool timed_out;
    char actual_error[OUTPUT_LENGTH];
    char actual_semantics[OUTPUT_LENGTH];
    char actual_ir[OUTPUT_LENGTH];
    char actual_pe[OUTPUT_LENGTH];

    int32_t actual_exit_code;
} Test_Result;

extern Test test;
extern Test_Result test_result;

// Platform dependent.
bool file_exists(const char (* file_name)[MAX_PATH_LENGTH]);
void delete_directory(const char (*directory)[MAX_PATH_LENGTH]);
void ensure_directory_exists(const char (*innermost_directory)[MAX_PATH_LENGTH]);
void platform_specific_initialization(void);
void run_test(void);

// Platform independent.
void format_test_name(const char* format, ...);
void format_source_path(uint8_t source_index, const char* format, ...);
void format_command_line_options(const char* format, ...);
void format_expectation(char (*expectation)[OUTPUT_LENGTH], const char* format, ...);
void format_source_file(uint8_t source_index, const char* format, ...);
const char* underline(size_t amount);
const char* space(size_t amount);

// Test_Iterator functions.
bool parsing(void);
bool parsing_compounds(void);
bool parsing_enumerations(void);
bool parsing_expressions_array_access(void);
bool parsing_expressions_array_initializer(void);
bool parsing_expressions_binary(void);
bool parsing_expressions_call(void);
bool parsing_expressions_field_access(void);
bool parsing_expressions_field_initializer(void);
bool parsing_expressions_reference(void);
bool parsing_expressions_sizeof(void);
bool parsing_functions(void);
bool parsing_namespaces(void);
bool parsing_statements_assignment(void);
bool parsing_statements_declaration(void);
bool parsing_statements_for(void);
bool parsing_statements_if(void);
bool parsing_statements_return(void);
bool parsing_statements_while(void);
bool parsing_tokens(void);
bool parsing_uses(void);
bool parsing_types(void);
bool semantics_compounds(void);
bool semantics_enumerations(void);
bool semantics_expressions_address_of(void);
bool semantics_expressions_array_access(void);
bool semantics_expressions_array_literal(void);
bool semantics_expressions_binary(void);
bool semantics_expressions_call_explicit_actual_type_parameters(void);
bool semantics_expressions_call_function_typed(void);
bool semantics_expressions_call_implicit_actual_type_parameters(void);
bool semantics_expressions_call_monomorphic(void);
bool semantics_expressions_cast(void);
bool semantics_expressions_compound_literal(void);
bool semantics_expressions_dereference(void);
bool semantics_expressions_enumeration_constant_access(void);
bool semantics_expressions_field_access(void);
bool semantics_expressions_integer_literal(void);
bool semantics_expressions_negate(void);
bool semantics_expressions_not(void);
bool semantics_expressions_null_literal(void);
bool semantics_expressions_parenthesized(void);
bool semantics_expressions_reference(void);
bool semantics_expressions_sizeof(void);
bool semantics_expressions_utf8_string_literal(void);
bool semantics_expressions_unicode_code_point_literal(void);
bool semantics_expressions_uninitialized_literal(void);
bool semantics_functions(void);
bool semantics_statements_assignment(void);
bool semantics_statements_break(void);
bool semantics_statements_continue(void);
bool semantics_statements_declaration(void);
bool semantics_statements_expression(void);
bool semantics_statements_for(void);
bool semantics_statements_if(void);
bool semantics_statements_return(void);
bool semantics_statements_while(void);

// Types and operators.
typedef enum
{
    Binary_Operator_Tag_logical_or            = 1 << 0,
    Binary_Operator_Tag_logical_and           = 1 << 1,
    Binary_Operator_Tag_equal_equal           = 1 << 2,
    Binary_Operator_Tag_not_equal             = 1 << 3,
    Binary_Operator_Tag_less_than_or_equal    = 1 << 4,
    Binary_Operator_Tag_greater_than_or_equal = 1 << 5,
    Binary_Operator_Tag_less_than             = 1 << 6,
    Binary_Operator_Tag_greater_than          = 1 << 7,
    Binary_Operator_Tag_plus                  = 1 << 8,
    Binary_Operator_Tag_minus                 = 1 << 9,
    Binary_Operator_Tag_bitwise_or            = 1 << 10,
    Binary_Operator_Tag_bitwise_xor           = 1 << 11,
    Binary_Operator_Tag_multiply              = 1 << 12,
    Binary_Operator_Tag_divide                = 1 << 13,
    Binary_Operator_Tag_modulo                = 1 << 14,
    Binary_Operator_Tag_bitwise_and           = 1 << 15,
    Binary_Operator_Tag_left_shift            = 1 << 16,
    Binary_Operator_Tag_right_shift           = 1 << 17
} Binary_Operator_Tag;

#define FIRST_BINARY_OPERATOR_TAG Binary_Operator_Tag_logical_or
#define LAST_BINARY_OPERATOR_TAG  Binary_Operator_Tag_right_shift

typedef struct
{
    Binary_Operator_Tag tag;
    char* token;
    bool is_relational;
    char* file_friendly_name;
} Binary_Operator;

#define BINARY_OPERATORS_LENGTH 18
extern Binary_Operator binary_operators[BINARY_OPERATORS_LENGTH];

uint8_t index_of_binary_operator(Binary_Operator_Tag tag);
Binary_Operator_Tag next_binary_operator(Binary_Operator_Tag tag);
bool is_binary_operator_defined_for_type(uint8_t type_index, Binary_Operator_Tag tag);
bool is_binary_operator_relational(Binary_Operator_Tag tag);

typedef struct
{
    char* name;
    char* file_friendly_name;
    char* default_value;
    char* file_friendly_default_value_name;
    char* default_value_name;

    char* min_value;
    char* max_value;

    char* underflow_value;
    char* overflow_value;

    uint8_t size_of;
    uint8_t allignment_of;
    
    Binary_Operator_Tag operators_defined_for_type;
} Type;

bool is_readable_type(uint8_t index);
bool is_writable_type(uint8_t index);
bool pointer_is_retained(uint8_t index);
bool pointer_type_is_compatible_with(uint8_t lhs_index, uint8_t rhs_index);

#define TYPES_LENGTH 33
#define LAST_TYPE_INDEX (TYPES_LENGTH - 1)
#define FIRST_SIGNED_NUMBER_TYPE_INDEX 1
#define LAST_SIGNED_NUMBER_TYPE_INDEX 4

#define I32_TYPE_INDEX 3

#define FIRST_UNSIGNED_NUMBER_TYPE_INDEX 5
#define LAST_UNSIGNED_NUMBER_TYPE_INDEX 8

#define FIRST_INTEGER_TYPE_INDEX 1
#define LAST_INTEGER_TYPE_INDEX 8

#define FIRST_FLOAT_TYPE_INDEX 9
#define LAST_FLOAT_TYPE_INDEX 10

#define FIRST_NUMBER_TYPE_INDEX 1
#define LAST_NUMBER_TYPE_INDEX 10

#define ENUMERATION_TYPE_INDEX 11

#define MONOMORPHIC_UNION_TYPE_INDEX 12
#define POLYMORPHIC_UNION_TYPE_INDEX 13
#define MONOMORPHIC_STRUCTURE_TYPE_INDEX 14
#define POLYMORPHIC_STRUCTURE_TYPE_INDEX 15
#define FIRST_COMPOUND_TYPE_INDEX 12
#define LAST_COMPOUND_TYPE_INDEX 15

#define FIRST_POINTER_TYPE_INDEX 16
#define LAST_POINTER_TYPE_INDEX 31

extern Type types[TYPES_LENGTH];

typedef enum
{
    Assignment_Operator_Tag_equal,
    Assignment_Operator_Tag_plus_equal,
    Assignment_Operator_Tag_minus_equal,
    Assignment_Operator_Tag_bitwise_or_equal,
    Assignment_Operator_Tag_bitwise_xor_equal,
    Assignment_Operator_Tag_multiply_equal,
    Assignment_Operator_Tag_divide_equal,
    Assignment_Operator_Tag_modulo_equal,
    Assignment_Operator_Tag_bitwise_and_equal,
    Assignment_Operator_Tag_left_shift_equal,
    Assignment_Operator_Tag_right_shift_equal
} Assignment_Operator_Tag;

#define FIRST_ASSIGNMENT_OPERATOR_TAG Assignment_Operator_Tag_equal
#define LAST_ASSIGNMENT_OPERATOR_TAG  Assignment_Operator_Tag_right_shift_equal

typedef struct
{
    char* token;
    char* file_friendly_name;
} Assignment_Operator;

extern Assignment_Operator assignment_operators[];

bool is_assignment_operator_defined_for_type(uint8_t type_index, Assignment_Operator_Tag tag);
