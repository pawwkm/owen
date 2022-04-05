#include "expression.h"

void type_check_float_literal(const File* file, Unchecked_Number* literal, Type_Handle inferred_type_handle)
{
    if (is_invalid_type_handle(inferred_type_handle))
        print_span_error(file, literal->span, "Float type cannot be inferred.");

    Number value;
    if (compare_types(f32_handle, inferred_type_handle))
        sscanf(literal->string.text, "%f", &value.f32);
    else if (compare_types(f64_handle, inferred_type_handle))
        sscanf(literal->string.text, "%lf", &value.f64);
    else
        print_span_error(file, literal->span, "Cannot infer literal as %t.", inferred_type_handle);

    *(Number_Literal*)literal = (Number_Literal)
    {
        .span = literal->span,
        .tag = Expression_Tag_float,
        .type = inferred_type_handle,
        .value = value
    };
}
