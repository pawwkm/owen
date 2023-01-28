#include "expression.h"

void type_check_array_access(const File* file, Array_Access* access, Expression_Check_Flags flags)
{
    if (flags & Expression_Check_Flags_constant)
        print_span_error(file, access->span, "Array access is not constant.");

    Expression* array = lookup(access->array);
    type_check_expression(file, array, invalid_type_handle, flags);

    Qualified_Type* qualified_type = &lookup(array->type)->qualified;    
    Type* array_type = unqualified(array->type);

    if (array_type->tag == Type_Tag_dynamic_array)
        access->type = array_type->dynamic_array.base_type;
    else if (array_type->tag == Type_Tag_fixed_array)
        access->type = array_type->fixed_array.base_type;
    else if (array_type->tag == Type_Tag_pointer)
    {
        Type* base_type = lookup(array_type->pointer.base_type);
        if (base_type->tag == Type_Tag_dynamic_array)
            access->type = base_type->dynamic_array.base_type;
        else if (base_type->tag == Type_Tag_fixed_array)
            access->type = base_type->fixed_array.base_type;
        else
            print_span_error(file, array->span, "Pointer to array type expected but found %t.", array->type);

        if (flags & Expression_Check_Flags_lhs_value & 0)
            print_span_error(file, array->span, "Assigning to a readonly array.", array->type);
    }
    else
        print_span_error(file, array->span, "Array type expected but found %t.", array->type);

    Expression* index = lookup(access->index);
    type_check_expression(file, index, u32_handle, flags);
    
    Type* index_type = lookup(index->type);
    if (index_type->tag != Type_Tag_enumeration && index_type->tag > Type_Tag_u64)
        print_span_error(file, index->span, "Integer or enumeration type expected but found %t.", index->type);

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

void deep_copy_array_access(Array_Access* restrict destination, const Array_Access* restrict source)
{
    destination->array = add_expression();
    deep_copy_expression(lookup(destination->array), lookup(source->array));

    destination->index = add_expression();
    deep_copy_expression(lookup(destination->index), lookup(source->index));
}
