#include "statement.h"

void type_check_continue_statement(const File* file, const Continue_Statement* continue_statement, bool is_inside_loop)
{
    if (!is_inside_loop)
        print_span_error(file, continue_statement->span, "No enclosing loop of which to continue from.");
}

void deep_copy_continue_statement(Continue_Statement* restrict destination, const Continue_Statement* restrict source)
{
    destination->span = source->span;
}
