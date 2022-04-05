#pragma once

#include "../semantics.h"

void type_check_binary(const File* file, Binary* expression, Type_Handle inferred_type_handle, Expression_Check_Flags flags);
void type_check_compound_literal(const File* file, Compound_Literal* literal, Type_Handle inferred_type_handle, Expression_Check_Flags flags);
void type_check_reference_expression(const File* file, Reference* reference, Type_Handle inferred_type_handle);
void type_check_float_literal(const File* file, Unchecked_Number* literal, Type_Handle inferred_type_handle);
void type_check_enumeration_constant_access(const File* file, Enumeration_Constant_Access* access);
void type_check_call_expression(const File* file, Call* call, Expression_Check_Flags flags);
void type_check_field_access(const File* file, Field_Access* access, Expression_Check_Flags flags);
void type_check_array_literal(const File* file, Array_Literal* literal, Type_Handle inferred_type_handle, Expression_Check_Flags flags);
void type_check_array_access(const File* file, Array_Access* access, Expression_Check_Flags flags);
void type_check_parenthesized(const File* file, Parenthesized* parenthesized, Expression_Check_Flags flags);
void type_check_cast(const File* file, Cast* cast, Expression_Check_Flags flags);
void type_check_address_of(const File* file, Address_Of* address_of, Expression_Check_Flags flags);
void type_check_dereference(const File* file, Dereference* dereference, Expression_Check_Flags flags);
void type_check_not(const File* file, Not* not, Expression_Check_Flags flags);
void type_check_uninitialized_literal(const File* file, Expression* expression, Type_Handle inferred_type_handle, Expression_Check_Flags flags);
void type_check_negate(const File* file, Negate* negate, Type_Handle inferred_type_handle, Expression_Check_Flags flags);
void type_check_blank_identifier(const File* file, Expression* expression, Expression_Check_Flags flags);
void type_check_null(const File* file, Expression* expression, Type_Handle inferred_type_handle);
void type_check_unicode_code_point_literal(const File* file, Unicode_Code_Point_Literal* literal, Type_Handle inferred_type_handle);
void type_check_utf8_string_literal(const File* file, Utf8_String_Literal* literal, Type_Handle inferred_type_handle, Expression_Check_Flags flags);
void type_check_sizeof(const File* file, Size_Of* size_of, Type_Handle inferred_type_handle, Expression_Check_Flags flags);

void deep_copy_reference(Reference* restrict destination, const Reference* restrict source);
void deep_copy_call(Call* restrict destination, const Call* restrict source);
void deep_copy_binary(Binary* restrict destination, const Binary* restrict source);
void deep_copy_array_access(Array_Access* restrict destination, const Array_Access* restrict source);
void deep_copy_array_literal(Array_Literal* restrict destination, const Array_Literal* restrict source);
void deep_copy_parenthesized(Parenthesized* restrict destination, const Parenthesized* restrict source);
void deep_copy_compound_literal(Compound_Literal* restrict destination, const Compound_Literal* restrict source);
void deep_copy_field_access(Field_Access* restrict destination, const Field_Access* restrict source);
void deep_copy_enumeration_constant_access(Enumeration_Constant_Access* restrict destination, const Enumeration_Constant_Access* restrict source);
void deep_copy_cast(Cast* restrict destination, const Cast* restrict source);
void deep_copy_address_of(Address_Of* restrict destination, const Address_Of* restrict source);
void deep_copy_dereference(Dereference* restrict destination, const Dereference* restrict source);
void deep_copy_not(Not* restrict destination, const Not* restrict source);
void deep_copy_unicode_code_point_literal(Unicode_Code_Point_Literal* restrict destination, const Unicode_Code_Point_Literal* restrict source);
void deep_copy_utf8_string_literal(Utf8_String_Literal* restrict destination, const Utf8_String_Literal* restrict source);
void deep_copy_sizeof(Size_Of* restrict destination, const Size_Of* restrict source);

bool is_formal_type_parameter_specified_by_a_formal_parameter(Type_Reference_Handle formal_parameter_handle, Interned_String_Handle t);
bool formal_parameter_type_matches_actual_parameter_type(const File* file, const Function* function, Type_Reference_Handle formal_parameter_type_handle, Type_Handle actual_type_handle);
void map_formal_type_parameter(const File* file, const Function* function, Type_Handle actual_parameter_type, const Type_Reference* formal_type_parameter, Span span_of_type_definition);
