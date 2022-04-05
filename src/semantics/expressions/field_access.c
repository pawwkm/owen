#include "expression.h"
#include <string.h>

void type_check_field_access(const File* file, Field_Access* access, Expression_Check_Flags flags)
{
    if (flags & Expression_Check_Flags_constant)
        print_span_error(file, access->span, "Field access is not constant.");

    Expression* compound = lookup_expression(access->compound);
    
    Type* type = lookup_type(type_check_expression(file, compound, invalid_type_handle, flags));
    if (type->tag == Type_Tag_compound)
        access->type = lookup_field_by_name(&type->compound, access->field, access->field_span)->type;
    else if (type->tag == Type_Tag_pointer)
    {
        Type* base_type = lookup_type(type->pointer.base_type);
        if (base_type->tag != Type_Tag_compound)
            print_span_error(file, compound->span, "%t is not a pointer to a compound type.", compound->type);
        else if (flags & Expression_Check_Flags_lhs_value && !(type->pointer.privileges & Pointer_Type_Privilege_writable))
            print_span_error(file, compound->span, "Missing writable privilege ($) from %t.", compound->type);
        else if (flags & Expression_Check_Flags_rhs_value && !(type->pointer.privileges & Pointer_Type_Privilege_readable))
            print_span_error(file, compound->span, "Missing readable privilege (?) from %t.", compound->type);

        access->type = lookup_field_by_name(&base_type->compound, access->field, access->field_span)->type;
    }
    else if (type->tag == Type_Tag_fixed_array)
    {
        if (compare_interned_strings(access->field, length_field))
        {
            access->type = u32_handle;
            access->tag = Expression_Tag_fixed_array_length;
        }
        else
            print_span_error(file, access->field_span, "length expected.");
    }
    else if (type->tag == Type_Tag_dynamic_array)
    {
        if (compare_interned_strings(access->field, length_field))
        {
            access->type = u32_handle; 
            access->tag = Expression_Tag_dynamic_array_length;
        }
        else if (compare_interned_strings(access->field, capacity_field))
        {
            access->type = u32_handle;
            access->tag = Expression_Tag_dynamic_array_capacity;
        }
        else if (compare_interned_strings(access->field, elements_field))
        {
            for (Type_Handle handle = { 0 }; handle.index < types_length; handle.index++)
            {
                Pointer_Type* t = &lookup_type(handle)->pointer;
                if (t->tag == Type_Tag_pointer && compare_types(t->base_type, type->dynamic_array.base_type))
                {
                    access->type = handle;
                    return;
                }
            }

            Type_Handle handle = add_type();
            Pointer_Type* pointer_type = &lookup_type(handle)->pointer;
            pointer_type->tag = Type_Tag_pointer;
            pointer_type->base_type = type->dynamic_array.base_type;

            access->type = handle;
            access->tag = Expression_Tag_dynamic_array_elements;
        }
        else
            print_span_error(file, access->field_span, "length, capacity or elements expected.");
    }
    else
        print_span_error(file, compound->span, "%t is not a compound type.", compound->type);
}

void deep_copy_field_access(Field_Access* restrict destination, const Field_Access* restrict source)
{
    destination->compound = add_expression();
    deep_copy_expression(lookup_expression(destination->compound), lookup_expression(source->compound));

    destination->field = source->field;
}
