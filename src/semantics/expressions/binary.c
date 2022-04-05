#include "expression.h"
#include <math.h>

static void fold_binary(const File* file, Expression* expression)
{
    Type_Tag tag = lookup_type(expression->type)->tag;
    uint8_t operator = expression->binary.operator;

    Expression* lhs = lookup_expression(expression->binary.lhs);
    Expression* rhs = lookup_expression(expression->binary.rhs);

    if (operator == Binary_Operator_logical_or)
        expression->tag = lhs->tag == Expression_Tag_true || rhs->tag == Expression_Tag_true ? Expression_Tag_true : Expression_Tag_false;
    else if (operator == Binary_Operator_logical_and)
        expression->tag = lhs->tag == Expression_Tag_true && rhs->tag == Expression_Tag_true ? Expression_Tag_true : Expression_Tag_false;
    else if (operator == Binary_Operator_equal_equal)
    {
        if (tag == Type_Tag_f32)
            expression->tag = lhs->number.value.f32 == rhs->number.value.f32 ? Expression_Tag_true : Expression_Tag_false;
        else if (tag == Type_Tag_f64)
            expression->tag = lhs->number.value.f64 == rhs->number.value.f64 ? Expression_Tag_true : Expression_Tag_false;
        else if (tag == Type_Tag_bool)
            expression->tag = lhs->tag == rhs->tag ? Expression_Tag_true : Expression_Tag_false;
        else if (tag == Type_Tag_enumeration)
        {
            Enumeration_Type* enumeration_type = &lookup_type(expression->type)->enumeration;
            Number lhs_value = lookup_constant_by_name(enumeration_type, lhs->enumeration_constant_access.constant, lhs->enumeration_constant_access.constant_span)->value;
            Number rhs_value = lookup_constant_by_name(enumeration_type, rhs->enumeration_constant_access.constant, rhs->enumeration_constant_access.constant_span)->value;

            expression->tag = lhs_value.u64 == rhs_value.u64 ? Expression_Tag_true : Expression_Tag_false;
        }
        else
            expression->tag = lhs->number.value.u64 == rhs->number.value.u64 ? Expression_Tag_true : Expression_Tag_false;
    }
    else if (operator == Binary_Operator_not_equal)
    {
        if (tag == Type_Tag_f32)
            expression->tag = lhs->number.value.f32 != rhs->number.value.f32 ? Expression_Tag_true : Expression_Tag_false;
        else if (tag == Type_Tag_f64)
            expression->tag = lhs->number.value.f64 != rhs->number.value.f64 ? Expression_Tag_true : Expression_Tag_false;
        else if (tag == Type_Tag_bool)
            expression->tag = lhs->tag != rhs->tag ? Expression_Tag_true : Expression_Tag_false;
        else if (tag == Type_Tag_enumeration)
        {
            Enumeration_Type* enumeration_type = &lookup_type(expression->type)->enumeration;
            Number lhs_value = lookup_constant_by_name(enumeration_type, lhs->enumeration_constant_access.constant, lhs->enumeration_constant_access.constant_span)->value;
            Number rhs_value = lookup_constant_by_name(enumeration_type, rhs->enumeration_constant_access.constant, rhs->enumeration_constant_access.constant_span)->value;

            expression->tag = lhs_value.u64 != rhs_value.u64 ? Expression_Tag_true : Expression_Tag_false;
        }
        else
            expression->tag = lhs->number.value.u64 != rhs->number.value.u64 ? Expression_Tag_true : Expression_Tag_false;
    }
    else if (operator == Binary_Operator_less_than_or_equal)
    {
        if (tag == Type_Tag_f32)
            expression->tag = lhs->number.value.f32 <= rhs->number.value.f32 ? Expression_Tag_true : Expression_Tag_false;
        else if (tag == Type_Tag_f64)
            expression->tag = lhs->number.value.f64 <= rhs->number.value.f64 ? Expression_Tag_true : Expression_Tag_false;
        else if (tag == Type_Tag_bool)
            expression->tag = lhs->tag <= rhs->tag ? Expression_Tag_true : Expression_Tag_false;
        else if (tag == Type_Tag_enumeration)
        {
            Enumeration_Type* enumeration_type = &lookup_type(expression->type)->enumeration;
            Number lhs_value = lookup_constant_by_name(enumeration_type, lhs->enumeration_constant_access.constant, lhs->enumeration_constant_access.constant_span)->value;
            Number rhs_value = lookup_constant_by_name(enumeration_type, rhs->enumeration_constant_access.constant, rhs->enumeration_constant_access.constant_span)->value;

            expression->tag = lhs_value.u64 <= rhs_value.u64 ? Expression_Tag_true : Expression_Tag_false;
        }
        else
            expression->tag = lhs->number.value.u64 <= rhs->number.value.u64 ? Expression_Tag_true : Expression_Tag_false;
    }
    else if (operator == Binary_Operator_greater_than_or_equal)
    {
        if (tag == Type_Tag_f32)
            expression->tag = lhs->number.value.f32 >= rhs->number.value.f32 ? Expression_Tag_true : Expression_Tag_false;
        else if (tag == Type_Tag_f64)
            expression->tag = lhs->number.value.f64 >= rhs->number.value.f64 ? Expression_Tag_true : Expression_Tag_false;
        else if (tag == Type_Tag_bool)
            expression->tag = lhs->tag >= rhs->tag ? Expression_Tag_true : Expression_Tag_false;
        else if (tag == Type_Tag_enumeration)
        {
            Enumeration_Type* enumeration_type = &lookup_type(expression->type)->enumeration;
            Number lhs_value = lookup_constant_by_name(enumeration_type, lhs->enumeration_constant_access.constant, lhs->enumeration_constant_access.constant_span)->value;
            Number rhs_value = lookup_constant_by_name(enumeration_type, rhs->enumeration_constant_access.constant, rhs->enumeration_constant_access.constant_span)->value;

            expression->tag = lhs_value.u64 >= rhs_value.u64 ? Expression_Tag_true : Expression_Tag_false;
        }
        else
            expression->tag = lhs->number.value.u64 >= rhs->number.value.u64 ? Expression_Tag_true : Expression_Tag_false;
    }
    else if (operator == Binary_Operator_less_than)
    {
        if (tag == Type_Tag_f32)
            expression->tag = lhs->number.value.f32 < rhs->number.value.f32 ? Expression_Tag_true : Expression_Tag_false;
        else if (tag == Type_Tag_f64)
            expression->tag = lhs->number.value.f64 < rhs->number.value.f64 ? Expression_Tag_true : Expression_Tag_false;
        else if (tag == Type_Tag_bool)
            expression->tag = lhs->tag < rhs->tag ? Expression_Tag_true : Expression_Tag_false;
        else if (tag == Type_Tag_enumeration)
        {
            Enumeration_Type* enumeration_type = &lookup_type(expression->type)->enumeration;
            Number lhs_value = lookup_constant_by_name(enumeration_type, lhs->enumeration_constant_access.constant, lhs->enumeration_constant_access.constant_span)->value;
            Number rhs_value = lookup_constant_by_name(enumeration_type, rhs->enumeration_constant_access.constant, rhs->enumeration_constant_access.constant_span)->value;

            expression->tag = lhs_value.u64 < rhs_value.u64 ? Expression_Tag_true : Expression_Tag_false;
        }
        else
            expression->tag = lhs->number.value.u64 < rhs->number.value.u64 ? Expression_Tag_true : Expression_Tag_false;
    }
    else if (operator == Binary_Operator_greater_than)
    {
        if (tag == Type_Tag_f32)
            expression->tag = lhs->number.value.f32 > rhs->number.value.f32 ? Expression_Tag_true : Expression_Tag_false;
        else if (tag == Type_Tag_f64)
            expression->tag = lhs->number.value.f64 > rhs->number.value.f64 ? Expression_Tag_true : Expression_Tag_false;
        else if (tag == Type_Tag_bool)
            expression->tag = lhs->tag > rhs->tag ? Expression_Tag_true : Expression_Tag_false;
        else if (tag == Type_Tag_enumeration)
        {
            Enumeration_Type* enumeration_type = &lookup_type(expression->type)->enumeration;
            Number lhs_value = lookup_constant_by_name(enumeration_type, lhs->enumeration_constant_access.constant, lhs->enumeration_constant_access.constant_span)->value;
            Number rhs_value = lookup_constant_by_name(enumeration_type, rhs->enumeration_constant_access.constant, rhs->enumeration_constant_access.constant_span)->value;

            expression->tag = lhs_value.u64 > rhs_value.u64 ? Expression_Tag_true : Expression_Tag_false;
        }
        else
            expression->tag = lhs->number.value.u64 > rhs->number.value.u64 ? Expression_Tag_true : Expression_Tag_false;
    }
    else if (operator == Binary_Operator_plus)
    {
        if (tag >= Type_Tag_u8 && tag <= Type_Tag_u64)
        {
            expression->number.value.u64 = lhs->number.value.u64 + lhs->number.value.u64;
            if (tag == Type_Tag_u8 && expression->number.value.u64 > UINT8_MAX ||
                tag == Type_Tag_u16 && expression->number.value.u64 > UINT16_MAX ||
                tag == Type_Tag_u32 && expression->number.value.u64 > UINT32_MAX ||
                lhs->number.value.u64 > expression->number.value.u64)
                print_span_error(file, expression->span, "%t overflowed.", expression->type);

            expression->tag = Expression_Tag_integer;
        }
        else if (tag == Type_Tag_i8)
        {
            if (lhs->number.value.i8 > 0 && rhs->number.value.i8 > INT8_MAX - lhs->number.value.i8)
                print_span_error(file, expression->span, "%t overflowed.", expression->type);

            expression->number.value.i8 = lhs->number.value.i8 + lhs->number.value.i8;
            expression->tag = Expression_Tag_integer;
        }
        else if (tag == Type_Tag_i16)
        {
            if (lhs->number.value.i16 > 0 && rhs->number.value.i16 > INT16_MAX - lhs->number.value.i16)
                print_span_error(file, expression->span, "%t overflowed.", expression->type);

            expression->number.value.i16 = lhs->number.value.i16 + lhs->number.value.i16;
            expression->tag = Expression_Tag_integer;
        }
        else if (tag == Type_Tag_i32)
        {
            if (lhs->number.value.i32 > 0 && rhs->number.value.i32 > INT32_MAX - lhs->number.value.i32)
                print_span_error(file, expression->span, "%t overflowed.", expression->type);

            expression->number.value.i32 = lhs->number.value.i32 + lhs->number.value.i32;
            expression->tag = Expression_Tag_integer;
        }
        else if (tag == Type_Tag_i64)
        {
            if (lhs->number.value.i64 > 0 && rhs->number.value.i64 > INT64_MAX - lhs->number.value.i64)
                print_span_error(file, expression->span, "%t overflowed.", expression->type);

            expression->number.value.i64 = lhs->number.value.i64 + lhs->number.value.i64;
            expression->tag = Expression_Tag_integer;
        }
        else if (tag == Type_Tag_f32)
        {
            expression->number.value.f32 = lhs->number.value.f32 + rhs->number.value.f32;
            expression->tag = Expression_Tag_float;
        }
        else if (tag == Type_Tag_f64)
        {
            expression->number.value.f64 = lhs->number.value.f64 + rhs->number.value.f64;
            expression->tag = Expression_Tag_float;
        }
        else
            unexpected_type(__FILE__, __LINE__, tag);
    }
    else if (operator == Binary_Operator_minus)
    {
        if (tag >= Type_Tag_u8 && tag <= Type_Tag_u64)
        {
            expression->number.value.u64 = lhs->number.value.u64 - lhs->number.value.u64;
            expression->tag = Expression_Tag_integer;
        }
        else if (tag == Type_Tag_i8)
        {
            if (lhs->number.value.i8 < 0 && rhs->number.value.i8 < INT8_MAX - lhs->number.value.i8)
                print_span_error(file, expression->span, "%t underflowed.", expression->type);

            expression->number.value.i8 = lhs->number.value.i8 - lhs->number.value.i8;
            expression->tag = Expression_Tag_integer;
        }
        else if (tag == Type_Tag_i16)
        {
            if (lhs->number.value.i16 < 0 && rhs->number.value.i16 < INT16_MAX - lhs->number.value.i16)
                print_span_error(file, expression->span, "%t underflowed.", expression->type);

            expression->number.value.i16 = lhs->number.value.i16 - lhs->number.value.i16;
            expression->tag = Expression_Tag_integer;
        }
        else if (tag == Type_Tag_i32)
        {
            if (lhs->number.value.i32 < 0 && rhs->number.value.i32 < INT32_MAX - lhs->number.value.i32)
                print_span_error(file, expression->span, "%t underflowed.", expression->type);

            expression->number.value.i32 = lhs->number.value.i32 - lhs->number.value.i32;
            expression->tag = Expression_Tag_integer;
        }
        else if (tag == Type_Tag_i64)
        {
            if (lhs->number.value.i64 < 0 && rhs->number.value.i64 < INT64_MAX - lhs->number.value.i64)
                print_span_error(file, expression->span, "%t underflowed.", expression->type);

            expression->number.value.i64 = lhs->number.value.i64 - lhs->number.value.i64;
            expression->tag = Expression_Tag_integer;
        }
        else if (tag == Type_Tag_f32)
        {
            expression->number.value.f32 = lhs->number.value.f32 - rhs->number.value.f32;
            expression->tag = Expression_Tag_float;
        }
        else if (tag == Type_Tag_f64)
        {
            expression->number.value.f64 = lhs->number.value.f64 - rhs->number.value.f64;
            expression->tag = Expression_Tag_float;
        }
        else
            unexpected_type(__FILE__, __LINE__, tag);
    }
    else if (operator == Binary_Operator_bitwise_or)
    {
        expression->number.value.u64 = lhs->number.value.u64 | rhs->number.value.u64;
        expression->tag = Expression_Tag_integer;
    }
    else if (operator == Binary_Operator_bitwise_xor)
    {
        expression->number.value.u64 = lhs->number.value.u64 ^ rhs->number.value.u64;
        expression->tag = Expression_Tag_integer;
    }
    else if (operator == Binary_Operator_multiply)
    {
        if (tag == Type_Tag_f32)
        {
            expression->number.value.f32 = lhs->number.value.f32 * rhs->number.value.f32;
            expression->tag = Expression_Tag_float;
        }
        else if (tag == Type_Tag_f64)
        {
            expression->number.value.f64 = lhs->number.value.f64 * rhs->number.value.f64;
            expression->tag = Expression_Tag_float;
        }
        else
        {
            if (tag == Type_Tag_u8)
                expression->number.value.u8 = lhs->number.value.u8 * rhs->number.value.u8;
            else if (tag == Type_Tag_u16)
                expression->number.value.u16 = lhs->number.value.u16 * rhs->number.value.u16;
            else if (tag == Type_Tag_u32)
                expression->number.value.u32 = lhs->number.value.u32 * rhs->number.value.u32;
            else if (tag == Type_Tag_u64)
                expression->number.value.u64 = lhs->number.value.u64 * rhs->number.value.u64;
            else if (tag == Type_Tag_i8)
                expression->number.value.i8 = lhs->number.value.i8 * rhs->number.value.i8;
            else if (tag == Type_Tag_i16)
                expression->number.value.i16 = lhs->number.value.i16 * rhs->number.value.i16;
            else if (tag == Type_Tag_i32)
                expression->number.value.i32 = lhs->number.value.i32 * rhs->number.value.i32;
            else if (tag == Type_Tag_i64)
                expression->number.value.i64 = lhs->number.value.i64 * rhs->number.value.i64;
            else
                unexpected_type(__FILE__, __LINE__, tag);

            expression->tag = Expression_Tag_integer;
        }
    }
    else if (operator == Binary_Operator_divide)
    {
        if (tag <= Type_Tag_u64)
        {
            if (tag >= Type_Tag_u8 && tag <= Type_Tag_u64)
                expression->number.value.u64 = lhs->number.value.u64 / rhs->number.value.u64;
            else if (tag == Type_Tag_i8)
                expression->number.value.i8 = lhs->number.value.i8 / rhs->number.value.i8;
            else if (tag == Type_Tag_i16)
                expression->number.value.i16 = lhs->number.value.i16 / rhs->number.value.i16;
            else if (tag == Type_Tag_i32)
                expression->number.value.i32 = lhs->number.value.i32 / rhs->number.value.i32;
            else if (tag == Type_Tag_i64)
                expression->number.value.i64 = lhs->number.value.i64 / rhs->number.value.i64;

            expression->tag = Expression_Tag_integer;
        }
        else if (tag == Type_Tag_f32)
        {
            expression->number.value.f32 = lhs->number.value.f32 / rhs->number.value.f32;
            expression->tag = Expression_Tag_float;
        }
        else if (tag == Type_Tag_f64)
        {
            expression->number.value.f64 = lhs->number.value.f64 / rhs->number.value.f64;
            expression->tag = Expression_Tag_float;
        }
        else
            unexpected_type(__FILE__, __LINE__, tag);
    }
    else if (operator == Binary_Operator_modulo)
    {
        if (tag <= Type_Tag_u8 && tag >= Type_Tag_u64)
            expression->number.value.u64 = lhs->number.value.u64 % rhs->number.value.u64;
        else if (tag == Type_Tag_i8)
            expression->number.value.i8 = lhs->number.value.i8 % rhs->number.value.i8;
        else if (tag == Type_Tag_i16)
            expression->number.value.i16 = lhs->number.value.i16 % rhs->number.value.i16;
        else if (tag == Type_Tag_i32)
            expression->number.value.i32 = lhs->number.value.i32 % rhs->number.value.i32;
        else if (tag == Type_Tag_i64)
            expression->number.value.i64 = lhs->number.value.i64 % rhs->number.value.i64;
        else if (tag == Type_Tag_f32)
            expression->number.value.f32 = (float)fmod(lhs->number.value.f32, rhs->number.value.f32);
        else if (tag == Type_Tag_f64)
            expression->number.value.f64 = fmod(lhs->number.value.f64, rhs->number.value.f64);
        else
            unexpected_type(__FILE__, __LINE__, tag);

        expression->tag = Expression_Tag_integer;
    }
    else if (operator == Binary_Operator_bitwise_and)
    {
        expression->number.value.u64 = lhs->number.value.u64 & rhs->number.value.u64;
        expression->tag = Expression_Tag_integer;
    }
    else if (operator == Binary_Operator_left_shift)
    {
        if (tag == Type_Tag_u8)
            expression->number.value.u8 = lhs->number.value.u8 << rhs->number.value.u8;
        else if (tag == Type_Tag_u16)
            expression->number.value.u16 = lhs->number.value.u16 << rhs->number.value.u16;
        else if (tag == Type_Tag_u32)
            expression->number.value.u32 = lhs->number.value.u32 << rhs->number.value.u32;
        else if (tag == Type_Tag_u64)
            expression->number.value.u64 = lhs->number.value.u64 << rhs->number.value.u64;
        else if (tag == Type_Tag_i8)
            expression->number.value.i8 = lhs->number.value.i8 << rhs->number.value.i8;
        else if (tag == Type_Tag_i16)
            expression->number.value.i16 = lhs->number.value.i16 << rhs->number.value.i16;
        else if (tag == Type_Tag_i32)
            expression->number.value.i32 = lhs->number.value.i32 << rhs->number.value.i32;
        else if (tag == Type_Tag_i64)
            expression->number.value.i64 = lhs->number.value.i64 << rhs->number.value.i64;
        else
            unexpected_type(__FILE__, __LINE__, tag);

        expression->tag = Expression_Tag_integer;
    }
    else if (operator == Binary_Operator_right_shift)
    {
        if (tag == Type_Tag_u8)
            expression->number.value.u8 = lhs->number.value.u8 >> rhs->number.value.u8;
        else if (tag == Type_Tag_u16)
            expression->number.value.u16 = lhs->number.value.u16 >> rhs->number.value.u16;
        else if (tag == Type_Tag_u32)
            expression->number.value.u32 = lhs->number.value.u32 >> rhs->number.value.u32;
        else if (tag == Type_Tag_u64)
            expression->number.value.u64 = lhs->number.value.u64 >> rhs->number.value.u64;
        else if (tag == Type_Tag_i8)
            expression->number.value.i8 = lhs->number.value.i8 >> rhs->number.value.i8;
        else if (tag == Type_Tag_i16)
            expression->number.value.i16 = lhs->number.value.i16 >> rhs->number.value.i16;
        else if (tag == Type_Tag_i32)
            expression->number.value.i32 = lhs->number.value.i32 >> rhs->number.value.i32;
        else if (tag == Type_Tag_i64)
            expression->number.value.i64 = lhs->number.value.i64 >> rhs->number.value.i64;
        else
            unexpected_type(__FILE__, __LINE__, tag);

        expression->tag = Expression_Tag_integer;
    }
}

void type_check_binary(const File* file, Binary* binary, Type_Handle inferred_type_handle, Expression_Check_Flags flags)
{
    Expression* lhs = lookup_expression(binary->lhs);
    Expression* rhs = lookup_expression(binary->rhs);
    
    bool lhs_is_inferable = lhs->tag == Expression_Tag_integer || lhs->tag == Expression_Tag_float;
    bool rhs_is_inferable = rhs->tag == Expression_Tag_integer || rhs->tag == Expression_Tag_float;
    
    if (lhs_is_inferable && !rhs_is_inferable)
    {
        type_check_expression(file, rhs, invalid_type_handle, flags);
        type_check_expression(file, lhs, rhs->type, flags);
    }
    else if (!lhs_is_inferable && rhs_is_inferable)
    {
        type_check_expression(file, lhs, invalid_type_handle, flags);
        type_check_expression(file, rhs, lhs->type, flags);
    }
    else
    {
        type_check_expression(file, lhs, inferred_type_handle, flags);
        type_check_expression(file, rhs, inferred_type_handle, flags);
    }

    Type* lhs_type = lookup_type(lhs->type);
    Type* rhs_type = lookup_type(rhs->type);
    bool operator_is_defined = false;
    
    if (!expression_types_match(lhs->type, rhs->type))
    {
        if (binary->operator == Binary_Operator_plus || binary->operator == Binary_Operator_minus)
            operator_is_defined = lhs_type->tag == Type_Tag_pointer && rhs_type->tag >= Type_Tag_u8 && rhs_type->tag <= Type_Tag_u64 ||
                                  rhs_type->tag == Type_Tag_pointer && lhs_type->tag >= Type_Tag_u8 && lhs_type->tag <= Type_Tag_u64;
    }
    else
    {
        switch (binary->operator)
        {
            case Binary_Operator_logical_and:
            case Binary_Operator_logical_or:
                operator_is_defined = lhs_type->tag == Type_Tag_bool;
                break;

            case Binary_Operator_less_than_or_equal:
            case Binary_Operator_greater_than_or_equal:
            case Binary_Operator_less_than:
            case Binary_Operator_greater_than:
                operator_is_defined = lhs_type->tag <= Type_Tag_f64 || 
                                      lhs_type->tag == Type_Tag_enumeration || 
                                      lhs_type->tag == Type_Tag_pointer;

                break;

            case Binary_Operator_equal_equal:
            case Binary_Operator_not_equal:
                operator_is_defined = lhs_type->tag <= Type_Tag_bool || 
                                      lhs_type->tag == Type_Tag_enumeration || 
                                      lhs_type->tag == Type_Tag_pointer;

                break;

            case Binary_Operator_plus:
            case Binary_Operator_minus:
            case Binary_Operator_multiply:
            case Binary_Operator_divide:
            case Binary_Operator_modulo:
                operator_is_defined = lhs_type->tag <= Type_Tag_f64;
                break;

            case Binary_Operator_bitwise_or:
            case Binary_Operator_bitwise_xor:
            case Binary_Operator_bitwise_and:
            case Binary_Operator_left_shift:
            case Binary_Operator_right_shift:
                operator_is_defined = lhs_type->tag <= Type_Tag_u64 || 
                                      lhs_type->tag == Type_Tag_enumeration;

                break;
        }
    }

    if (operator_is_defined)
    {
        if (binary->operator <= Binary_Operator_greater_than)
            binary->type = bool_handle;
        else if (lhs_type->tag != Type_Tag_pointer && rhs_type->tag == Type_Tag_pointer)
            // This binary expression is doing pointer arithmetic.
            binary->type = rhs->type;
        else
            // And the pointer arithmetic case is implicitly handled here.
            binary->type = lhs->type;
    }
    else
        print_span_error(file, binary->operator_span, "Operator not defined for %t and %t.", lhs->type, rhs->type);

    if (flags & Expression_Check_Flags_constant)
        fold_binary(file, (Expression*)binary);
}

void deep_copy_binary(Binary* restrict destination, const Binary* restrict source)
{
    destination->operator = source->operator;
    destination->operator_span = source->operator_span;

    destination->lhs = add_expression();
    deep_copy_expression(lookup_expression(destination->lhs), lookup_expression(source->lhs));

    destination->rhs = add_expression();
    deep_copy_expression(lookup_expression(destination->rhs), lookup_expression(source->rhs));
}
