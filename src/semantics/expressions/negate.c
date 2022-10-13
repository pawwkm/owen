#include "expression.h"
#include <inttypes.h>

void type_check_negate(const File* file, Negate* negate, Type_Handle inferred_type_handle, Expression_Check_Flags flags)
{
    Expression* sub_expression = lookup(negate->expression);
    flags |= Expression_Check_Flags_is_nested;

    if (invalid(inferred_type_handle))
    {
        if (sub_expression->tag == Expression_Tag_integer || sub_expression->tag == Expression_Tag_float)
            print_span_error(file, sub_expression->span, "Type cannot be inferred.");

        negate->type = type_check_expression(file, sub_expression, inferred_type_handle, flags);

        Type_Tag type_tag = lookup(negate->type)->tag;
        if (!(type_tag <= Type_Tag_i64 || type_tag == Type_Tag_f32 || type_tag == Type_Tag_f64))
            print_span_error(file, sub_expression->span, "Operator not defined for %t.", sub_expression->type);
    }
    else
    {
        Type_Tag type_tag = lookup(inferred_type_handle)->tag;
        if (type_tag <= Type_Tag_i64 && sub_expression->tag == Expression_Tag_integer)
        {
            negate->type = type_check_expression(file, sub_expression, u64_handle, flags);
            if (!expression_types_match(u64_handle, negate->type))
                print_span_error(file, negate->span, "%t expected but found %t.", u64_handle, negate->type);

            Number value;
            if (type_tag == Type_Tag_i8)
            {
                if (sub_expression->number.value.u64 > 0x80)
                    print_span_error(file, negate->span, "-%" PRIu64 " underflows %t.", sub_expression->number.value.u64, inferred_type_handle);

                value.i8 = -(int8_t)sub_expression->number.value.u64;
            }
            else if (type_tag == Type_Tag_i16)
            {
                if (sub_expression->number.value.u64 > 0x8000)
                    print_span_error(file, negate->span, "-%" PRIu64 " underflows %t.", sub_expression->number.value.u64, inferred_type_handle);

                value.i16 = -(int16_t)sub_expression->number.value.u64;
            }
            else if (type_tag == Type_Tag_i32)
            {
                if (sub_expression->number.value.u64 > 0x80000000)
                    print_span_error(file, negate->span, "-%" PRIu64 " underflows %t.", sub_expression->number.value.u64, inferred_type_handle);

                value.i32 = -(int32_t)sub_expression->number.value.u64;
            }
            else if (type_tag == Type_Tag_i64)
            {
                if (sub_expression->number.value.u64 > 0x8000000000000000)
                    print_span_error(file, negate->span, "-%" PRIu64 " underflows %t.", sub_expression->number.value.u64, inferred_type_handle);

                value.i64 = -(int64_t)sub_expression->number.value.u64;
            }

            *(Number_Literal*)negate = (Number_Literal)
            {
                .span = negate->span,
                .type = inferred_type_handle,
                .tag = Expression_Tag_integer,
                .value = value
            };
        }
        else if (type_tag == Type_Tag_f32 && sub_expression->tag == Expression_Tag_float)
        {
            type_check_expression(file, sub_expression, inferred_type_handle, flags);
            *(Number_Literal*)negate = (Number_Literal)
            {
                .span = negate->span,
                .type = inferred_type_handle,
                .tag = Expression_Tag_float,
                .value.f32 = -sub_expression->number.value.f32
            };
        }
        else if (type_tag == Type_Tag_f64 && sub_expression->tag == Expression_Tag_float)
        {
            type_check_expression(file, sub_expression, inferred_type_handle, flags);
            *(Number_Literal*)negate = (Number_Literal)
            {
                .span = negate->span,
                .type = inferred_type_handle,
                .tag = Expression_Tag_float,
                .value.f64 = -sub_expression->number.value.f64
            };
        }
        else
            negate->type = type_check_expression(file, sub_expression, inferred_type_handle, flags);

        if (type_tag <= Type_Tag_i64 || type_tag == Type_Tag_f32 || type_tag == Type_Tag_f64)
        {
            if (!expression_types_match(inferred_type_handle, negate->type))
                print_span_error(file, negate->span, "%t expected but found %t.", inferred_type_handle, negate->type);
        }
        else
            print_span_error(file, sub_expression->span, "Operator not defined for %t.", sub_expression->type);
    }
}
