#include "expression.h"

void type_check_dereference(const File* file, Dereference* dereference, Expression_Check_Flags flags)
{
    if (flags & Expression_Check_Flags_constant)
        print_span_error(file, dereference->span, "Dereference operator is not constant.");

    Expression* sub_expression = lookup(dereference->expression);

    type_check_expression(file, sub_expression, invalid_type_handle, flags);
    if (!is_addressable(sub_expression->tag))
        print_span_error(file, sub_expression->span, "Expression is not addressable.");

    Type* type = lookup(sub_expression->type);
    Qualifier qualifiers = 0;

    if (type->tag == Type_Tag_qualified)
    {
        qualifiers = type->qualified.qualifiers;
        type = lookup(type->qualified.unqualified);
    }

    if (type->tag != Type_Tag_pointer)
        print_span_error(file, sub_expression->span, "Pointer type expected but found %t.", sub_expression->type);
    else if (lookup(type->pointer.base_type)->is_pointerless)
        qualifiers &= ~Qualifier_noalias;

    dereference->type = find_or_add_qualified_type(qualifiers, type->pointer.base_type);
}

void deep_copy_dereference(Dereference* restrict destination, const Dereference* restrict source)
{
    destination->expression = add_expression();
    deep_copy_expression(lookup(destination->expression), lookup(source->expression));
}