#include "expression.h"

void type_check_dereference(const File* file, Dereference* dereference, Expression_Check_Flags flags)
{
    if (flags & Expression_Check_Flags_constant)
        print_span_error(file, dereference->span, "Dereference operator is not constant.");

    Expression* sub_expression = lookup_expression(dereference->expression);

    type_check_expression(file, sub_expression, invalid_type_handle, flags);
    if (!is_addressable(sub_expression->tag))
        print_span_error(file, sub_expression->span, "Expression is not addressable.");

    Pointer_Type* type = &lookup_type(sub_expression->type)->pointer;
    if (type->tag != Type_Tag_pointer)
        print_span_error(file, sub_expression->span, "Pointer type expected but found %t.", sub_expression->type);
    else if (flags & Expression_Check_Flags_lhs_value && !(type->privileges & Pointer_Type_Privilege_writable))
        print_span_error(file, dereference->span, "Missing writable privilege ($) from %t.", sub_expression->type);
    else if (flags & Expression_Check_Flags_rhs_value && !(type->privileges & Pointer_Type_Privilege_readable))
        print_span_error(file, dereference->span, "Missing readable privilege (?) from %t.", sub_expression->type);

    dereference->type = type->base_type;
}

void deep_copy_dereference(Dereference* restrict destination, const Dereference* restrict source)
{
    destination->expression = add_expression();
    deep_copy_expression(lookup_expression(destination->expression), lookup_expression(source->expression));
}