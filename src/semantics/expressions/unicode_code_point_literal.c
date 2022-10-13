#include "expression.h"

void type_check_unicode_code_point_literal(const File* file, Unicode_Code_Point_Literal* literal, Type_Handle inferred_type_handle)
{
    if (invalid(inferred_type_handle))
        print_span_error(file, literal->span, "Integer type cannot be inferred.");
    
    Type_Tag inferred_type_tag = lookup(inferred_type_handle)->tag;
    if (inferred_type_tag > Type_Tag_u64)
        print_span_error(file, literal->span, "Cannot infer literal as %t.", inferred_type_handle);
    
    Number value;
    if (inferred_type_tag == Type_Tag_i8)
    {
        if (literal->code_point > INT8_MAX)
            print_span_error(file, literal->span, "%u overflows %t.", literal->code_point, literal->type);

        value.i8 = (int8_t)literal->code_point;
    }
    else if (inferred_type_tag == Type_Tag_i16)
    {
        if (literal->code_point > INT16_MAX)
            print_span_error(file, literal->span, "%u overflows %t.", literal->code_point, literal->type);

        value.i16 = (int16_t)literal->code_point;
    }
    else if (inferred_type_tag == Type_Tag_i32)
    {
        if (literal->code_point > INT32_MAX)
            print_span_error(file, literal->span, "%u overflows %t.", literal->code_point, literal->type);

        value.i32 = (int32_t)literal->code_point;
    }
    else if (inferred_type_tag == Type_Tag_u8)
    {
        if (literal->code_point > UINT8_MAX)
            print_span_error(file, literal->span, "%u overflows %t.", literal->code_point, literal->type);

        value.u8 = (uint8_t)literal->code_point;
    }
    else if (inferred_type_tag == Type_Tag_u16)
    {
        if (literal->code_point > UINT16_MAX)
            print_span_error(file, literal->span, "%u overflows %t.", literal->code_point, literal->type);

        value.u16 = (uint16_t)literal->code_point;
    }
    else
        value.u64 = literal->code_point;

    *(Number_Literal*)literal = (Number_Literal)
    {
        .span = literal->span,
        .tag = Expression_Tag_integer,
        .type = inferred_type_handle,
        .value = value
    };
}

void deep_copy_unicode_code_point_literal(Unicode_Code_Point_Literal* restrict destination, const Unicode_Code_Point_Literal* restrict source)
{
    destination->code_point = source->code_point;
}
