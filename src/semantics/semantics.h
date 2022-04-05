#pragma once

#include "../compiler.h"

void check_that_identifier_is_undefined(const File* file, Interned_String_Handle name, Span name_span);
void check_for_unreferenced_variables(const File* file, uint8_t start_from_index);

void check_public_compound_for_fields_with_non_public_type(const Compound_Type* compound);
void check_compound_for_recursive_fields(const Compound_Type* compound);
void check_if_compound_has_nested_formal_type_parameters(const Compound_Type* compound);
void check_if_compounds_formal_type_parameters_matches_any_types(Type_Handle type_handle);
void check_if_compound_has_duplicate_formal_type_parameters(const Compound_Type* compound);
void check_if_compound_has_duplicate_field_names(const Compound_Type* compound);

Symbol* symbol_of_reference(Interned_String_Handle name);

bool signatures_match(Function_Handle a, Function_Handle b);

void assign_types_to_compound_fields(const Compound_Type* compound);

void intern_primitive_type_identifiers(void);

bool is_addressable(uint8_t tag);

// TODO
// I have multiple functions for testing whether or not types match.
// 1 for Type_Handle, 1 one for Type_Handle vs. Type_Reference_Handle and
// 1 for Type_Reference_Handle. Can I do something to remove some of them
// or can I at least streamline them?
// static bool type_declarations_match(Type* a, Type* b)
// expression_types_match
// formal_parameter_type_matches_actual_parameter_type
// formal_parameter_type_matches_actual_type_parameter
// formal_parameter_types_in_signatures_matches
// bool signatures_match(Function* a, Function* b)
// signature_matches_actual_parameter_types(Function* function, Expression_List* actual_parameters)
// put all of these in file.c or if there is enough move them into a file
// that is only about matching types.
// Is the problem more that I currently need to compare Type and Type_Reference in varieos ways?
// Can I get away with just comparing Type_References? If so, do that. I really don't like the 
// occational performance bump by comparing pointers at the added cost of complexity from
// all the variations.
//
// At the very least have 2 veriants. One for type and one for type references.
// That should work. In the case where there is one of each then lower the 
// type reference to a type the compare.
bool expression_types_match(Type_Handle expected_type, Type_Handle actual_type);

void type_check_signature(const File* file, Function* function);
void type_check_function_body(const File* file, Function* function);
void type_check_enumeration_declaration(Enumeration_Type* enumeration);
bool type_check_statements(const File* file, Statement_Handle_Array* body, const Function* function, bool is_inside_loop);

typedef enum
{
    Expression_Check_Flags_none = 1 << 0,
    Expression_Check_Flags_retain = 1 << 1,
    Expression_Check_Flags_allow_unitialized_literals = 1 << 2,
    Expression_Check_Flags_is_nested = 1 << 3,
    Expression_Check_Flags_constant = 1 << 4,
    Expression_Check_Flags_lhs_value = 1 << 5,
    Expression_Check_Flags_rhs_value = 1 << 6
} Expression_Check_Flags;
Type_Handle type_check_expression(const File* file, Expression* expression, Type_Handle inferred_type, Expression_Check_Flags flags);
Number type_check_integer_literal(const File* file, String integer, Span integer_span, Type_Handle inferred_type_handle);

void deep_copy_type_references(Type_Reference_Handle_Array* restrict destination, const Type_Reference_Handle_Array* restrict source);
void deep_copy_type_reference(Type_Reference_Handle destination_handle, Type_Reference_Handle source_handle);
void deep_copy_statements(Statement_Handle_Array* restrict destination, const Statement_Handle_Array* restrict source);
void deep_copy_expressions(Expression_Handle_Array* restrict destination, const Expression_Handle_Array* restrict source);
void deep_copy_expression(Expression* restrict destination, const Expression* restrict source);

Type_Handle monomorphisize_compound(Type_Handle polymorphic_handle, const Compound_Type* polymorphic);
Type_Handle monomorphisize_function(Function_Handle polymorphic_handle);

extern Interned_String_Handle length_field;
extern Interned_String_Handle capacity_field;
extern Interned_String_Handle elements_field;

Type_Handle find_or_add_pointer_type(Type_Handle base_type, Pointer_Type_Privilege privileges);
