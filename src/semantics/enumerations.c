#include "semantics.h"
#include <assert.h>

Constant* lookup_constant_by_name(const Enumeration_Type* enumeration, Interned_String_Handle name, Span name_span)
{
    for (Array_Size i = 0; i < enumeration->constants.handles_length; i++)
    {
        Constant* constant = lookup(handle_at(&enumeration->constants, i));
        if (compare(constant->name, name))
            return constant;
    }

    print_span_error(lookup(enumeration->file), name_span, "%e does not define a constant named %I.", enumeration, name);
    return NULL;
}

static void type_check_underlying_type_of_enumeration(const File* file, Enumeration_Type* enumeration)
{
    Type_Handle handle = lookup_type_by_reference(file, enumeration->underlying_type_reference, true);
    Primitive_Type* type = &lookup(handle)->primitive;
    if (type->tag > Type_Tag_u64)
        print_span_error(file, lookup(enumeration->underlying_type_reference)->span, "Expected an integer but found a %t.", handle);

    enumeration->underlying_type = handle;
    enumeration->allignment_of = type->allignment_of;
    enumeration->size_of = type->size_of;
}

static void assign_constant_values(const File* file, const Enumeration_Type* enumeration)
{
    Number previous_value = { 0 };
    for (Array_Size i = 0; i < enumeration->constants.handles_length; i++)
    {
        Constant* constant = lookup_in(&enumeration->constants, i);
        if (!invalid(constant->explicit_value))
        {
            Expression* explicit_value = lookup(constant->explicit_value);
            type_check_expression(file, explicit_value, enumeration->underlying_type, Expression_Check_Flags_constant);

            constant->value = explicit_value->number.value;
            previous_value = constant->value;
        }
        else
        {
            if (i)
            {
                Number previous_constant = lookup_in(&enumeration->constants, i - 1)->value;
                if (compare(enumeration->underlying_type, i8_handle)  && previous_constant.i8  == INT8_MAX   ||
                    compare(enumeration->underlying_type, i16_handle) && previous_constant.i16 == INT16_MAX  ||
                    compare(enumeration->underlying_type, i32_handle) && previous_constant.i32 == INT32_MAX  ||
                    compare(enumeration->underlying_type, i64_handle) && previous_constant.i64 == INT64_MAX  ||
                    compare(enumeration->underlying_type, u8_handle)  && previous_constant.u8  == UINT8_MAX  ||
                    compare(enumeration->underlying_type, u16_handle) && previous_constant.u16 == UINT16_MAX ||
                    compare(enumeration->underlying_type, u32_handle) && previous_constant.u32 == UINT32_MAX ||
                    compare(enumeration->underlying_type, u64_handle) && previous_constant.u64 == UINT64_MAX)
                    print_span_error(file, constant->name_span, "%I overflows %t.", constant->name, enumeration->underlying_type);

                if (compare(enumeration->underlying_type, i8_handle))
                    previous_value.i8++;
                else if (compare(enumeration->underlying_type, i16_handle))
                    previous_value.i16++;
                else if (compare(enumeration->underlying_type, i32_handle))
                    previous_value.i32++;
                else if (compare(enumeration->underlying_type, i64_handle))
                    previous_value.i64++;
                else
                    previous_value.u64++;
            }

            constant->value = previous_value;
        }
    }
}

void type_check_enumeration_declaration(Enumeration_Type* enumeration)
{
    File* file = lookup(enumeration->file);
    type_check_underlying_type_of_enumeration(file, enumeration);
    assign_constant_values(file, enumeration);
}
