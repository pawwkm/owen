#include "statement.h"

void type_check_expression_statement(const File* file, Expression* expression)
{
    type_check_expression(file, expression, invalid_type_handle, Expression_Check_Flags_none);
    if (expression->tag != Expression_Tag_call)
        print_span_error(file, expression->span, "Function call expected.");

    if (!expression_types_match(expression->type, none_handle))
    {
        char* message = "%t is unassigned.";
        if (lookup(expression->type)->tag == Type_Tag_tuple)
            message = "%t are unassigned.";

        print_span_error(file, expression->span, message, expression->type);
    }
}
