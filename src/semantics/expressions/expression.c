#include "expression.h"

Type_Handle type_check_expression(const File* file, Expression* expression, Type_Handle inferred_type_handle, Expression_Check_Flags flags)
{
    assert(is_invalid_type_handle(expression->type));
    if (expression->tag == Expression_Tag_binary)
        type_check_binary(file, &expression->binary, inferred_type_handle, flags | Expression_Check_Flags_is_nested);
    else if (expression->tag == Expression_Tag_compound_literal)
        type_check_compound_literal(file, &expression->compound_literal, inferred_type_handle, flags | Expression_Check_Flags_is_nested);
    else if (expression->tag == Expression_Tag_reference)
        type_check_reference_expression(file, &expression->reference, inferred_type_handle);
    else if (expression->tag == Expression_Tag_true || expression->tag == Expression_Tag_false)
        expression->type = bool_handle;
    else if (expression->tag == Expression_Tag_unicode_code_point)
        type_check_unicode_code_point_literal(file, &expression->code_point, inferred_type_handle);
    else if (expression->tag == Expression_Tag_integer)
    {
        expression->number.value = type_check_integer_literal(file, expression->unchecked_number.string, expression->unchecked_number.span, inferred_type_handle);
        expression->type = inferred_type_handle;
    }
    else if (expression->tag == Expression_Tag_float)
        type_check_float_literal(file, &expression->unchecked_number, inferred_type_handle);
    else if (expression->tag == Expression_Tag_enumeration_constant_access)
        type_check_enumeration_constant_access(file, &expression->enumeration_constant_access);
    else if (expression->tag == Expression_Tag_call)
        type_check_call_expression(file, &expression->call, flags | Expression_Check_Flags_is_nested);
    else if (expression->tag == Expression_Tag_field_access)
        type_check_field_access(file, &expression->field_access, flags | Expression_Check_Flags_is_nested);
    else if (expression->tag == Expression_Tag_array_literal)
        type_check_array_literal(file, &expression->array_literal, inferred_type_handle, flags | Expression_Check_Flags_is_nested);
    else if (expression->tag == Expression_Tag_array_access)
        type_check_array_access(file, &expression->array_access, flags | Expression_Check_Flags_is_nested);
    else if (expression->tag == Expression_Tag_parenthesized)
        type_check_parenthesized(file, &expression->parenthesized, flags | Expression_Check_Flags_is_nested);
    else if (expression->tag == Expression_Tag_cast)
        type_check_cast(file, &expression->cast, flags | Expression_Check_Flags_is_nested);
    else if (expression->tag == Expression_Tag_address_of)
        type_check_address_of(file, &expression->address_of, flags | Expression_Check_Flags_is_nested);
    else if (expression->tag == Expression_Tag_dereference)
        type_check_dereference(file, &expression->dereference, flags | Expression_Check_Flags_is_nested);
    else if (expression->tag == Expression_Tag_not)
        type_check_not(file, &expression->not, flags | Expression_Check_Flags_is_nested);
    else if (expression->tag == Expression_Tag_uninitialized_literal)
        type_check_uninitialized_literal(file, expression, inferred_type_handle, flags);
    else if (expression->tag == Expression_Tag_negate)
        type_check_negate(file, &expression->negate, inferred_type_handle, flags);
    else if (expression->tag == Expression_Tag_blank_identifier)
        type_check_blank_identifier(file, expression, flags);
    else if (expression->tag == Expression_Tag_null)
        type_check_null(file, expression, inferred_type_handle);
    else if (expression->tag == Expression_Tag_utf8_string_literal)
        type_check_utf8_string_literal(file, &expression->string_literal, inferred_type_handle, flags);
    else if (expression->tag == Expression_Tag_sizeof)
        type_check_sizeof(file, &expression->size_of, inferred_type_handle, flags);
    else
        unexpected_expression(__FILE__, __LINE__, expression->tag);

    return expression->type;
}

bool expression_types_match(Type_Handle expected_type_handle, Type_Handle actual_type_handle)
{
    Type* expected_type = lookup_type(expected_type_handle);
    Type* actual_type = lookup_type(actual_type_handle);

    return expected_type == actual_type || 
           expected_type->tag == actual_type->tag &&
           expected_type->tag == Type_Tag_pointer &&
           (~actual_type->pointer.privileges & expected_type->pointer.privileges) == 0 &&
           expression_types_match(expected_type->pointer.base_type, actual_type->pointer.base_type);
}

void deep_copy_expressions(Expression_Handle_Array* destination, const Expression_Handle_Array* source)
{
    reserve_expression_handles(destination, source->handles_length);
    for (uint8_t i = 0; i < source->handles_length; i++)
    {
        add_to_expression_array(destination, add_expression());
        deep_copy_expression(lookup_expression(destination->handles[i]), lookup_expression(source->handles[i]));
    }
}

void deep_copy_expression(Expression* restrict destination, const Expression* restrict source)
{
    destination->tag = source->tag;
    destination->type = source->type;
    destination->span = source->span;

    if (source->tag == Expression_Tag_reference)
        deep_copy_reference(&destination->reference, &source->reference);
    else if (source->tag == Expression_Tag_integer)
        destination->unchecked_number = source->unchecked_number;
    else if (source->tag == Expression_Tag_float)
        destination->unchecked_number = source->unchecked_number;
    else if (source->tag == Expression_Tag_true)
        ;
    else if (source->tag == Expression_Tag_false)
        ;
    else if (source->tag == Expression_Tag_call)
        deep_copy_call(&destination->call, &source->call);
    else if (source->tag == Expression_Tag_binary)
        deep_copy_binary(&destination->binary, &source->binary);
    else if (source->tag == Expression_Tag_field_access)
        deep_copy_field_access(&destination->field_access, &source->field_access);
    else if (source->tag == Expression_Tag_enumeration_constant_access)
        deep_copy_enumeration_constant_access(&destination->enumeration_constant_access, &source->enumeration_constant_access);
    else if (source->tag == Expression_Tag_array_access)
        deep_copy_array_access(&destination->array_access, &source->array_access);
    else if (source->tag == Expression_Tag_parenthesized)
        deep_copy_parenthesized(&destination->parenthesized, &source->parenthesized);
    else if (source->tag == Expression_Tag_array_literal)
        deep_copy_array_literal(&destination->array_literal, &source->array_literal);
    else if (source->tag == Expression_Tag_compound_literal)
        deep_copy_compound_literal(&destination->compound_literal, &source->compound_literal);
    else if (source->tag == Expression_Tag_cast)
        deep_copy_cast(&destination->cast, &source->cast);
    else if (source->tag == Expression_Tag_address_of)
        deep_copy_address_of(&destination->address_of, &source->address_of);
    else if (source->tag == Expression_Tag_dereference)
        deep_copy_dereference(&destination->dereference, &source->dereference);
    else if (source->tag == Expression_Tag_not)
        deep_copy_not(&destination->not, &source->not);
    else if (source->tag == Expression_Tag_uninitialized_literal)
        ;
    else if (source->tag == Expression_Tag_blank_identifier)
        ;
    else if (source->tag == Expression_Tag_null)
        ;
    else if (source->tag == Expression_Tag_unicode_code_point)
        deep_copy_unicode_code_point_literal(&destination->code_point, &source->code_point);
    else if (source->tag == Expression_Tag_utf8_string_literal)
        deep_copy_utf8_string_literal(&destination->string_literal, &source->string_literal);
    else if (source->tag == Expression_Tag_sizeof)
        deep_copy_sizeof(&destination->size_of, &source->size_of);
    else
        unexpected_expression(__FILE__, __LINE__, source->tag);
}

bool is_addressable(uint8_t tag)
{
    return tag == Expression_Tag_reference ||
           tag == Expression_Tag_field_access ||
           tag == Expression_Tag_dynamic_array_length ||
           tag == Expression_Tag_dynamic_array_capacity ||
           tag == Expression_Tag_dynamic_array_elements ||
           tag == Expression_Tag_array_access || 
           tag == Expression_Tag_dereference ||
           tag == Expression_Tag_blank_identifier;
}
