#include "expression.h"

void type_check_uninitialized_literal(const File* file, Expression* expression, Type_Handle inferred_type_handle, Expression_Check_Flags flags)
{
    if (flags & Expression_Check_Flags_constant)
        print_span_error(file, expression->span, "Unitialized literal is not constant.");

    if (flags & Expression_Check_Flags_is_nested)
        print_span_error(file, expression->span, "Unitialized literals cannot be nested.");

    if (flags & Expression_Check_Flags_allow_unitialized_literals)
    {
        assert(!is_invalid_type_handle(inferred_type_handle));
        expression->type = inferred_type_handle;
    }
    else
        print_span_error(file, expression->span, "Unitialized literals can only be used in declarations.");
}
