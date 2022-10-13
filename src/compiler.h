#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

#define FILE_CAPACITY                   100
#define TYPE_CAPACITY                   4000
#define TYPE_REFERENCE_CAPACITY         20000
#define NAMESPACE_CAPACITY              400
#define FUNCTION_CAPACITY               8000
#define FORMAL_PARAMETER_CAPACITY       20000
#define STATEMENT_CAPACITY              100000
#define BRANCH_CAPACITY                 8000
#define EXPRESSION_CAPACITY             300000
#define FIELD_CAPACITY                  2000
#define FIELD_INITIALIZER_CAPACITY      2000
#define CONSTANT_CAPACITY               2000
#define ELEMENT_INITIALIZER_CAPACITY    2000
#define VARIABLE_CAPACITY               2000
#define INTERNED_STRING_CAPACITY        16000
#define CHARACTER_CAPACITY              4000000

#define SYMBOL_CAPACITY                 32

#define IR_FUNCTION_CAPACITY            FUNCTION_CAPACITY
#define IR_BASIC_BLOCK_CAPACITY         40000
#define IR_INSTRUCTION_CAPACITY         100000
#define IR_OPERAND_CAPACITY             20000

#define TEXT_SECTION_CAPACITY           1000000

#define FILE_HANDLE_CAPACITY                FILE_CAPACITY
#define TYPE_HANDLE_CAPACITY                16000
#define TYPE_REFERENCE_HANDLE_CAPACITY      20000
#define NAMESPACE_HANDLE_CAPACITY           NAMESPACE_CAPACITY
#define FUNCTION_HANDLE_CAPACITY            8000
#define FORMAL_PARAMETER_HANDLE_CAPACITY    24000
#define STATEMENT_HANDLE_CAPACITY           200000
#define EXPRESSION_HANDLE_CAPACITY          100000
#define ELEMENT_INITIALIZER_HANDLE_CAPACITY 2000
#define FIELD_HANDLE_CAPACITY               FIELD_CAPACITY
#define FIELD_INITIALIZER_HANDLE_CAPACITY   2000
#define CONSTANT_HANDLE_CAPACITY            2000
#define BRANCH_HANDLE_CAPACITY              10000
#define VARIABLE_HANDLE_CAPACITY            VARIABLE_CAPACITY

#define IR_BASIC_BLOCK_HANDLE_CAPACITY      IR_BASIC_BLOCK_CAPACITY
#define IR_INSTRUCTION_HANDLE_CAPACITY      300000
#define IR_OPERAND_HANDLE_CAPACITY          30000

#define DEFINE_HANDLE_TYPE(SIZE, TYPE) \
typedef struct                         \
{                                      \
    SIZE index;                        \
} TYPE##_Handle;

typedef uint8_t Array_Size;
#define DEFINE_HANDLE_ARRAY_TYPE(TYPE) \
typedef struct                         \
{                                      \
    TYPE##_Handle* handles;            \
    Array_Size handles_length;         \
    Array_Size handles_capacity;       \
} TYPE##_Handle_Array;

DEFINE_HANDLE_TYPE(uint8_t,  File)
DEFINE_HANDLE_TYPE(uint16_t, Type)
DEFINE_HANDLE_TYPE(uint16_t, Type_Reference)
DEFINE_HANDLE_TYPE(uint16_t, Namespace)
DEFINE_HANDLE_TYPE(uint16_t, Function)
DEFINE_HANDLE_TYPE(uint16_t, Formal_Parameter)
DEFINE_HANDLE_TYPE(uint32_t, Statement)
DEFINE_HANDLE_TYPE(uint16_t, Variable)
DEFINE_HANDLE_TYPE(uint16_t, Branch)
DEFINE_HANDLE_TYPE(uint32_t, Expression)
DEFINE_HANDLE_TYPE(uint16_t, Field)
DEFINE_HANDLE_TYPE(uint16_t, Field_Initializer)
DEFINE_HANDLE_TYPE(uint16_t, Constant)
DEFINE_HANDLE_TYPE(uint16_t, Element_Initializer)
DEFINE_HANDLE_TYPE(uint16_t, Interned_String)
DEFINE_HANDLE_TYPE(uint16_t, Ir_Function)
DEFINE_HANDLE_TYPE(uint16_t, Ir_Basic_Block)
DEFINE_HANDLE_TYPE(uint32_t, Ir_Instruction)
DEFINE_HANDLE_TYPE(uint16_t, Ir_Immediate)
DEFINE_HANDLE_TYPE(uint16_t, Ir_Operand)

DEFINE_HANDLE_ARRAY_TYPE(File)
DEFINE_HANDLE_ARRAY_TYPE(Type)
DEFINE_HANDLE_ARRAY_TYPE(Type_Reference)
DEFINE_HANDLE_ARRAY_TYPE(Namespace)
DEFINE_HANDLE_ARRAY_TYPE(Function)
DEFINE_HANDLE_ARRAY_TYPE(Formal_Parameter)
DEFINE_HANDLE_ARRAY_TYPE(Statement)
DEFINE_HANDLE_ARRAY_TYPE(Variable)
DEFINE_HANDLE_ARRAY_TYPE(Branch)
DEFINE_HANDLE_ARRAY_TYPE(Expression)
DEFINE_HANDLE_ARRAY_TYPE(Field)
DEFINE_HANDLE_ARRAY_TYPE(Field_Initializer)
DEFINE_HANDLE_ARRAY_TYPE(Constant)
DEFINE_HANDLE_ARRAY_TYPE(Element_Initializer)
DEFINE_HANDLE_ARRAY_TYPE(Interned_String)
DEFINE_HANDLE_ARRAY_TYPE(Ir_Basic_Block)
DEFINE_HANDLE_ARRAY_TYPE(Ir_Instruction)
DEFINE_HANDLE_ARRAY_TYPE(Ir_Operand)

typedef enum
{
    Token_Tag_integer,
    Token_Tag_float,
    Token_Tag_upper_case_identifier,
    Token_Tag_lower_case_identifier,
    Token_Tag_namespace,
    Token_Tag_use,
    Token_Tag_public,
    Token_Tag_external,
    Token_Tag_function,
    Token_Tag_end,
    Token_Tag_if,
    Token_Tag_elif,
    Token_Tag_else,
    Token_Tag_for,
    Token_Tag_while,
    Token_Tag_break,
    Token_Tag_continue,
    Token_Tag_structure,
    Token_Tag_enumeration,
    Token_Tag_sizeof,
    Token_Tag_union,
    Token_Tag_return,
    Token_Tag_true,
    Token_Tag_false,
    Token_Tag_null,
    Token_Tag_version,
    Token_Tag_unicode_code_point,
    Token_Tag_utf8_string,
    Token_Tag_logical_or,
    Token_Tag_logical_and,
    Token_Tag_plus_equal,
    Token_Tag_minus_equal,
    Token_Tag_multiply_equal,
    Token_Tag_divide_equal,
    Token_Tag_bitwise_and_equal,
    Token_Tag_bitwise_or_equal,
    Token_Tag_bitwise_xor_equal,
    Token_Tag_modulo_equal,
    Token_Tag_left_shift_equal,
    Token_Tag_right_shift_equal,
    Token_Tag_equal,
    Token_Tag_equal_equal,
    Token_Tag_not_equal,
    Token_Tag_less_than_or_equal,
    Token_Tag_greater_than_or_equal,
    Token_Tag_less_than,
    Token_Tag_greater_than,
    Token_Tag_plus,
    Token_Tag_minus,
    Token_Tag_bitwise_or,
    Token_Tag_bitwise_xor,
    Token_Tag_multiply,
    Token_Tag_divide,
    Token_Tag_modulo,
    Token_Tag_bitwise_and,
    Token_Tag_left_shift,
    Token_Tag_right_shift,
    Token_Tag_exclamation_mark,
    Token_Tag_pound_sign,
    Token_Tag_dereference,
    Token_Tag_dot,
    Token_Tag_left_parentheses,
    Token_Tag_right_parentheses,
    Token_Tag_left_square_bracket,
    Token_Tag_right_square_bracket,
    Token_Tag_left_curly_bracket,
    Token_Tag_right_curly_bracket,
    Token_Tag_comma,
    Token_Tag_semicolon,
    Token_Tag_colon,
    Token_Tag_backtick,
    Token_Tag_question_mark,
    Token_Tag_dollar_sign,
    Token_Tag_uninitialized_literal,
    Token_Tag_blank_identifier,
    Token_Tag_eof
} Token_Tag;

typedef struct
{
    uint16_t line;
    uint16_t column;
} Position;

typedef struct
{
    Position start;
    Position end;
} Span;

typedef struct
{
    char* text;
    uint32_t length;
} Interned_String;

typedef struct
{
    char* text;
    uint32_t length;
} String;

typedef enum
{
    Type_Tag_i8,
    Type_Tag_i16,
    Type_Tag_i32,
    Type_Tag_i64,
    Type_Tag_u8,
    Type_Tag_u16,
    Type_Tag_u32,
    Type_Tag_u64,
    Type_Tag_f32,
    Type_Tag_f64,
    Type_Tag_bool,
    Type_Tag_enumeration,
    Type_Tag_compound,
    Type_Tag_function,
    Type_Tag_pointer,
    Type_Tag_dynamic_array,
    Type_Tag_fixed_array,
    Type_Tag_none,
    Type_Tag_tuple
} Type_Tag;

#define COMMON_TYPE_FIELDS  \
struct                      \
{                           \
    uint8_t tag;            \
    uint16_t allignment_of; \
    uint16_t size_of;       \
}

typedef struct
{
    COMMON_TYPE_FIELDS;
    Interned_String_Handle name;
    Span name_span;
} Primitive_Type;

typedef struct
{
    COMMON_TYPE_FIELDS;

    // This flag prevents function types from being looked up and refered to before they 
    // are fully constructed. This can cause infinite loops like this one.
    // Example: function a(Function() b) : Function()
    // caused this error: file.owen:5:12: Function() expected but found Function(Function(Fu..
    bool fully_constructed;
    
    Type_Handle_Array formal_parameters;
    Type_Handle_Array return_types;
} Function_Type;

typedef enum
{
    Compound_Attribute_is_union = 1,
    Compound_Attribute_is_polymorphic = 2,
    Compound_Attribute_is_public = 4
} Compound_Attribute;

typedef struct
{
    COMMON_TYPE_FIELDS;

    File_Handle file;
    Interned_String_Handle name;
    Span name_span;

    uint8_t attributes;

    union
    {
        // While the grammar defines this as a list of upper_case_identifier
        // it is easier implementation wise for lookups if it is
        // a Type_Reference. An example of this is: 
        // check_if_compound_has_duplicate_formal_type_parameters
        // 
        // It is guaranteed by the parser to be a upper_case_identifier. 
        Type_Reference_Handle_Array formal_type_parameters; // Is polymorphic

        struct // Not polymorphic && !is_invalid_type_handle(template)
        {
            Type_Handle_Array actual_type_parameters;
            Type_Handle template;
        };
    };

    Field_Handle_Array fields;
} Compound_Type;

typedef union
{
    int8_t i8;
    int16_t i16;
    int32_t i32;
    int64_t i64;
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    float f32;
    double f64;
} Number;

typedef struct
{
    Interned_String_Handle name;
    Span name_span;
    Expression_Handle explicit_value;
    Number value;
} Constant;

typedef struct
{
    COMMON_TYPE_FIELDS;

    File_Handle file;
    Interned_String_Handle name;
    Span name_span;
    bool is_public;
    
    union
    {
        Type_Handle underlying_type;
        
        // While the grammar defines this as a list of upper_case_identifier
        // it is easier implementation wise for lookups if it is
        // a Type_Reference. It is guaranteed by the parser to be a 
        // upper_case_identifier.
        Type_Reference_Handle underlying_type_reference;
    };

    Constant_Handle_Array constants;
} Enumeration_Type;

typedef struct
{
    COMMON_TYPE_FIELDS;
    Type_Handle_Array types;
} Tuple_Type;

typedef enum
{
    Pointer_Type_Privilege_retained = 1,
    Pointer_Type_Privilege_alias = 2,
    Pointer_Type_Privilege_readable = 4,
    Pointer_Type_Privilege_writable = 8
} Pointer_Type_Privilege;

#define ALL_PRIVILEGES (Pointer_Type_Privilege_retained | Pointer_Type_Privilege_alias | Pointer_Type_Privilege_readable | Pointer_Type_Privilege_writable)

typedef struct
{
    COMMON_TYPE_FIELDS;
    Type_Handle base_type;
    Pointer_Type_Privilege privileges;
} Pointer_Type;

typedef struct
{
    COMMON_TYPE_FIELDS;
    Type_Handle base_type;
    uint32_t size;
} Fixed_Array_Type;

typedef struct
{
    COMMON_TYPE_FIELDS;
    Type_Handle base_type;
} Dynamic_Array_Type;

typedef union
{
    COMMON_TYPE_FIELDS;
    Primitive_Type primitive;
    Function_Type function;
    Compound_Type compound;
    Enumeration_Type enumeration;
    Tuple_Type tuple;
    Pointer_Type pointer;
    Fixed_Array_Type fixed_array;
    Dynamic_Array_Type dynamic_array;
} Type;

typedef enum
{
    Type_Reference_Tag_name,
    Type_Reference_Tag_pointer,
    Type_Reference_Tag_dynamic_array,
    Type_Reference_Tag_fixed_array,
    Type_Reference_Tag_function,
    Type_Reference_Tag_polymorphic_compound
} Type_Reference_Tag;

#define COMMON_TYPE_REFERENCE_FIELDS \
struct                               \
{                                    \
    uint8_t tag;                     \
    Span span;                       \
}

typedef struct
{
    COMMON_TYPE_REFERENCE_FIELDS;
    Interned_String_Handle name;
} Named_Type_Reference;

typedef struct
{
    COMMON_TYPE_REFERENCE_FIELDS;
    Pointer_Type_Privilege privileges;

    // While the grammar defines this as a list of upper_case_identifier
    // it is easier implementation wise for lookups if it is
    // a Type_Reference. It is guaranteed by the parser to be a 
    // upper_case_identifier.
    Type_Reference_Handle base_type;
} Pointer_Type_Reference;

typedef struct
{
    COMMON_TYPE_REFERENCE_FIELDS;
    // While the grammar defines this as a list of upper_case_identifier
    // it is easier implementation wise for lookups if it is
    // a Type_Reference. It is guaranteed by the parser to be a 
    // upper_case_identifier.
    Type_Reference_Handle base_type;
} Dynamic_Array_Type_Reference;

typedef struct
{
    COMMON_TYPE_REFERENCE_FIELDS;
    String size;
    Span size_span;

    // While the grammar defines this as a list of upper_case_identifier
    // it is easier implementation wise for lookups if it is
    // a Type_Reference. It is guaranteed by the parser to be a 
    // upper_case_identifier.
    Type_Reference_Handle base_type;
} Fixed_Array_Type_Reference;

typedef struct
{
    COMMON_TYPE_REFERENCE_FIELDS;
    Type_Reference_Handle_Array formal_parameters;
    Type_Reference_Handle_Array return_types;
} Function_Type_Reference;

typedef struct
{
    COMMON_TYPE_REFERENCE_FIELDS;
    Interned_String_Handle name;
    Span name_span;
    Type_Reference_Handle_Array actual_type_parameters;
} Polymorphic_Compound_Type_Reference;

typedef union
{
    COMMON_TYPE_REFERENCE_FIELDS;
    Named_Type_Reference named;
    Pointer_Type_Reference pointer;
    Dynamic_Array_Type_Reference dynamic_array;
    Fixed_Array_Type_Reference fixed_array;
    Function_Type_Reference function;
    Polymorphic_Compound_Type_Reference polymorphic_compound;
} Type_Reference;

typedef enum
{
    Expression_Tag_reference,
    Expression_Tag_integer,
    Expression_Tag_float,
    Expression_Tag_true,
    Expression_Tag_false,
    Expression_Tag_unicode_code_point,
    Expression_Tag_call,
    Expression_Tag_binary,
    Expression_Tag_field_access,
    Expression_Tag_enumeration_constant_access,
    Expression_Tag_array_access,
    Expression_Tag_array_literal,
    Expression_Tag_parenthesized,
    Expression_Tag_compound_literal,
    Expression_Tag_fixed_array_length,
    Expression_Tag_dynamic_array_length,
    Expression_Tag_dynamic_array_capacity,
    Expression_Tag_dynamic_array_elements,
    Expression_Tag_cast,
    Expression_Tag_address_of,
    Expression_Tag_dereference,
    Expression_Tag_not,
    Expression_Tag_uninitialized_literal,
    Expression_Tag_negate,
    Expression_Tag_blank_identifier,
    Expression_Tag_null,
    Expression_Tag_utf8_string_literal,
    Expression_Tag_sizeof
} Expression_Tag;

typedef struct
{
    Expression_Handle explicit_index;
    uint32_t index;
    Expression_Handle expression;
} Element_Initializer;

#define COMMON_EXPRESSION_FIELDS \
struct                           \
{                                \
    uint8_t tag;                 \
    Type_Handle type;            \
    Span span;                   \
}

typedef struct
{
    COMMON_EXPRESSION_FIELDS;
    Interned_String_Handle name;
    Type_Reference_Handle_Array actual_type_parameters;
} Reference;

typedef struct
{
    COMMON_EXPRESSION_FIELDS;
    uint32_t code_point;
} Unicode_Code_Point_Literal;

typedef struct
{
    COMMON_EXPRESSION_FIELDS;
    Interned_String_Handle string;
} Utf8_String_Literal;

typedef struct
{
    COMMON_EXPRESSION_FIELDS;
    String string;
} Unchecked_Number;

typedef enum
{
    Binary_Operator_logical_or = Token_Tag_logical_or,
    Binary_Operator_logical_and,
    Binary_Operator_equal_equal = Token_Tag_equal_equal,
    Binary_Operator_not_equal,
    Binary_Operator_less_than_or_equal,
    Binary_Operator_greater_than_or_equal,
    Binary_Operator_less_than,
    Binary_Operator_greater_than,
    Binary_Operator_plus,
    Binary_Operator_minus,
    Binary_Operator_bitwise_or,
    Binary_Operator_bitwise_xor,
    Binary_Operator_multiply,
    Binary_Operator_divide,
    Binary_Operator_modulo,
    Binary_Operator_bitwise_and,
    Binary_Operator_left_shift,
    Binary_Operator_right_shift,
} Binary_Operator;

typedef struct
{
    COMMON_EXPRESSION_FIELDS;
    uint8_t operator;
    Span operator_span;
    Expression_Handle lhs;
    Expression_Handle rhs;
} Binary;

typedef struct
{
    COMMON_EXPRESSION_FIELDS;
    Field_Initializer_Handle_Array field_initializers;
} Compound_Literal;

typedef struct
{
    COMMON_EXPRESSION_FIELDS;

    // While the grammar defines this as a list of upper_case_identifier
    // it is easier implementation wise for lookups if it is
    // a Type_Reference. It is guaranteed by the parser to be a 
    // upper_case_identifier.
    Type_Reference_Handle enumeration;
    Interned_String_Handle constant;
    Span constant_span;
} Enumeration_Constant_Access;

typedef struct
{
    COMMON_EXPRESSION_FIELDS;
    Expression_Handle compound;
    Interned_String_Handle field;
    Span field_span;
} Field_Access;

typedef struct
{
    COMMON_EXPRESSION_FIELDS;
    Expression_Handle callee;
    Function_Handle callee_declaration;
    Expression_Handle_Array actual_parameters;
} Call;

typedef struct
{
    COMMON_EXPRESSION_FIELDS;
    Element_Initializer_Handle_Array elements;
} Array_Literal;

typedef struct
{
    COMMON_EXPRESSION_FIELDS;
    Expression_Handle array;
    Expression_Handle index;
} Array_Access;

typedef struct
{
    COMMON_EXPRESSION_FIELDS;
    Expression_Handle expression;
} Parenthesized;

typedef struct
{
    COMMON_EXPRESSION_FIELDS;
    Expression_Handle source;
    Type_Reference_Handle target;
} Cast;

typedef struct
{
    COMMON_EXPRESSION_FIELDS;
    Expression_Handle expression;
} Address_Of;

typedef struct
{
    COMMON_EXPRESSION_FIELDS;
    Expression_Handle expression;
} Dereference;

typedef struct
{
    COMMON_EXPRESSION_FIELDS;
    Expression_Handle expression;
} Not;

typedef struct
{
    COMMON_EXPRESSION_FIELDS;
    Expression_Handle expression;
} Negate;

typedef struct
{
    COMMON_EXPRESSION_FIELDS;
    Number value;
} Number_Literal;

typedef struct
{
    COMMON_EXPRESSION_FIELDS;
    Type_Reference_Handle t;
    Expression_Handle expression;
} Size_Of;

typedef union
{
    COMMON_EXPRESSION_FIELDS;
    Reference reference;
    Unchecked_Number unchecked_number;
    Unicode_Code_Point_Literal code_point;
    Utf8_String_Literal string_literal;
    Binary binary;
    Compound_Literal compound_literal;
    Enumeration_Constant_Access enumeration_constant_access;
    Field_Access field_access;
    Call call;
    Array_Literal array_literal;
    Array_Access array_access;
    Parenthesized parenthesized;
    Cast cast;
    Address_Of address_of;
    Dereference dereference;
    Not not;
    Negate negate;
    Number_Literal number;
    Size_Of size_of;
} Expression;

typedef struct 
{
    Interned_String_Handle field;
    Span field_span;
    Expression_Handle expression;
} Field_Initializer;

typedef enum
{
    Statement_Tag_declaration,
    Statement_Tag_assignment,
    Statement_Tag_expression,
    Statement_Tag_if,
    Statement_Tag_loop,
    Statement_Tag_break,
    Statement_Tag_continue,
    Statement_Tag_return
} Statement_Tag;

typedef struct
{
    Interned_String_Handle name;
    Span name_span;
    bool is_blank;

    union
    {
        Type_Reference_Handle type_reference;
        Type_Handle type;
    };
} Variable;

typedef struct
{
    Variable_Handle_Array variables;
    Expression_Handle_Array expressions;
} Declaration_Statement;

typedef enum
{
    Assignment_Operator_plus_equal = Token_Tag_plus_equal,
    Assignment_Operator_minus_equal,
    Assignment_Operator_multiply_equal,
    Assignment_Operator_divide_equal,
    Assignment_Operator_bitwise_and_equal,
    Assignment_Operator_bitwise_or_equal,
    Assignment_Operator_bitwise_xor_equal,
    Assignment_Operator_modulo_equal,
    Assignment_Operator_left_shift_equal,
    Assignment_Operator_right_shift_equal,
    Assignment_Operator_equal
} Assignment_Operator;

typedef struct
{
    Expression_Handle_Array lhs;
    Assignment_Operator operator;
    Span operator_span;
    Expression_Handle_Array rhs;
} Assignment_Statement;

typedef struct
{
    Statement_Handle declaration_statement;
    Expression_Handle condition;
    Statement_Handle_Array body;
} Branch;

typedef struct
{
    Branch_Handle_Array branches;
} If_Statement;

typedef struct
{
    Statement_Handle declaration_statement;
    Expression_Handle condition;
    Statement_Handle assignment_statement;
    Statement_Handle_Array body;
} Loop_Statement;

typedef struct
{
    Span span;
} Break_Statement;

typedef struct
{
    Span span;
} Continue_Statement;

typedef struct
{
    Expression_Handle_Array values;
    Span span;
} Return_Statement;

typedef struct
{
    uint8_t tag;
    union
    {
        Declaration_Statement declaration_statement;
        Assignment_Statement assignment_statement;
        Expression_Handle expression_statement;        
        If_Statement if_statement;
        Loop_Statement loop_statement;
        Continue_Statement continue_statement;
        Break_Statement break_statement;
        Return_Statement return_statement;
    };
} Statement;

typedef struct
{
    Type_Handle type;
    Type_Reference_Handle type_reference;
    Interned_String_Handle name;
    Span name_span;
} Field;

typedef struct
{
    Type_Reference_Handle type;
    Interned_String_Handle name;
    Span name_span;
} Formal_Parameter;

typedef enum
{
    Function_Attribute_is_public = 1,
    Function_Attribute_is_polymorphic = 2,
} Function_Attribute;

typedef struct
{
    File_Handle file;
    Interned_String_Handle name;
    Span name_span;
    uint8_t attributes;

    union
    {
        // While the grammar defines this as a list of upper_case_identifier
        // it is easier implementation wise for lookups if it is
        // a Type_Reference. An example of this is: 
        // check_if_signatures_formal_type_parameters_matches_any_types
        // 
        // It is guaranteed by the parser to be a upper_case_identifier. 
        Type_Reference_Handle_Array formal_type_parameters; // is_polymorphic == true
        
        struct // is_polymorphic == false && template
        {
            Type_Handle_Array actual_type_parameters;
            Function_Handle template;
        };
    };

    Formal_Parameter_Handle_Array formal_parameters;
    Type_Reference_Handle_Array return_types;
    Type_Handle signature;
    Statement_Handle_Array body;
    Ir_Function_Handle ir;
} Function;

typedef struct
{
    Interned_String_Handle name;
    Span name_span;
} Namespace;

typedef struct
{
    char* path;

    Namespace_Handle namespace;
    Namespace_Handle_Array uses;
    Type_Handle_Array type_declarations;
    Type_Handle_Array types_in_scope;
    Function_Handle_Array function_declarations;
    Function_Handle_Array functions_in_scope;
} File;

typedef enum
{
    Ir_Operand_Tag_x64_rax,
    Ir_Operand_Tag_x64_rcx,
    Ir_Operand_Tag_x64_rdx,
    Ir_Operand_Tag_x64_rbx,
    Ir_Operand_Tag_x64_sp,
    Ir_Operand_Tag_x64_bp,
    Ir_Operand_Tag_x64_si,
    Ir_Operand_Tag_x64_r8,
    Ir_Operand_Tag_x64_r9,
    Ir_Operand_Tag_x64_r10,
    Ir_Operand_Tag_x64_r11,
    Ir_Operand_Tag_x64_r12,
    Ir_Operand_Tag_x64_r13,
    Ir_Operand_Tag_x64_r14,
    Ir_Operand_Tag_x64_r15,

    Ir_Operand_Tag_immediate,
    Ir_Operand_Tag_vreg,
    Ir_Operand_Tag_stack_slot,
    Ir_Operand_Tag_parameter
} Ir_Operand_Tag;

#define IS_X64_REG(TAG) (TAG >= Ir_Operand_Tag_x64_rax && TAG <= Ir_Operand_Tag_x64_r15)

typedef enum
{
    Ir_Tag_store,
    Ir_Tag_call,
    Ir_Tag_return,
    Ir_Tag_x64_mov,
    Ir_Tag_x64_xor,
    Ir_Tag_x64_call,
    Ir_Tag_x64_ret
} Ir_Tag;

#define COMMON_IR_FIELDS             \
struct                               \
{                                    \
    uint8_t tag;                     \
    Type_Handle type;                \
    Ir_Operand_Handle_Array sources; \
    Ir_Operand_Handle destination;   \
}

#define COMMON_IR_OPERAND_FIELDS \
struct                           \
{                                \
    uint8_t tag;                 \
}

typedef struct
{
    COMMON_IR_OPERAND_FIELDS;
    Number value;
} Ir_Immediate;

typedef struct
{
    COMMON_IR_OPERAND_FIELDS;
    Ir_Instruction_Handle ir;
} Ir_Virtual_Register;

typedef enum
{
    Ir_Stack_Slot_Tag_formal_parameter
} Ir_Stack_Slot_Tag;

typedef struct
{
    COMMON_IR_OPERAND_FIELDS;
    uint8_t stack_tag;
    int32_t offset;
} Ir_Stack_Slot;

typedef struct
{
    COMMON_IR_OPERAND_FIELDS;
    uint8_t index;
} Ir_Parameter;

typedef union
{
    COMMON_IR_OPERAND_FIELDS;
    Ir_Immediate imm;
    Ir_Virtual_Register vreg;
    Ir_Stack_Slot stack;
    Ir_Parameter parameter;
} Ir_Operand;

typedef struct
{
    COMMON_IR_FIELDS;
    Type_Handle callee_type;
    Function_Handle callee_declaration;
} Ir_Call_Instruction;

typedef struct
{
    COMMON_IR_FIELDS;
    Function_Handle callee_declaration;
    uint32_t absolute_address;
} Ir_X64_Call;

typedef union
{
    COMMON_IR_FIELDS;
    Ir_Call_Instruction call;
    Ir_X64_Call x64_call;
} Ir_Instruction;

typedef struct
{
    Interned_String_Handle name;
    Ir_Operand_Handle value;
} Ir_Definition;

typedef struct
{
    uint16_t size_of_ir_in_bytes;
    Ir_Instruction_Handle_Array ir;
    Ir_Definition definitions[SYMBOL_CAPACITY];
    uint8_t definitions_length;
} Ir_Basic_Block;

typedef struct
{
    uint32_t absolute_address;
    uint16_t size_of_blocks_in_bytes;
    Ir_Basic_Block_Handle_Array blocks;
} Ir_Function;

typedef struct
{
    bool print_timings;
    bool print_semantics;
    bool print_version;
    bool print_memory;
    char* destination;
} Options;

extern Options options;

extern Type_Handle i8_handle;
extern Type_Handle i16_handle;
extern Type_Handle i32_handle;
extern Type_Handle i64_handle;
extern Type_Handle u8_handle;
extern Type_Handle u16_handle;
extern Type_Handle u32_handle;
extern Type_Handle u64_handle;
extern Type_Handle f32_handle;
extern Type_Handle f64_handle;
extern Type_Handle bool_handle;
extern Type_Handle none_handle;
extern Type_Handle utf8_string_literal_type_handle;

extern const Type_Handle invalid_type_handle;

typedef enum
{
    Timer_file_reading,
    Timer_lexing,
    Timer_parsing,
    Timer_analysis,
    Timer_optimization,
    Timer_code_generation,
    Timer_file_writing
} Timer;

void start_timer(Timer timer);
void stop_current_timer(void);
void print_timings(void);
void print_memory(void);

void not_implemented(const char* path, uint32_t line, const char* thing);
void out_of_memory(const char* path, uint32_t line, const char* type);
void ice(const char* path, uint32_t line, const char* message);
void unexpected_expression(const char* path, uint32_t line, uint8_t tag);
void unexpected_reference_type(const char* path, uint32_t line, uint8_t tag);
void unexpected_type(const char* path, uint32_t line, uint8_t tag);
void unexpected_statement(const char* path, uint32_t line, uint8_t tag);
void unexpected_token(const char* path, uint32_t line, uint8_t tag);
void unexpected_ir_instruction(const char* path, uint32_t line, uint8_t tag);
void print_error(const char* message, ...);

#define POLYMORPHIC_STACK_TYPES_CAPACITY 8
typedef struct
{
    Array_Size length;
    Type_Reference_Handle polymorphic_types[POLYMORPHIC_STACK_TYPES_CAPACITY];
    Type_Handle monomorphic_types[POLYMORPHIC_STACK_TYPES_CAPACITY];
    
    const File* file;

    Span origin;
    Compound_Type* monomorphisized_compound;
    Function* monomorphisized_function;
} Polymorphic_Type_Mapping;

#define POLYMORPHIC_STACK_CAPACITY 8
extern Polymorphic_Type_Mapping* current_polymorphic_type_mapping;
extern Polymorphic_Type_Mapping polymorphic_type_mappings[];
extern uint8_t polymorphic_type_mappings_length;
extern uint8_t deepest_polymorphic_type_mapping;

typedef struct
{
    Interned_String_Handle name;
    Span name_span;
    Type_Handle type;
    bool is_referenced;
} Symbol;

typedef struct
{
    Symbol symbols[SYMBOL_CAPACITY];
    uint8_t symbols_length;
} Symbol_Table;

extern Symbol_Table* current_symbol_table;
extern uint8_t deepest_symbol_table;
void add_symbol(Interned_String_Handle name, Span name_span, Type_Handle type);
void acquire_symbol_table(void);
void release_symbol_table(void);

void add_definition(Ir_Basic_Block* block, Ir_Definition definition);

void acquire_polymorphic_type_mapping(void);
void release_polymorphic_type_mapping(void);

File* file_of_type(const Type* type);
Position start_of_type(const Type* type);

void print_declaration_header(const File* file, Position position, const char* message, ...);
void print_function_declaration(const Function* function);
void print_type_declaration(Type_Handle type_handle);
void exit_with_error_code(void);

void print_span_error(const File* file, Span span, const char* message, ...);

void print_string(String view, FILE* file);
void print_string_by_handle(Interned_String_Handle view, FILE* file);
void print_type_reference_for_tuple(const Tuple_Type* tuple, FILE* file);
void print_type_reference_for_type (Type_Handle type_handle, FILE* file);
void print_type_reference_for_compound(const Compound_Type* compound, FILE* file);

void parse_file(char* path);
void analyze_program(void);
void lower_ast(void);
void ir_abi_pass(void);
void ir_x64_pass(void);
uint16_t size_of_x64_ir(const Ir_Instruction* inst);
void generate_pe(void);

Type_Handle lookup_signature(const File* file, const Function* function);
Field* lookup_field_by_name(const Compound_Type* compound, Interned_String_Handle name, Span name_span);
Constant* lookup_constant_by_name(const Enumeration_Type* enumeration, Interned_String_Handle name, Span name_span);
Type_Handle lookup_type_by_reference(const File* file, Type_Reference_Handle reference_handle, bool print_error_if_reference_is_undefined);
Type_Handle lookup_tuple_type_by_signature(Type_Handle signature_handle);

#define DECLARE_DEFAULT_ARENA(TYPE, LOWER_CASE_TYPE, LOWER_CASE_TYPE_PLURAL, UINT_X)                          \
extern TYPE LOWER_CASE_TYPE_PLURAL[];                                                                         \
extern UINT_X LOWER_CASE_TYPE_PLURAL##_length;                                                                \
                                                                                                              \
extern const TYPE##_Handle invalid_##LOWER_CASE_TYPE##_handle;                                                \
extern TYPE##_Handle LOWER_CASE_TYPE##_handles[];                                                             \
extern UINT_X LOWER_CASE_TYPE##_handles_length;                                                               \
                                                                                                              \
TYPE##_Handle add_##LOWER_CASE_TYPE(void);                                                                    \
TYPE* lookup_##LOWER_CASE_TYPE(TYPE##_Handle handle);                                                         \
bool is_invalid_##LOWER_CASE_TYPE##_handle(TYPE##_Handle handle);                                             \
bool compare_##LOWER_CASE_TYPE_PLURAL(TYPE##_Handle a, TYPE##_Handle b);                                      \
void add_to_##LOWER_CASE_TYPE##_array(TYPE##_Handle_Array* array, TYPE##_Handle handle);                      \
void insert_##LOWER_CASE_TYPE##_in_array(TYPE##_Handle_Array* array, TYPE##_Handle handle, Array_Size index); \
void remove_##LOWER_CASE_TYPE##_from_array(TYPE##_Handle_Array* array, Array_Size index);                     \
void reserve_##LOWER_CASE_TYPE##_handles(TYPE##_Handle_Array* array, Array_Size amount);                      \
TYPE##_Handle LOWER_CASE_TYPE##_handle_at(const TYPE##_Handle_Array* array, Array_Size index);                \
void replace_##LOWER_CASE_TYPE(TYPE##_Handle_Array* array, TYPE##_Handle handle, Array_Size index);           \
TYPE* lookup_##LOWER_CASE_TYPE##_in(const TYPE##_Handle_Array* array, Array_Size index);

DECLARE_DEFAULT_ARENA(File,                file,                files,                uint8_t )
DECLARE_DEFAULT_ARENA(Type,                type,                types,                uint16_t)
DECLARE_DEFAULT_ARENA(Type_Reference,      type_reference,      type_references,      uint16_t)
DECLARE_DEFAULT_ARENA(Namespace,           namespace,           namespaces,           uint16_t)
DECLARE_DEFAULT_ARENA(Function,            function,            functions,            uint16_t)
DECLARE_DEFAULT_ARENA(Formal_Parameter,    formal_parameter,    formal_parameters,    uint16_t)
DECLARE_DEFAULT_ARENA(Statement,           statement,           statements,           uint32_t)
DECLARE_DEFAULT_ARENA(Branch,              branch,              branches,             uint16_t)
DECLARE_DEFAULT_ARENA(Expression,          expression,          expressions,          uint32_t)
DECLARE_DEFAULT_ARENA(Field,               field,               fields,               uint16_t) 
DECLARE_DEFAULT_ARENA(Field_Initializer,   field_initializer,   field_initializers,   uint16_t)
DECLARE_DEFAULT_ARENA(Constant,            constant,            constants,            uint16_t)
DECLARE_DEFAULT_ARENA(Element_Initializer, element_initializer, element_initializers, uint16_t)
DECLARE_DEFAULT_ARENA(Variable,            variable,            variables,            uint16_t)

DECLARE_DEFAULT_ARENA(Ir_Basic_Block,      ir_basic_block,      ir_basic_blocks,      uint16_t)
DECLARE_DEFAULT_ARENA(Ir_Instruction,      ir_instruction,      ir_instructions,      uint32_t)
DECLARE_DEFAULT_ARENA(Ir_Operand,          ir_operand,          ir_operands,          uint16_t)

extern Ir_Function ir_functions[];
extern uint16_t ir_functions_length;
extern const Ir_Function_Handle invalid_ir_function_handle;
extern Ir_Function_Handle ir_function_handles[];
extern uint16_t ir_function_handles_length;
Ir_Function_Handle add_ir_function(void);
Ir_Function* lookup_ir_function(Ir_Function_Handle handle);
bool is_invalid_ir_function_handle(Ir_Function_Handle handle);
bool compare_ir_functions(Ir_Function_Handle a, Ir_Function_Handle b);

extern const Function_Handle invalid_function_handle;
extern Function_Handle main_function;

extern File files[];
extern uint8_t files_length;
File* lookup_file(File_Handle handle);

extern char characters[];
extern uint32_t characters_length;

extern Symbol_Table symbol_tables[];
extern uint8_t symbol_tables_length;

bool is_public_type(Type_Handle handle);
void print_semantics(void);
void print_ir(void);

extern Interned_String interned_strings[];
extern uint16_t interned_strings_length;
extern const Interned_String_Handle invalid_interned_string_handle;
Interned_String* lookup_interned_string(Interned_String_Handle handle);
bool compare_interned_strings(Interned_String_Handle a, Interned_String_Handle b);
bool compare_strings(String a, String b);
Interned_String_Handle add_interned_string(String string);

#define lookup(HANDLE) _Generic((HANDLE),                   \
    File_Handle:                lookup_file,                \
    Type_Handle:                lookup_type,                \
    Type_Reference_Handle:      lookup_type_reference,      \
    Namespace_Handle:           lookup_namespace,           \
    Function_Handle:            lookup_function,            \
    Formal_Parameter_Handle:    lookup_formal_parameter,    \
    Statement_Handle:           lookup_statement,           \
    Variable_Handle:            lookup_variable,            \
    Branch_Handle:              lookup_branch,              \
    Expression_Handle:          lookup_expression,          \
    Field_Handle:               lookup_field,               \
    Field_Initializer_Handle:   lookup_field_initializer,   \
    Constant_Handle:            lookup_constant,            \
    Element_Initializer_Handle: lookup_element_initializer, \
    Interned_String_Handle:     lookup_interned_string,     \
    Ir_Function_Handle:         lookup_ir_function,         \
    Ir_Basic_Block_Handle:      lookup_ir_basic_block,      \
    Ir_Instruction_Handle:      lookup_ir_instruction,      \
    Ir_Operand_Handle:          lookup_ir_operand)(HANDLE)

#define invalid(HANDLE) _Generic((HANDLE),                              \
    File_Handle:                is_invalid_file_handle,                \
    Type_Handle:                is_invalid_type_handle,                \
    Type_Reference_Handle:      is_invalid_type_reference_handle,      \
    Namespace_Handle:           is_invalid_namespace_handle,           \
    Function_Handle:            is_invalid_function_handle,            \
    Formal_Parameter_Handle:    is_invalid_formal_parameter_handle,    \
    Statement_Handle:           is_invalid_statement_handle,           \
    Variable_Handle:            is_invalid_variable_handle,            \
    Branch_Handle:              is_invalid_branch_handle,              \
    Expression_Handle:          is_invalid_expression_handle,          \
    Field_Handle:               is_invalid_field_handle,               \
    Field_Initializer_Handle:   is_invalid_field_initializer_handle,   \
    Constant_Handle:            is_invalid_constant_handle,            \
    Element_Initializer_Handle: is_invalid_element_initializer_handle, \
    Ir_Function_Handle:         is_invalid_ir_function_handle,         \
    Ir_Basic_Block_Handle:      is_invalid_ir_basic_block_handle,      \
    Ir_Instruction_Handle:      is_invalid_ir_instruction_handle,      \
    Ir_Operand_Handle:          is_invalid_ir_operand_handle)(HANDLE)

#define compare(A, B) _Generic((A),                            \
    File_Handle:                compare_files,                \
    Type_Handle:                compare_types,                \
    Type_Reference_Handle:      compare_type_references,      \
    Namespace_Handle:           compare_namespaces,           \
    Function_Handle:            compare_functions,            \
    Formal_Parameter_Handle:    compare_formal_parameters,    \
    Statement_Handle:           compare_statements,           \
    Variable_Handle:            compare_variables,            \
    Branch_Handle:              compare_branches,             \
    Expression_Handle:          compare_expressions,          \
    Field_Handle:               compare_fields,               \
    Field_Initializer_Handle:   compare_field_initializers,   \
    Constant_Handle:            compare_constants,            \
    Element_Initializer_Handle: compare_element_initializers, \
    Interned_String_Handle:     compare_interned_strings,     \
    String:                     compare_strings,              \
    Ir_Function_Handle:         compare_ir_functions,         \
    Ir_Basic_Block_Handle:      compare_ir_basic_blocks,      \
    Ir_Instruction_Handle:      compare_ir_instructions,      \
    Ir_Operand_Handle:          compare_ir_operands)(A, B)

#define add_to(ARRAY, HANDLE) _Generic((ARRAY),                                \
    File_Handle_Array*:                add_to_file_array,                      \
    Type_Handle_Array*:                add_to_type_array,                      \
    Type_Reference_Handle_Array*:      add_to_type_reference_array,            \
    Namespace_Handle_Array*:           add_to_namespace_array,                 \
    Function_Handle_Array*:            add_to_function_array,                  \
    Formal_Parameter_Handle_Array*:    add_to_formal_parameter_array,          \
    Statement_Handle_Array*:           add_to_statement_array,                 \
    Variable_Handle_Array*:            add_to_variable_array,                  \
    Branch_Handle_Array*:              add_to_branch_array,                    \
    Expression_Handle_Array*:          add_to_expression_array,                \
    Field_Handle_Array*:               add_to_field_array,                     \
    Field_Initializer_Handle_Array*:   add_to_field_initializer_array,         \
    Constant_Handle_Array*:            add_to_constant_array,                  \
    Element_Initializer_Handle_Array*: add_to_element_initializer_array,       \
    Ir_Basic_Block_Handle_Array*:      add_to_ir_basic_block_array,            \
    Ir_Instruction_Handle_Array*:      add_to_ir_instruction_array,            \
    Ir_Operand_Handle_Array*:          add_to_ir_operand_array)(ARRAY, HANDLE)

#define insert(ARRAY, HANDLE, INDEX) _Generic((ARRAY),                                   \
    File_Handle_Array*:                insert_file_in_array,                             \
    Type_Handle_Array*:                insert_type_in_array,                             \
    Type_Reference_Handle_Array*:      insert_type_reference_in_array,                   \
    Namespace_Handle_Array*:           insert_namespace_in_array,                        \
    Function_Handle_Array*:            insert_function_in_array,                         \
    Formal_Parameter_Handle_Array*:    insert_formal_parameter_in_array,                 \
    Statement_Handle_Array*:           insert_statement_in_array,                        \
    Variable_Handle_Array*:            insert_variable_in_array,                         \
    Branch_Handle_Array*:              insert_branch_in_array,                           \
    Expression_Handle_Array*:          insert_expression_in_array,                       \
    Field_Handle_Array*:               insert_field_in_array,                            \
    Field_Initializer_Handle_Array*:   insert_field_initializer_in_array,                \
    Constant_Handle_Array*:            insert_constant_in_array,                         \
    Element_Initializer_Handle_Array*: insert_element_initializer_in_array,              \
    Ir_Basic_Block_Handle_Array*:      insert_ir_basic_block_in_array,                   \
    Ir_Instruction_Handle_Array*:      insert_ir_instruction_in_array,                   \
    Ir_Operand_Handle_Array*:          insert_ir_operand_in_array)(ARRAY, HANDLE, INDEX)

#define remove(ARRAY, INDEX) _Generic((ARRAY),                                     \
    File_Handle_Array*:                remove_file_from_array,                     \
    Type_Handle_Array*:                remove_type_from_array,                     \
    Type_Reference_Handle_Array*:      remove_type_reference_from_array,           \
    Namespace_Handle_Array*:           remove_namespace_from_array,                \
    Function_Handle_Array*:            remove_function_from_array,                 \
    Formal_Parameter_Handle_Array*:    remove_formal_parameter_from_array,         \
    Statement_Handle_Array*:           remove_statement_from_array,                \
    Variable_Handle_Array*:            remove_variable_from_array,                 \
    Branch_Handle_Array*:              remove_branch_from_array,                   \
    Expression_Handle_Array*:          remove_expression_from_array,               \
    Field_Handle_Array*:               remove_field_from_array,                    \
    Field_Initializer_Handle_Array*:   remove_field_initializer_from_array,        \
    Constant_Handle_Array*:            remove_constant_from_array,                 \
    Element_Initializer_Handle_Array*: remove_element_initializer_from_array,      \
    Ir_Basic_Block_Handle_Array*:      remove_ir_basic_block_from_array,           \
    Ir_Instruction_Handle_Array*:      remove_ir_instruction_from_array,           \
    Ir_Operand_Handle_Array*:          remove_ir_operand_from_array)(ARRAY, INDEX)

#define reserve(ARRAY, AMOUNT) _Generic((ARRAY),                                  \
    File_Handle_Array*:                reserve_file_handles,                      \
    Type_Handle_Array*:                reserve_type_handles,                      \
    Type_Reference_Handle_Array*:      reserve_type_reference_handles,            \
    Namespace_Handle_Array*:           reserve_namespace_handles,                 \
    Function_Handle_Array*:            reserve_function_handles,                  \
    Formal_Parameter_Handle_Array*:    reserve_formal_parameter_handles,          \
    Statement_Handle_Array*:           reserve_statement_handles,                 \
    Variable_Handle_Array*:            reserve_variable_handles,                  \
    Branch_Handle_Array*:              reserve_branch_handles,                    \
    Expression_Handle_Array*:          reserve_expression_handles,                \
    Field_Handle_Array*:               reserve_field_handles,                     \
    Field_Initializer_Handle_Array*:   reserve_field_initializer_handles,         \
    Constant_Handle_Array*:            reserve_constant_handles,                  \
    Element_Initializer_Handle_Array*: reserve_element_initializer_handles,       \
    Ir_Basic_Block_Handle_Array*:      reserve_ir_basic_block_handles,            \
    Ir_Instruction_Handle_Array*:      reserve_ir_instruction_handles,            \
    Ir_Operand_Handle_Array*:          reserve_ir_operand_handles)(ARRAY, AMOUNT)

#define handle_at(ARRAY, INDEX) _Generic((ARRAY),                                \
    const File_Handle_Array*:                file_handle_at,                     \
          File_Handle_Array*:                file_handle_at,                     \
    const Type_Handle_Array*:                type_handle_at,                     \
          Type_Handle_Array*:                type_handle_at,                     \
    const Type_Reference_Handle_Array*:      type_reference_handle_at,           \
          Type_Reference_Handle_Array*:      type_reference_handle_at,           \
    const Namespace_Handle_Array*:           namespace_handle_at,                \
          Namespace_Handle_Array*:           namespace_handle_at,                \
    const Function_Handle_Array*:            function_handle_at,                 \
          Function_Handle_Array*:            function_handle_at,                 \
    const Formal_Parameter_Handle_Array*:    formal_parameter_handle_at,         \
          Formal_Parameter_Handle_Array*:    formal_parameter_handle_at,         \
    const Statement_Handle_Array*:           statement_handle_at,                \
          Statement_Handle_Array*:           statement_handle_at,                \
    const Variable_Handle_Array*:            variable_handle_at,                 \
          Variable_Handle_Array*:            variable_handle_at,                 \
    const Branch_Handle_Array*:              branch_handle_at,                   \
          Branch_Handle_Array*:              branch_handle_at,                   \
    const Expression_Handle_Array*:          expression_handle_at,               \
          Expression_Handle_Array*:          expression_handle_at,               \
    const Field_Handle_Array*:               field_handle_at,                    \
          Field_Handle_Array*:               field_handle_at,                    \
    const Field_Initializer_Handle_Array*:   field_initializer_handle_at,        \
          Field_Initializer_Handle_Array*:   field_initializer_handle_at,        \
    const Constant_Handle_Array*:            constant_handle_at,                 \
          Constant_Handle_Array*:            constant_handle_at,                 \
    const Element_Initializer_Handle_Array*: element_initializer_handle_at,      \
          Element_Initializer_Handle_Array*: element_initializer_handle_at,      \
    const Ir_Basic_Block_Handle_Array*:      ir_basic_block_handle_at,           \
          Ir_Basic_Block_Handle_Array*:      ir_basic_block_handle_at,           \
    const Ir_Instruction_Handle_Array*:      ir_instruction_handle_at,           \
          Ir_Instruction_Handle_Array*:      ir_instruction_handle_at,           \
    const Ir_Operand_Handle_Array*:          ir_operand_handle_at,               \
          Ir_Operand_Handle_Array*:          ir_operand_handle_at)(ARRAY, INDEX)

#define replace(ARRAY, HANDLE, INDEX) _Generic((ARRAY),                         \
    File_Handle_Array*:                replace_file,                            \
    Type_Handle_Array*:                replace_type,                            \
    Type_Reference_Handle_Array*:      replace_type_reference,                  \
    Namespace_Handle_Array*:           replace_namespace,                       \
    Function_Handle_Array*:            replace_function,                        \
    Formal_Parameter_Handle_Array*:    replace_formal_parameter,                \
    Statement_Handle_Array*:           replace_statement,                       \
    Variable_Handle_Array*:            replace_variable,                        \
    Branch_Handle_Array*:              replace_branch,                          \
    Expression_Handle_Array*:          replace_expression,                      \
    Field_Handle_Array*:               replace_field,                           \
    Field_Initializer_Handle_Array*:   replace_field_initializer,               \
    Constant_Handle_Array*:            replace_constant,                        \
    Element_Initializer_Handle_Array*: replace_element_initializer,             \
    Ir_Basic_Block_Handle_Array*:      replace_ir_basic_block,                  \
    Ir_Instruction_Handle_Array*:      replace_ir_instruction,                  \
    Ir_Operand_Handle_Array*:          replace_ir_operand)(ARRAY, HANDLE, INDEX)

#define lookup_in(ARRAY, INDEX) _Generic((ARRAY),                                \
    const File_Handle_Array*:                lookup_file_in,                     \
          File_Handle_Array*:                lookup_file_in,                     \
    const Type_Handle_Array*:                lookup_type_in,                     \
          Type_Handle_Array*:                lookup_type_in,                     \
    const Type_Reference_Handle_Array*:      lookup_type_reference_in,           \
          Type_Reference_Handle_Array*:      lookup_type_reference_in,           \
    const Namespace_Handle_Array*:           lookup_namespace_in,                \
          Namespace_Handle_Array*:           lookup_namespace_in,                \
    const Function_Handle_Array*:            lookup_function_in,                 \
          Function_Handle_Array*:            lookup_function_in,                 \
    const Formal_Parameter_Handle_Array*:    lookup_formal_parameter_in,         \
          Formal_Parameter_Handle_Array*:    lookup_formal_parameter_in,         \
    const Statement_Handle_Array*:           lookup_statement_in,                \
          Statement_Handle_Array*:           lookup_statement_in,                \
    const Variable_Handle_Array*:            lookup_variable_in,                 \
          Variable_Handle_Array*:            lookup_variable_in,                 \
    const Branch_Handle_Array*:              lookup_branch_in,                   \
          Branch_Handle_Array*:              lookup_branch_in,                   \
    const Expression_Handle_Array*:          lookup_expression_in,               \
          Expression_Handle_Array*:          lookup_expression_in,               \
    const Field_Handle_Array*:               lookup_field_in,                    \
          Field_Handle_Array*:               lookup_field_in,                    \
    const Field_Initializer_Handle_Array*:   lookup_field_initializer_in,        \
          Field_Initializer_Handle_Array*:   lookup_field_initializer_in,        \
    const Constant_Handle_Array*:            lookup_constant_in,                 \
          Constant_Handle_Array*:            lookup_constant_in,                 \
    const Element_Initializer_Handle_Array*: lookup_element_initializer_in,      \
          Element_Initializer_Handle_Array*: lookup_element_initializer_in,      \
    const Ir_Basic_Block_Handle_Array*:      lookup_ir_basic_block_in,           \
          Ir_Basic_Block_Handle_Array*:      lookup_ir_basic_block_in,           \
    const Ir_Instruction_Handle_Array*:      lookup_ir_instruction_in,           \
          Ir_Instruction_Handle_Array*:      lookup_ir_instruction_in,           \
    const Ir_Operand_Handle_Array*:          lookup_ir_operand_in,               \
          Ir_Operand_Handle_Array*:          lookup_ir_operand_in)(ARRAY, INDEX)

const char* expression_tag_as_string(uint8_t tag);
const char* type_reference_tag_as_string(uint8_t tag);
const char* type_tag_as_string(uint8_t tag);
const char* statement_tag_as_string(uint8_t tag);
const char* token_tag_as_string(uint8_t tag);

void ordered_ir_function_iteration(void (*func)(Ir_Function*));
bool imm_is_zero(const Ir_Immediate* value, uint8_t tag);

uint8_t count_bytes_in_sequence(const char* text);
uint32_t sequence_to_code_point(const char* text, uint8_t length);

bool is_ascii_upper(uint8_t c);
bool is_ascii_lower(uint8_t c);
bool is_ascii_digit(uint8_t c);
bool is_ascii_alpha_numeric(uint8_t c);
bool is_ascii_hex_digit(uint8_t c);
bool is_ascii_binary_digit(uint8_t c);

#ifdef __GNUC__
#define PACK(__Declaration__) __Declaration__ __attribute__((__packed__))
#else
#define PACK(__Declaration__) __pragma(pack(push, 1)) __Declaration__ __pragma(pack(pop))
#endif

extern uint8_t  text_section[TEXT_SECTION_CAPACITY];
extern uint32_t text_section_length;
