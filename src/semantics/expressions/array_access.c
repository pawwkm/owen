#include "expression.h"

void type_check_array_access(const File* file, Array_Access* access, Expression_Check_Flags flags)
{
    if (flags & Expression_Check_Flags_constant)
        print_span_error(file, access->span, "Array access is not constant.");

    Expression* array = lookup_expression(access->array);
    type_check_expression(file, array, invalid_type_handle, flags);

    Type* array_type = lookup_type(array->type);
    if (array_type->tag == Type_Tag_dynamic_array)
        access->type = array_type->dynamic_array.base_type;
    else if (array_type->tag == Type_Tag_fixed_array)
        access->type = array_type->fixed_array.base_type;
    else if (array_type->tag == Type_Tag_pointer)
    {
        Type* base_type = lookup_type(array_type->pointer.base_type);
        if (base_type->tag == Type_Tag_dynamic_array)
            access->type = base_type->dynamic_array.base_type;
        else if (base_type->tag == Type_Tag_fixed_array)
            access->type = base_type->fixed_array.base_type;
        else
            print_span_error(file, array->span, "Pointer to array type expected but found %t.", array->type);

        if (flags & Expression_Check_Flags_lhs_value && !(array_type->pointer.privileges & Pointer_Type_Privilege_writable))
            print_span_error(file, array->span, "Missing writable privilege ($) from %t.", array->type);
        else if (flags & Expression_Check_Flags_rhs_value && !(array_type->pointer.privileges & Pointer_Type_Privilege_readable))
            print_span_error(file, array->span, "Missing readable privilege (?) from %t.", array->type);
    }
    else
        print_span_error(file, array->span, "Array type expected but found %t.", array->type);

    Expression* index = lookup_expression(access->index);
    type_check_expression(file, index, u32_handle, flags);
    
    Type* index_type = lookup_type(index->type);
    if (index_type->tag != Type_Tag_enumeration && index_type->tag > Type_Tag_u64)
        print_span_error(file, index->span, "Integer or enumeration type expected but found %t.", index->type);
}

void deep_copy_array_access(Array_Access* restrict destination, const Array_Access* restrict source)
{
    destination->array = add_expression();
    deep_copy_expression(lookup_expression(destination->array), lookup_expression(source->array));

    destination->index = add_expression();
    deep_copy_expression(lookup_expression(destination->index), lookup_expression(source->index));
}
