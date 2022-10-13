#include "expression.h"

static Expression* is_referencing_variable_or_parameter(const File* file, Expression* expression)
{
    switch (expression->tag)
    {
        case Expression_Tag_reference:
            if (!expression->reference.actual_type_parameters.handles_length)
                return expression;

        case Expression_Tag_array_access:
            return is_referencing_variable_or_parameter(file, lookup(expression->array_access.array));

        case Expression_Tag_field_access:
        case Expression_Tag_fixed_array_length:
        case Expression_Tag_dynamic_array_length:
        case Expression_Tag_dynamic_array_capacity:
        case Expression_Tag_dynamic_array_elements:
            return is_referencing_variable_or_parameter(file, lookup(expression->field_access.compound));

        default:
            return NULL;
    }
}

void type_check_address_of(const File* file, Address_Of* address_of, Expression_Check_Flags flags)
{
    if (flags & Expression_Check_Flags_constant)
        print_span_error(file, address_of->span, "Address of operator is not constant.");

    // In situations like overload resolution it cannot be determined whether or not an address_of
    // expression needs to be retained. Therefore this function is called a second time when that
    // is known. Be aware this is an exceptional case.
    if (invalid(address_of->type))
    {
        Expression* sub_expression = lookup(address_of->expression);

        type_check_expression(file, sub_expression, invalid_type_handle, flags);
        if (!is_addressable(sub_expression->tag))
            print_span_error(file, sub_expression->span, "Expression is not addressable.");

        address_of->type = find_or_add_pointer_type(sub_expression->type, ALL_PRIVILEGES);
    }
    
    if (flags & Expression_Check_Flags_retain)
    {        
        Expression* reference = is_referencing_variable_or_parameter(file, lookup(address_of->expression));
        if (reference)
            print_span_error(file, reference->span, "Parameters and variables cannot be retained.");
    }
}

void deep_copy_address_of(Address_Of* restrict destination, const Address_Of* restrict source)
{
    destination->expression = add_expression();
    deep_copy_expression(lookup(destination->expression), lookup(source->expression));
}
