#include "call.h"

void resolve_type_of_call(Call* call, Type_Handle signature_handle)
{
    lookup_expression(call->callee)->type = signature_handle;

    Function_Type* signature = &lookup_type(signature_handle)->function;
    if (signature->return_types.handles_length > 1)
        call->type = lookup_tuple_type_by_signature(signature_handle);
    else if (signature->return_types.handles_length)
        call->type = type_at(&signature->return_types, 0);
    else
        call->type = none_handle;
}

bool signature_matches_actual_parameter_types(const Function* function, const Expression_Handle_Array* actual_parameters)
{
    assert(!(function->attributes & Function_Attribute_is_polymorphic) && "Polymorhic functions have no signature type.");
    if (function->formal_parameters.handles_length != actual_parameters->handles_length)
        return false;
    
    Function_Type* signature = &lookup_type(function->signature)->function;
    for (Array_Size i = 0; i < actual_parameters->handles_length; i++)
    {
        if (!expression_types_match(type_at(&signature->formal_parameters, i), lookup_expression(expression_at(actual_parameters, i))->type))
            return false;
    }
    
    return true;
}

static Address_Of* find_address_of(Expression_Handle expression_handle)
{
    Expression* expression = lookup_expression(expression_handle);
    switch (expression->tag)
    {
        case Expression_Tag_array_access:
            return find_address_of(expression->array_access.array);

        case Expression_Tag_field_access:
        case Expression_Tag_fixed_array_length:
        case Expression_Tag_dynamic_array_length:
        case Expression_Tag_dynamic_array_capacity:
        case Expression_Tag_dynamic_array_elements:
            return find_address_of(expression->field_access.compound);

        case Expression_Tag_address_of:
            return &expression->address_of;

        default:
            return NULL;
    }
}

void type_check_call_expression(const File* file, Call* call, Expression_Check_Flags flags)
{
    if (flags & Expression_Check_Flags_constant)
        print_span_error(file, call->span, "Calls are not constant.");

    for (Array_Size i = 0; i < call->actual_parameters.handles_length; i++)
        type_check_expression(file, lookup_expression(expression_at(&call->actual_parameters, i)), invalid_type_handle, flags & ~Expression_Check_Flags_retain);

    Expression* callee = lookup_expression(call->callee);
    if (callee->tag == Expression_Tag_reference)
    {
        if (callee->reference.actual_type_parameters.handles_length)
            resolve_call_to_function_overload_by_actual_type_parameters(file, callee->reference.name, callee->reference.span, call);
        else if (symbol_of_reference(callee->reference.name))
            call_to_function_typed_expression(file, call, flags);
        else
            resolve_call_to_function_overload_by_actual_parameters(file, callee->reference.name, callee->reference.span, call);
    }
    else
        call_to_function_typed_expression(file, call, flags);

    Function_Type* signature = &lookup_type(callee->type)->function;
    for (Array_Size i = 0; i < call->actual_parameters.handles_length; i++)
    {
        Type* formal_parameter_type = lookup_type(type_at(&signature->formal_parameters, i));
        if (formal_parameter_type->tag != Type_Tag_pointer || !(formal_parameter_type->pointer.privileges & Pointer_Type_Privilege_retained))
            continue;

        Address_Of* address_of = find_address_of(expression_at(&call->actual_parameters, i)); 
        if (address_of)
            type_check_address_of(file, address_of, flags & Expression_Check_Flags_retain);
    }
}

void deep_copy_call(Call* restrict destination, const Call* restrict source)
{
    destination->callee = add_expression();
    deep_copy_expression(lookup_expression(destination->callee), lookup_expression(source->callee));

    deep_copy_expressions(&destination->actual_parameters, &source->actual_parameters); 
}
