#include "expression.h"
#include <string.h>

void type_check_field_access(const File* file, Field_Access* access, Expression_Check_Flags flags)
{
    if (flags & Expression_Check_Flags_constant)
        print_span_error(file, access->span, "Field access is not constant.");

    Expression* compound = lookup(access->compound);
    type_check_expression(file, compound, invalid_type_handle, flags);
    
    Qualified_Type* qualified_type = &lookup(compound->type)->qualified;    
    Type* unqualified_type = unqualified(compound->type);
    
    if (unqualified_type->tag == Type_Tag_pointer)
    {
        Type* base_type = lookup(unqualified_type->pointer.base_type);
        if (base_type->tag != Type_Tag_compound && base_type->tag != Type_Tag_fixed_array && base_type->tag != Type_Tag_dynamic_array)
            print_span_error(file, compound->span, "%t is not a pointer to a compound or array type.", compound->type);

        unqualified_type = base_type;
    }

    if (unqualified_type->tag == Type_Tag_compound)
        access->type = lookup_field_by_name(&unqualified_type->compound, access->field, access->field_span)->type;
    else if (unqualified_type->tag == Type_Tag_fixed_array)
    {
        if (compare(access->field, length_field))
        {
            access->type = u32_handle;
            access->tag = Expression_Tag_fixed_array_length;
        }
        else
            print_span_error(file, access->field_span, "length expected.");
    }
    else if (unqualified_type->tag == Type_Tag_dynamic_array)
    {
        if (compare(access->field, length_field))
        {
            access->type = u32_handle; 
            access->tag = Expression_Tag_dynamic_array_length;
        }
        else if (compare(access->field, capacity_field))
        {
            access->type = u32_handle;
            access->tag = Expression_Tag_dynamic_array_capacity;
        }
        else if (compare(access->field, elements_field))
        {
            access->type = find_or_add_pointer_type(unqualified_type->dynamic_array.base_type);
            access->tag = Expression_Tag_dynamic_array_elements;
        }
        else
            print_span_error(file, access->field_span, "length, capacity or elements expected.");
    }
    else
        print_span_error(file, compound->span, "%t is not a compound type.", compound->type);
    
    if (qualified_type->tag == Type_Tag_qualified)
    {
        access->type = find_or_add_qualified_type
        (
            lookup(access->type)->is_pointerless ? qualified_type->qualifiers & ~Qualifier_noalias : 
                                                   qualified_type->qualifiers,
            access->type
        );
    }  
}

void deep_copy_field_access(Field_Access* restrict destination, const Field_Access* restrict source)
{
    destination->compound = add_expression();
    deep_copy_expression(lookup(destination->compound), lookup(source->compound));

    destination->field = source->field;
}
