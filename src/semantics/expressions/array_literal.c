#include "expression.h"

#include <inttypes.h>

void type_check_array_literal(const File* file, Array_Literal* literal, Type_Handle inferred_type_handle, Expression_Check_Flags flags)
{
    if (is_invalid_type_handle(inferred_type_handle))
        print_span_error(file, literal->span, "Array type cannot be inferred.");

    Fixed_Array_Type* inferred_type = &lookup_type(inferred_type_handle)->fixed_array;
    if (inferred_type->tag != Type_Tag_fixed_array)
        print_span_error(file, literal->span, "Cannot infer literal as %t.", inferred_type_handle);

    uint32_t next_element_index = 0;
    for (uint32_t a = 0; a < literal->elements.handles_length; a++)
    {
        Element_Initializer* a_element = lookup_element_initializer(literal->elements.handles[a]);
        if (is_invalid_expression_handle(a_element->explicit_index))
        {
            if (next_element_index >= inferred_type->size)
                print_span_error(file, lookup_expression(a_element->expression)->span, "%" PRIu32 " is out of bounds in %t.", next_element_index, inferred_type_handle);

            a_element->index = next_element_index;
        }
        else
        {
            Expression* explicit_index = lookup_expression(a_element->explicit_index);
            if (explicit_index->tag == Expression_Tag_enumeration_constant_access)
            {
                Enumeration_Type* enumeration_type = &lookup_type(type_check_expression(file, explicit_index, invalid_type_handle, flags))->enumeration;
                Constant* constant = lookup_constant_by_name(enumeration_type, explicit_index->enumeration_constant_access.constant, explicit_index->enumeration_constant_access.constant_span);

                Type* underlying_type = lookup_type(enumeration_type->underlying_type);
                if (underlying_type->tag == Type_Tag_i8)
                {
                    if (constant->value.i8 < 0 || (uint32_t)constant->value.i8 >= inferred_type->size)
                        print_span_error(file, explicit_index->span, "%" PRIi8 " is out of bounds in %t.", constant->value.i8, inferred_type_handle);

                    a_element->index = (uint32_t)constant->value.i8;
                }
                else if (underlying_type->tag == Type_Tag_i16)
                {
                    if (constant->value.i16 < 0 || (uint32_t)constant->value.i16 >= inferred_type->size)
                        print_span_error(file, explicit_index->span, "%" PRIi16 " is out of bounds in %t.", constant->value.i16, inferred_type_handle);

                    a_element->index = (uint32_t)constant->value.i16;
                }
                else if (underlying_type->tag == Type_Tag_i32)
                {
                    if (constant->value.i32 < 0 || (uint32_t)constant->value.i32 >= inferred_type->size)
                        print_span_error(file, explicit_index->span, "%" PRIi32 " is out of bounds in %t.", constant->value.i32, inferred_type_handle);

                    a_element->index = (uint32_t)constant->value.i32;
                }
                else if (underlying_type->tag == Type_Tag_i64)
                {
                    if (constant->value.i64 < 0 || constant->value.i64 >= (int64_t)inferred_type->size)
                        print_span_error(file, explicit_index->span, "%" PRIi64 " is out of bounds in %t.", constant->value.i64, inferred_type_handle);

                    a_element->index = (uint32_t)constant->value.i64;
                }
                else if (constant->value.u64 >= inferred_type->size)
                    print_span_error(file, explicit_index->span, "%" PRIu64 " is out of bounds in %t.", constant->value.u64, inferred_type_handle);
                else
                    a_element->index = constant->value.u32;
            }
            else if (explicit_index->tag == Expression_Tag_integer)
            {
                type_check_expression(file, explicit_index, u32_handle, flags);
                if (explicit_index->number.value.u32 >= inferred_type->size)
                    print_span_error(file, explicit_index->span, "%" PRIu32 " is out of bounds in %t.", explicit_index->number.value.u32, inferred_type_handle);

                a_element->index = explicit_index->number.value.u32;
            }
            else
                ice(__FILE__, __LINE__, "Enumeration constant or integer literal expected.");
        }

        for (uint32_t b = 0; b < a; b++)
        {
            Element_Initializer* b_element = lookup_element_initializer(literal->elements.handles[b]);
            if (a_element->index == b_element->index)
                print_span_error(file, lookup_expression(b_element->expression)->span, "Element at index %u already initialized.", a_element->index);
        }

        Expression* element_expression = lookup_expression(a_element->expression);
        type_check_expression(file, element_expression, inferred_type->base_type, flags);
        if (!expression_types_match(inferred_type->base_type, element_expression->type))
            print_span_error(file, element_expression->span, "%t expected but found %t.", inferred_type->base_type, element_expression->type);

        next_element_index = a_element->index + 1;
        if (next_element_index < a_element->index && a != (uint32_t)literal->elements.handles_length - 1)
        {
            Element_Initializer* next_element = lookup_element_initializer(literal->elements.handles[a + 1]);
            if (is_invalid_expression_handle(next_element->explicit_index))
                print_span_error(file, lookup_expression(next_element->expression)->span, "%" PRIu64 " overflows U32.", (uint64_t)a_element->index + 1);
        }
    }

    literal->type = inferred_type_handle;
}

static void deep_copy_element_initializer(Element_Initializer* restrict destination, const Element_Initializer* restrict source)
{
    if (is_invalid_expression_handle(source->explicit_index))
        destination->explicit_index = source->explicit_index;
    else
    {
        destination->explicit_index = add_expression();
        deep_copy_expression(lookup_expression(destination->explicit_index), lookup_expression(source->explicit_index));
    }

    destination->expression = add_expression();
    deep_copy_expression(lookup_expression(destination->expression), lookup_expression(source->expression));
}

static void deep_copy_element_initializers(Element_Initializer_Handle_Array* restrict destination, const Element_Initializer_Handle_Array* restrict source)
{
    reserve_element_initializer_handles(destination, source->handles_length);
    for (uint8_t i = 0; i < source->handles_length; i++)
    {
        add_to_element_initializer_array(destination, add_element_initializer());
        deep_copy_element_initializer(lookup_element_initializer(destination->handles[i]), lookup_element_initializer(source->handles[i]));
    }
}

void deep_copy_array_literal(Array_Literal* restrict destination, const Array_Literal* restrict source)
{
    deep_copy_element_initializers(&destination->elements, &source->elements);
}
