#include "expression.h"

void type_check_parenthesized(const File* file, Parenthesized* parenthesized, Expression_Check_Flags flags)
{
    parenthesized->type = type_check_expression(file, lookup_expression(parenthesized->expression), invalid_type_handle, flags);
}

void deep_copy_parenthesized(Parenthesized* restrict destination, const Parenthesized* restrict source)
{
    destination->expression = add_expression();
    deep_copy_expression(lookup_expression(destination->expression), lookup_expression(source->expression));
}
