#include "expression.h"

void type_check_utf8_string_literal(const File* file, Utf8_String_Literal* literal, Type_Handle inferred_type_handle, Expression_Check_Flags flags)
{
    if (flags & Expression_Check_Flags_constant)
        print_span_error(file, literal->span, "Utf8 string literals are not constant.");
    else if (invalid(inferred_type_handle))
        print_span_error(file, literal->span, "String literal type cannot be inferred.");
    
    if (!expression_types_match(inferred_type_handle, utf8_string_literal_type_handle))
        print_span_error(file, literal->span, "Cannot infer literal as %t.", inferred_type_handle);

    literal->type = inferred_type_handle;
}

void deep_copy_utf8_string_literal(Utf8_String_Literal* restrict destination, const Utf8_String_Literal* restrict source)
{
    destination->string = source->string;
}
