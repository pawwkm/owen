#include "call.h"

void resolve_call_to_function_overload_by_actual_type_parameters(const File* file, Interned_String_Handle function_name, Span function_name_span, Call* call)
{
    Expression* callee = lookup_expression(call->callee);
    bool function_with_the_same_name_exists = false;
    for (Array_Size a = 0; a < file->functions_in_scope.handles_length; a++)
    {
        Function* function = lookup_function(function_at(&file->functions_in_scope, a));
        if (!compare_interned_strings(function->name, function_name) || !(function->attributes & Function_Attribute_is_polymorphic))
            continue;

        function_with_the_same_name_exists = true;
        if (function->formal_parameters.handles_length != call->actual_parameters.handles_length || 
            function->formal_type_parameters.handles_length != callee->reference.actual_type_parameters.handles_length)
            continue;

        bool is_match = true;

        acquire_polymorphic_type_mapping();
        current_polymorphic_type_mapping->length = callee->reference.actual_type_parameters.handles_length;
        current_polymorphic_type_mapping->file = file;
        current_polymorphic_type_mapping->origin = callee->span;
        for (uint8_t b = 0; b < callee->reference.actual_type_parameters.handles_length; b++)
        {
            current_polymorphic_type_mapping->polymorphic_types[b] = type_reference_at(&function->formal_type_parameters, b);
            current_polymorphic_type_mapping->monomorphic_types[b] = lookup_type_by_reference(file, type_reference_at(&callee->reference.actual_type_parameters, b), true);
        }

        for (Array_Size b = 0; b < call->actual_parameters.handles_length; b++)
        {
            Type_Reference_Handle formal_parameter_type = lookup_formal_parameter(formal_parameter_at(&function->formal_parameters, b))->type;
            Type_Handle actual_parameter_type = lookup_expression(expression_at(&call->actual_parameters, b))->type;

            if (!expression_types_match(lookup_type_by_reference(file, formal_parameter_type, false), actual_parameter_type))
            {
                is_match = false;
                break;
            }
        }

        if (!is_match)
        {
            release_polymorphic_type_mapping();
            break;
        }

        for (uint16_t b = 0; b < functions_length; b++)
        {
            Function* monomorphic_function = &functions[b];
            if (!compare_functions(monomorphic_function->template, function_at(&file->functions_in_scope, a)))
                continue;

            if (signature_matches_actual_parameter_types(monomorphic_function, &call->actual_parameters))
            {
                resolve_type_of_call(call, monomorphic_function->signature);
                release_polymorphic_type_mapping();

                return;
            }
        }

        if (is_invalid_type_handle(call->type))
        {
            resolve_type_of_call(call, monomorphisize_function(function_at(&file->functions_in_scope, a)));
            release_polymorphic_type_mapping();
            
            return;
        }
    }

    if (function_with_the_same_name_exists)
    {
        print_declaration_header(file, function_name_span.start, "No matching signature for %I.", function_name);
        for (Array_Size i = 0; i < file->functions_in_scope.handles_length; i++)
        {
            Function* function = lookup_function(function_at(&file->functions_in_scope, i));
            if (compare_interned_strings(function->name, function_name))
                print_function_declaration(function);
        }

        exit_with_error_code();
    }
    else
        print_span_error(file, callee->span, symbol_of_reference(function_name) ? "%I is not a function." : "%I is undefined.", function_name);
}
