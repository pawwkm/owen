#include "statement.h"

void type_check_break_statement(const File* file, const Break_Statement* break_statement, bool is_inside_loop)
{
    if (!is_inside_loop)
        print_span_error(file, break_statement->span, "No enclosing loop of which to break out of.");
}

void deep_copy_break_statement(Break_Statement* restrict destination, const Break_Statement* restrict source)
{
    destination->span = source->span;
}
