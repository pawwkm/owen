#include "expression.h"

void type_check_sizeof(const File* file, Size_Of* size_of, Type_Handle inferred_type_handle, Expression_Check_Flags flags)
{
    if (flags & Expression_Check_Flags_constant)
        print_span_error(file, size_of->span, "sizeof is not constant.");

    if (invalid(inferred_type_handle))
        print_span_error(file, size_of->span, "sizeof type cannot be inferred.");

    Type_Tag inferred_type_tag = lookup(inferred_type_handle)->tag;
    if (inferred_type_tag > Type_Tag_u64)
        print_span_error(file, size_of->span, "Cannot infer sizeof as %t.", inferred_type_handle);

    Type* t;
    if (invalid(size_of->t))
        t = lookup(type_check_expression(file, lookup(size_of->expression), invalid_type_handle, flags | Expression_Check_Flags_is_nested));
    else
        t = lookup(lookup_type_by_reference(file, size_of->t, true));

    if (!t->size_of)
        ice(__FILE__, __LINE__, "Zero sized type.");

    Number value;
    if (inferred_type_tag == Type_Tag_i8)
    {
        if (t->size_of > INT8_MAX)
            print_span_error(file, size_of->span, "%u overflows %t.", t->size_of, inferred_type_handle);

        value.i8 = (int8_t)t->size_of;
    }
    else if (inferred_type_tag == Type_Tag_i16)
    {
        if (t->size_of > INT16_MAX)
            print_span_error(file, size_of->span, "%u overflows %t.", t->size_of, inferred_type_handle);

        value.i16 = (int16_t)t->size_of;
    }
    else if (inferred_type_tag == Type_Tag_i32)
    {
        if (t->size_of > INT32_MAX)
            print_span_error(file, size_of->span, "%u overflows %t.", t->size_of, inferred_type_handle);

        value.i32 = (int32_t)t->size_of;
    }
    else if (inferred_type_tag == Type_Tag_u8)
    {
        if (t->size_of > UINT8_MAX)
            print_span_error(file, size_of->span, "%u overflows %t.", t->size_of, inferred_type_handle);

        value.u8 = (uint8_t)t->size_of;
    }
    else if (inferred_type_tag == Type_Tag_u16)
    {
        if (t->size_of > UINT16_MAX)
            print_span_error(file, size_of->span, "%u overflows %t.", t->size_of, inferred_type_handle);

        value.u16 = (uint16_t)t->size_of;
    }
    else
        value.u64 = t->size_of;

    *(Number_Literal*)size_of = (Number_Literal)
    {
        .span = size_of->span,
        .tag = Expression_Tag_integer,
        .type = inferred_type_handle,
        .value = value
    };
}

void deep_copy_sizeof(Size_Of* restrict destination, const Size_Of* restrict source)
{
    if (invalid(source->t))
    {
        destination->expression = add_expression();
        deep_copy_expression(lookup(destination->expression), lookup(source->expression));
    }
    else
    {
        destination->t = add_type_reference();
        deep_copy_type_reference(destination->t, source->t);
    }
}
