#include "expressions.h"

Ir_Operand_Handle lower_expression(Ir_Basic_Block* block, Expression_Handle handle)
{
    Expression* expression = lookup_expression(handle);
    if (expression->tag == Expression_Tag_reference)
        return lower_reference(block, &expression->reference);
    else if (expression->tag == Expression_Tag_integer || expression->tag == Expression_Tag_float)
        return lower_number_literal(&expression->number);
    else if (expression->tag == Expression_Tag_true)
        not_implemented(__FILE__, __LINE__, "Expression_Tag_true");
    else if (expression->tag == Expression_Tag_false)
        not_implemented(__FILE__, __LINE__, "Expression_Tag_false");
    else if (expression->tag == Expression_Tag_call)
        return lower_call(block, &expression->call);
    else if (expression->tag == Expression_Tag_binary)
        not_implemented(__FILE__, __LINE__, "Expression_Tag_binary");
    else if (expression->tag == Expression_Tag_field_access)
        not_implemented(__FILE__, __LINE__, "Expression_Tag_field_access");
    else if (expression->tag == Expression_Tag_enumeration_constant_access)
        not_implemented(__FILE__, __LINE__, "Expression_Tag_enumeration_constant_access");
    else if (expression->tag == Expression_Tag_array_access)
        not_implemented(__FILE__, __LINE__, "Expression_Tag_array_access");
    else if (expression->tag == Expression_Tag_array_literal)
        not_implemented(__FILE__, __LINE__, "Expression_Tag_array_literal");
    else if (expression->tag == Expression_Tag_parenthesized)
        not_implemented(__FILE__, __LINE__, "Expression_Tag_parenthesized");
    else if (expression->tag == Expression_Tag_compound_literal)
        not_implemented(__FILE__, __LINE__, "Expression_Tag_compound_literal");
    else if (expression->tag == Expression_Tag_fixed_array_length)
        not_implemented(__FILE__, __LINE__, "Expression_Tag_fixed_array_length");
    else if (expression->tag == Expression_Tag_dynamic_array_length)
        not_implemented(__FILE__, __LINE__, "Expression_Tag_dynamic_array_length");
    else if (expression->tag == Expression_Tag_dynamic_array_capacity)
        not_implemented(__FILE__, __LINE__, "Expression_Tag_dynamic_array_capacity");
    else if (expression->tag == Expression_Tag_dynamic_array_elements)
        not_implemented(__FILE__, __LINE__, "Expression_Tag_dynamic_array_elements");
    else if (expression->tag == Expression_Tag_cast)
        return lower_cast(block, &expression->cast);
    else if (expression->tag == Expression_Tag_address_of)
        not_implemented(__FILE__, __LINE__, "Expression_Tag_address_of");
    else if (expression->tag == Expression_Tag_dereference)
        not_implemented(__FILE__, __LINE__, "Expression_Tag_dereference");
    else if (expression->tag == Expression_Tag_not)
        not_implemented(__FILE__, __LINE__, "Expression_Tag_not");
    else if (expression->tag == Expression_Tag_uninitialized_literal)
        not_implemented(__FILE__, __LINE__, "Expression_Tag_uninitialized_literal");
    else if (expression->tag == Expression_Tag_negate)
        not_implemented(__FILE__, __LINE__, "Expression_Tag_negate");
    else if (expression->tag == Expression_Tag_blank_identifier)
        not_implemented(__FILE__, __LINE__, "Expression_Tag_blank_identifier");
    else if (expression->tag == Expression_Tag_null)
        not_implemented(__FILE__, __LINE__, "Expression_Tag_null");
    else
        unexpected_expression(__FILE__, __LINE__, expression->tag);

    return invalid_ir_operand_handle;
}

void lower_expressions(Ir_Basic_Block* block, Ir_Operand_Handle_Array* destination, const Expression_Handle_Array* source)
{
    for (uint8_t i = 0; i < source->handles_length; i++)
        add_to_ir_operand_array(destination, lower_expression(block, source->handles[i]));
}
