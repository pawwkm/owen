#include "expression.h"

void type_check_address_of(const File* file, Address_Of* address_of, Expression_Check_Flags flags)
{
    if (flags & Expression_Check_Flags_constant)
        print_span_error(file, address_of->span, "Address of operator is not constant.");

    Expression* sub_expression = lookup(address_of->expression);

    type_check_expression(file, sub_expression, invalid_type_handle, flags);
    if (!is_addressable(sub_expression->tag))
        print_span_error(file, sub_expression->span, "Expression is not addressable.");

    Qualified_Type* sub_expression_type = &lookup(sub_expression->type)->qualified;
    if (sub_expression_type->tag == Type_Tag_qualified)
        address_of->type = find_or_add_qualified_type(sub_expression_type->qualifiers, find_or_add_pointer_type(sub_expression_type->unqualified));
    else
        address_of->type = find_or_add_pointer_type(sub_expression->type);
}

void deep_copy_address_of(Address_Of* restrict destination, const Address_Of* restrict source)
{
    destination->expression = add_expression();
    deep_copy_expression(lookup(destination->expression), lookup(source->expression));
}
