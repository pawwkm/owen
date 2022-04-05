#include "expression.h"

void type_check_null(const File* file, Expression* expression, Type_Handle inferred_type_handle)
{
    if (is_invalid_type_handle(inferred_type_handle))
        print_span_error(file, expression->span, "Null type cannot be inferred.");

    if (lookup_type(inferred_type_handle)->tag != Type_Tag_pointer)
        print_span_error(file, expression->span, "Pointer type expected but found %t.", inferred_type_handle);

    expression->type = inferred_type_handle;
}