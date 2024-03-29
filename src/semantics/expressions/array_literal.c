#include "expression.h"

#include <inttypes.h>

void type_check_array_literal(const File* file, Array_Literal* literal, Type_Handle inferred_type_handle, Expression_Check_Flags flags)
{
    if (invalid(inferred_type_handle))
        print_span_error(file, literal->span, "Array type cannot be inferred.");

    Qualified_Type* qualified_type = &lookup(inferred_type_handle)->qualified;    
    Fixed_Array_Type* unqualified_array_type = &unqualified(inferred_type_handle)->fixed_array;
    Type_Handle qualified_base_type;
    
    if (qualified_type->tag == Type_Tag_qualified)
        qualified_base_type = find_or_add_qualified_type(qualified_type->qualifiers, unqualified_array_type->base_type);
    else
        qualified_base_type = unqualified_array_type->base_type;
        
    if (unqualified_array_type->tag != Type_Tag_fixed_array)
        print_span_error(file, literal->span, "Cannot infer literal as %t.", inferred_type_handle);

    uint32_t next_element_index = 0;
    for (Array_Size a = 0; a < literal->elements.handles_length; a++)
    {
        Element_Initializer* a_element = lookup_in(&literal->elements, a);
        if (invalid(a_element->explicit_index))
        {
            if (next_element_index >= unqualified_array_type->size)
                print_span_error(file, lookup(a_element->expression)->span, "%" PRIu32 " is out of bounds in %t.", next_element_index, inferred_type_handle);

            a_element->index = next_element_index;
        }
        else
        {
            Expression* explicit_index = lookup(a_element->explicit_index);
            if (explicit_index->tag == Expression_Tag_enumeration_constant_access)
            {
                Enumeration_Type* enumeration_type = &lookup(type_check_expression(file, explicit_index, invalid_type_handle, flags))->enumeration;
                Constant* constant = lookup_constant_by_name(enumeration_type, explicit_index->enumeration_constant_access.constant, explicit_index->enumeration_constant_access.constant_span);

                Type* underlying_type = lookup(enumeration_type->underlying_type);
                if (underlying_type->tag == Type_Tag_i8)
                {
                    if (constant->value.i8 < 0 || (uint32_t)constant->value.i8 >= unqualified_array_type->size)
                        print_span_error(file, explicit_index->span, "%" PRIi8 " is out of bounds in %t.", constant->value.i8, inferred_type_handle);

                    a_element->index = (uint32_t)constant->value.i8;
                }
                else if (underlying_type->tag == Type_Tag_i16)
                {
                    if (constant->value.i16 < 0 || (uint32_t)constant->value.i16 >= unqualified_array_type->size)
                        print_span_error(file, explicit_index->span, "%" PRIi16 " is out of bounds in %t.", constant->value.i16, inferred_type_handle);

                    a_element->index = (uint32_t)constant->value.i16;
                }
                else if (underlying_type->tag == Type_Tag_i32)
                {
                    if (constant->value.i32 < 0 || (uint32_t)constant->value.i32 >= unqualified_array_type->size)
                        print_span_error(file, explicit_index->span, "%" PRIi32 " is out of bounds in %t.", constant->value.i32, inferred_type_handle);

                    a_element->index = (uint32_t)constant->value.i32;
                }
                else if (underlying_type->tag == Type_Tag_i64)
                {
                    if (constant->value.i64 < 0 || constant->value.i64 >= (int64_t)unqualified_array_type->size)
                        print_span_error(file, explicit_index->span, "%" PRIi64 " is out of bounds in %t.", constant->value.i64, inferred_type_handle);

                    a_element->index = (uint32_t)constant->value.i64;
                }
                else if (constant->value.u64 >= unqualified_array_type->size)
                    print_span_error(file, explicit_index->span, "%" PRIu64 " is out of bounds in %t.", constant->value.u64, inferred_type_handle);
                else
                    a_element->index = constant->value.u32;
            }
            else if (explicit_index->tag == Expression_Tag_integer)
            {
                type_check_expression(file, explicit_index, u32_handle, flags);
                if (explicit_index->number.value.u32 >= unqualified_array_type->size)
                    print_span_error(file, explicit_index->span, "%" PRIu32 " is out of bounds in %t.", explicit_index->number.value.u32, inferred_type_handle);

                a_element->index = explicit_index->number.value.u32;
            }
            else
                ice(__FILE__, __LINE__, "Enumeration constant or integer literal expected.");
        }

        for (Array_Size b = 0; b < a; b++)
        {
            Element_Initializer* b_element = lookup_in(&literal->elements, b);
            if (a_element->index == b_element->index)
                print_span_error(file, lookup(b_element->expression)->span, "Element at index %u already initialized.", a_element->index);
        }

        Expression* element_expression = lookup(a_element->expression);
        type_check_expression(file, element_expression, qualified_base_type, flags);
        if (!expression_types_match(qualified_base_type, element_expression->type))
            print_span_error(file, element_expression->span, "%t expected but found %t.", qualified_base_type, element_expression->type);

        next_element_index = a_element->index + 1;
        if (next_element_index < a_element->index && a != (uint32_t)literal->elements.handles_length - 1)
        {
            Element_Initializer* next_element = lookup_in(&literal->elements, a + 1);
            if (invalid(next_element->explicit_index))
                print_span_error(file, lookup(next_element->expression)->span, "%" PRIu64 " overflows U32.", (uint64_t)a_element->index + 1);
        }
    }

    literal->type = inferred_type_handle;
}

static void deep_copy_element_initializer(Element_Initializer* restrict destination, const Element_Initializer* restrict source)
{
    if (invalid(source->explicit_index))
        destination->explicit_index = source->explicit_index;
    else
    {
        destination->explicit_index = add_expression();
        deep_copy_expression(lookup(destination->explicit_index), lookup(source->explicit_index));
    }

    destination->expression = add_expression();
    deep_copy_expression(lookup(destination->expression), lookup(source->expression));
}

static void deep_copy_element_initializers(Element_Initializer_Handle_Array* restrict destination, const Element_Initializer_Handle_Array* restrict source)
{
    reserve(destination, source->handles_length);
    for (Array_Size i = 0; i < source->handles_length; i++)
    {
        add_to(destination, add_element_initializer());
        deep_copy_element_initializer(lookup_in(destination, i), lookup_in(source, i));
    }
}

void deep_copy_array_literal(Array_Literal* restrict destination, const Array_Literal* restrict source)
{
    deep_copy_element_initializers(&destination->elements, &source->elements);
}
