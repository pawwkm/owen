#include "expression.h"

Number type_check_integer_literal(const File* file, String integer, Span integer_span, Type_Handle inferred_type_handle)
{
    if (is_invalid_type_handle(inferred_type_handle))
        print_span_error(file, integer_span, "Integer type cannot be inferred.");
    
    Type_Tag inferred_type_tag = lookup_type(inferred_type_handle)->tag;
    if (inferred_type_tag > Type_Tag_u64)
        print_span_error(file, integer_span, "Cannot infer literal as %t.", inferred_type_handle);

    uint32_t index = 0;

    uint8_t base = 10;
    if (integer.length >= index + 2)
    {
        if (integer.text[index] == '0')
        {
            if (integer.text[index + 1] == 'b')
            {
                base = 2;
                index += 2;
            }
            else if (integer.text[index + 1] == 'x')
            {
                base = 16;
                index += 2;
            }
        }
    }

    bool is_zero = true;
    while (index < integer.length)
    {
        if (integer.text[index] != '_' && integer.text[index] != '0')
        {
            is_zero = false;
            break;
        }

        index++;
    }

    uint64_t current_value = 0;
    for (; index < integer.length; index++)
    {
        if (integer.text[index] == '_')
            continue;

        uint8_t digit = integer.text[index] - (integer.text[index] <= '9' ? '0' : 'A' - 10);
        uint64_t new_value = current_value * base + digit;

        if (inferred_type_tag == Type_Tag_i8 && new_value > INT8_MAX ||
            inferred_type_tag == Type_Tag_i16 && new_value > INT16_MAX ||
            inferred_type_tag == Type_Tag_i32 && new_value > INT32_MAX ||
            inferred_type_tag == Type_Tag_i64 && new_value > INT64_MAX ||
            inferred_type_tag == Type_Tag_u8  && new_value > UINT8_MAX ||
            inferred_type_tag == Type_Tag_u16 && new_value > UINT16_MAX ||
            inferred_type_tag == Type_Tag_u32 && new_value > UINT32_MAX ||
            inferred_type_tag == Type_Tag_u64 && new_value < current_value)
            print_span_error(file, integer_span, "%S overflows %t.", integer, inferred_type_handle);

        current_value = new_value;
    }

    Number number;
    if (inferred_type_tag == Type_Tag_i8)
        number.i8 = (int8_t)current_value;
    else if (inferred_type_tag == Type_Tag_i16)
        number.i16 = (int16_t)current_value;
    else if (inferred_type_tag == Type_Tag_i32)
        number.i32 = (int32_t)current_value;
    else if (inferred_type_tag == Type_Tag_i64)
        number.i64 = (int64_t)current_value;
    else
        number.u64 = current_value;

    return number;
}
