#include "expression.h"

void type_check_not(const File* file, Not* not, Expression_Check_Flags flags)
{
    if (flags & Expression_Check_Flags_constant)
        print_span_error(file, not->span, "Not operator is not constant.");

    Expression* sub_expression = lookup(not->expression);
    not->type = type_check_expression(file, sub_expression, invalid_type_handle, flags);
    if (!compare(bool_handle, not->type))
        print_span_error(file, sub_expression->span, "%t expected but found %t.", bool_handle, not->type);
}

void deep_copy_not(Not* restrict destination, const Not* restrict source)
{
    destination->expression = add_expression();
    deep_copy_expression(lookup(destination->expression), lookup(source->expression));
}