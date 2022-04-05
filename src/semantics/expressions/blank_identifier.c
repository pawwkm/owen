#include "expression.h"

void type_check_blank_identifier(const File* file, Expression* expression, Expression_Check_Flags flags)
{
    if (flags & Expression_Check_Flags_is_nested)
        print_span_error(file, expression->span, "_ cannot be nested.");
    else if (!(flags & Expression_Check_Flags_lhs_value))
        print_span_error(file, expression->span, "_ only allowed in tuple declarations and assignments.");

    // The type is set elsewhere.
}
