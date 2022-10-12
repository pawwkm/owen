#include "call.h"

bool is_formal_type_parameter_specified_by_a_formal_parameter(Type_Reference_Handle formal_parameter_handle, Interned_String_Handle t)
{
    Type_Reference* formal_parameter = lookup_type_reference(formal_parameter_handle);
    if (formal_parameter->tag == Type_Reference_Tag_name)
        return compare_interned_strings(formal_parameter->named.name, t);
    else if (formal_parameter->tag == Type_Reference_Tag_pointer)
        return is_formal_type_parameter_specified_by_a_formal_parameter(formal_parameter->pointer.base_type, t);
    else if (formal_parameter->tag == Type_Reference_Tag_fixed_array)
        return is_formal_type_parameter_specified_by_a_formal_parameter(formal_parameter->fixed_array.base_type, t);
    else if (formal_parameter->tag == Type_Reference_Tag_dynamic_array)
        return is_formal_type_parameter_specified_by_a_formal_parameter(formal_parameter->dynamic_array.base_type, t);
    else if (formal_parameter->tag == Type_Reference_Tag_function)
        not_implemented(__FILE__, __LINE__, "Type_Reference_Tag_function");
    else if (formal_parameter->tag == Type_Reference_Tag_polymorphic_compound)
    {
        if (compare_interned_strings(formal_parameter->named.name, t))
            return true;

        for (Array_Size i = 0; i < formal_parameter->polymorphic_compound.actual_type_parameters.handles_length; i++)
        {
            Type_Reference_Handle handle = type_reference_at(&formal_parameter->polymorphic_compound.actual_type_parameters, i);
            if (is_formal_type_parameter_specified_by_a_formal_parameter(handle, t))
                return true;
        }
    }

    return false;
}

void map_formal_type_parameter(const File* file, const Function* function, Type_Handle actual_parameter_type, const Type_Reference* formal_type_parameter, Span span_of_type_definition)
{
    if (formal_type_parameter->tag == Type_Reference_Tag_name)
    {
        for (Array_Size i = 0; i < function->formal_type_parameters.handles_length; i++)
        {
            if (compare_interned_strings(lookup_type_reference(type_reference_at(&function->formal_type_parameters, i))->named.name, formal_type_parameter->named.name))
            {
                if (is_invalid_type_handle(current_polymorphic_type_mapping->monomorphic_types[i]))
                {
                    current_polymorphic_type_mapping->polymorphic_types[i] = type_reference_at(&function->formal_type_parameters, i);
                    current_polymorphic_type_mapping->monomorphic_types[i] = actual_parameter_type;
                }
                else if (!expression_types_match(current_polymorphic_type_mapping->monomorphic_types[i], actual_parameter_type))
                {
                    // current_polymorphic_type_mapping->monomorphisized_function is not yet set and will 
                    // cause an error if the polymorphic stack is printed.
                    Named_Type_Reference* polymorphic_type = &lookup_type_reference(current_polymorphic_type_mapping->polymorphic_types[i])->named;
                    Type_Handle original_type = current_polymorphic_type_mapping->monomorphic_types[i];

                    release_polymorphic_type_mapping();

                    print_span_error(file, span_of_type_definition, "%I redefined as %t. It was originally %t.",
                                                                    polymorphic_type->name,
                                                                    actual_parameter_type,
                                                                    original_type);
                }

                break;
            }
        }
    }
    else if (formal_type_parameter->tag == Type_Reference_Tag_pointer)
    {
        Type_Handle actual_base_type = lookup_type(actual_parameter_type)->pointer.base_type;
        Type_Reference* formal_base_type = lookup_type_reference(formal_type_parameter->pointer.base_type);
        map_formal_type_parameter(file, function, actual_base_type, formal_base_type, span_of_type_definition);
    }
    else if (formal_type_parameter->tag == Type_Reference_Tag_fixed_array)
    {
        Type_Handle actual_base_type = lookup_type(actual_parameter_type)->fixed_array.base_type;
        Type_Reference* formal_base_type = lookup_type_reference(formal_type_parameter->fixed_array.base_type);
        map_formal_type_parameter(file, function, actual_base_type, formal_base_type, span_of_type_definition);
    }
    else if (formal_type_parameter->tag == Type_Reference_Tag_dynamic_array)
    {
        Type_Handle actual_base_type = lookup_type(actual_parameter_type)->dynamic_array.base_type;
        Type_Reference* formal_base_type = lookup_type_reference(formal_type_parameter->dynamic_array.base_type);
        map_formal_type_parameter(file, function, actual_base_type, formal_base_type, span_of_type_definition);
    }
    else if (formal_type_parameter->tag == Type_Reference_Tag_function)
        not_implemented(__FILE__, __LINE__, "Type_Reference_Tag_function");
    else if (formal_type_parameter->tag == Type_Reference_Tag_polymorphic_compound)
    {
        for (Array_Size i = 0; i < formal_type_parameter->polymorphic_compound.actual_type_parameters.handles_length; i++)
        {
            Type_Handle actual = type_at(&lookup_type(actual_parameter_type)->compound.actual_type_parameters, i);
            Type_Reference* formal = lookup_type_reference(type_reference_at(&formal_type_parameter->polymorphic_compound.actual_type_parameters, i));
            map_formal_type_parameter(file, function, actual, formal, span_of_type_definition);
        }
    }
}

bool formal_parameter_type_matches_actual_parameter_type(const File* file, const Function* function, Type_Reference_Handle formal_parameter_type_handle, Type_Handle actual_type_handle)
{
    Type_Reference* formal_parameter_type = lookup_type_reference(formal_parameter_type_handle);
    Type* actual_type = lookup_type(actual_type_handle);

    if (formal_parameter_type->tag == Type_Reference_Tag_name)
    {
        for (Array_Size i = 0; i < function->formal_type_parameters.handles_length; i++)
        {
            if (compare_interned_strings(formal_parameter_type->named.name, lookup_type_reference(type_reference_at(&function->formal_type_parameters, i))->named.name))
                return true;
        }

        return expression_types_match(lookup_type_by_reference(file, formal_parameter_type_handle, true), actual_type_handle);
    }
    else if (formal_parameter_type->tag == Type_Reference_Tag_pointer)
        return (~formal_parameter_type->pointer.privileges & actual_type->pointer.privileges) == 0 &&
               formal_parameter_type_matches_actual_parameter_type(file, function, formal_parameter_type->pointer.base_type, actual_type->pointer.base_type);
    else if (formal_parameter_type->tag == Type_Reference_Tag_dynamic_array)
        return formal_parameter_type_matches_actual_parameter_type(file, function, formal_parameter_type->dynamic_array.base_type, actual_type->dynamic_array.base_type);
    else if (formal_parameter_type->tag == Type_Reference_Tag_fixed_array)
        return formal_parameter_type_matches_actual_parameter_type(file, function, formal_parameter_type->fixed_array.base_type, actual_type->fixed_array.base_type);
    else if (formal_parameter_type->tag == Type_Reference_Tag_function)
        not_implemented(__FILE__, __LINE__, "Type_Reference_Tag_function");
    else if (formal_parameter_type->tag == Type_Reference_Tag_polymorphic_compound)
    {
        if (actual_type->tag != Type_Tag_compound ||
            is_invalid_type_handle(actual_type->compound.template) ||
            actual_type->compound.actual_type_parameters.handles_length != formal_parameter_type->polymorphic_compound.actual_type_parameters.handles_length)
            return false;

        for (Array_Size i = 0; i < actual_type->compound.actual_type_parameters.handles_length; i++)
        {
            if (!formal_parameter_type_matches_actual_parameter_type(file, function, type_reference_at(&formal_parameter_type->polymorphic_compound.actual_type_parameters, i), type_at(&actual_type->compound.actual_type_parameters, i)))
                return false;
        }

        return true;
    }

    return false;
}

void resolve_call_to_function_overload_by_actual_parameters(const File* file, Interned_String_Handle function_name, Span function_name_span, Call* call)
{
    bool function_with_the_same_name_exists = false;
    for (Array_Size a = 0; a < file->functions_in_scope.handles_length; a++)
    {
        Function* function = lookup_function(function_at(&file->functions_in_scope, a));
        if (!compare_interned_strings(function->name, function_name) || function->attributes & Function_Attribute_is_polymorphic)
            continue;

        function_with_the_same_name_exists = true;
        if (function->formal_parameters.handles_length != call->actual_parameters.handles_length)
            continue;

        if (signature_matches_actual_parameter_types(function, &call->actual_parameters))
        {
            resolve_type_of_call(call, function->signature);
            return;
        }
    }

    for (Array_Size a = 0; a < file->functions_in_scope.handles_length; a++)
    {
        Function* function = lookup_function(function_at(&file->functions_in_scope, a));
        if (!compare_interned_strings(function->name, function_name) || !(function->attributes & Function_Attribute_is_polymorphic))
            continue;

        // Since polymorphic functions do not have a signature Type_Handle we must verify 
        // whether or not its signature matches the callee using Type_References instead.
        function_with_the_same_name_exists = true;
        if (function->formal_parameters.handles_length != call->actual_parameters.handles_length)
            continue;

        bool is_match = true;
        for (Array_Size b = 0; b < call->actual_parameters.handles_length; b++)
        {
            Type_Reference_Handle formal_type_parameter = lookup_formal_parameter(formal_parameter_at(&function->formal_parameters, b))->type;
            Type_Handle actual_type = lookup_expression(expression_at(&call->actual_parameters, b))->type;

            if (!formal_parameter_type_matches_actual_parameter_type(file, function, formal_type_parameter, actual_type))
            {
                is_match = false;
                break;
            }
        }

        if (!is_match)
            continue;

        // Check that all formal type parameters are specified by actual parameters.
        // TODO: This is done every time the overload is resolved and the answer is 
        // always the same... This should be a flag.
        for (Array_Size b = 0; b < function->formal_type_parameters.handles_length; b++)
        {
            Named_Type_Reference* formal_type_parameter = &lookup_type_reference(type_reference_at(&function->formal_type_parameters, b))->named;
            bool specified_by_formal_parameter = false;

            for (Array_Size c = 0; c < function->formal_parameters.handles_length; c++)
            {
                Formal_Parameter* formal_parameter = lookup_formal_parameter(formal_parameter_at(&function->formal_parameters, c));
                if (is_formal_type_parameter_specified_by_a_formal_parameter(formal_parameter->type, formal_type_parameter->name))
                {
                    specified_by_formal_parameter = true;
                    break;
                }
            }

            if (!specified_by_formal_parameter)
                print_span_error(file, call->span, "%I could not be inferred from the call site. Specify it explicitly.", formal_type_parameter->name);
        }

        // Lookup previously monophosized functions. 
        for (uint16_t b = 0; b < functions_length; b++)
        {
            Function* monomorphic_function = &functions[b];
            if (!compare_functions(monomorphic_function->template, (Function_Handle) { .index = b }))
                continue;

            if (signature_matches_actual_parameter_types(monomorphic_function, &call->actual_parameters))
            {
                resolve_type_of_call(call, monomorphic_function->signature);
                return;
            }
        }

        acquire_polymorphic_type_mapping();
        current_polymorphic_type_mapping->length = function->formal_type_parameters.handles_length;
        current_polymorphic_type_mapping->file = file;
        current_polymorphic_type_mapping->origin = call->span;

        for (Array_Size b = 0; b < function->formal_parameters.handles_length; b++)
        {
            Type_Reference* formal_parameter_type = lookup_type_reference(lookup_formal_parameter(formal_parameter_at(&function->formal_parameters, b))->type);
            for (Array_Size c = 0; c < call->actual_parameters.handles_length; c++)
            {
                Expression* actual_parameter = lookup_expression(expression_at(&call->actual_parameters, c));
                map_formal_type_parameter(file, function, actual_parameter->type, formal_parameter_type, actual_parameter->span);
            }
        }

        resolve_type_of_call(call, monomorphisize_function(function_at(&file->functions_in_scope, a)));
        release_polymorphic_type_mapping();

        return;
    }

    if (function_with_the_same_name_exists)
    {
        print_declaration_header(file, function_name_span.start, "No matching signature for %I.", function_name);
        for (Array_Size a = 0; a < file->functions_in_scope.handles_length; a++)
        {
            Function* function = lookup_function(function_at(&file->functions_in_scope, a));
            if (compare_interned_strings(function->name, function_name))
                print_function_declaration(function);
        }

        exit_with_error_code();
    }
    else
        print_span_error(file, function_name_span, "%I is undefined.", function_name);
}
