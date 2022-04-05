#include "expression.h"

void type_check_reference_expression(const File* file, Reference* reference, Type_Handle inferred_type_handle)
{
    if (reference->actual_type_parameters.handles_length)
        not_implemented(__FILE__, __LINE__, "Expression_Tag_reference");
    else
    {
        Symbol* symbol = symbol_of_reference(reference->name);
        if (symbol)
            reference->type = symbol->type;
        else if (is_invalid_type_handle(inferred_type_handle))
        { 
            bool function_with_the_same_name_exists = false;
            for (uint16_t i = 0; i < file->functions_in_scope.handles_length; i++)
            {
                Function* function = lookup_function(file->functions_in_scope.handles[i]);
                if (compare_interned_strings(function->name, reference->name))
                {
                    if (!function_with_the_same_name_exists)
                        print_declaration_header(file, reference->span.start, "No matching signature for %I.", reference->name);

                    print_function_declaration(function);
                    function_with_the_same_name_exists = true;
                }
            }

            if (function_with_the_same_name_exists)
                exit_with_error_code();
            else
                print_span_error(file, reference->span, "%I is undefined.", reference->name);
        }
        else
        {
            reference->type = inferred_type_handle;

            Type* inferred_type = lookup_type(inferred_type_handle);
            bool function_with_the_same_name_exists = false;
            for (uint16_t a = 0; a < file->functions_in_scope.handles_length; a++)
            {
                Function* function = lookup_function(file->functions_in_scope.handles[a]);
                if (!compare_interned_strings(reference->name, function->name) || function->attributes & Function_Attribute_is_polymorphic)
                    continue;

                function_with_the_same_name_exists = true;
                if (function->formal_parameters.handles_length != inferred_type->function.formal_parameters.handles_length ||
                    function->return_types.handles_length != inferred_type->function.return_types.handles_length)
                    continue;

                if (expression_types_match(function->signature, reference->type))
                    return;
            }

            for (uint16_t a = 0; a < file->functions_in_scope.handles_length; a++)
            {
                Function_Handle a_handle = file->functions_in_scope.handles[a];
                Function* function = lookup_function(a_handle);
                if (!compare_interned_strings(reference->name, function->name) || !(function->attributes & Function_Attribute_is_polymorphic))
                    continue;

                function_with_the_same_name_exists = true;
                if (function->formal_parameters.handles_length != inferred_type->function.formal_parameters.handles_length ||
                    function->return_types.handles_length != inferred_type->function.return_types.handles_length)
                    continue;

                bool is_match = true;
                for (uint8_t b = 0; b < function->formal_parameters.handles_length; b++)
                {
                    Type_Reference_Handle formal_type_parameter = lookup_formal_parameter(function->formal_parameters.handles[b])->type;
                    Type_Handle actual_type = inferred_type->function.formal_parameters.handles[b];

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
                for (uint8_t b = 0; b < function->formal_type_parameters.handles_length; b++)
                {
                    Named_Type_Reference* formal_type_parameter = &lookup_type_reference(function->formal_type_parameters.handles[b])->named;
                    bool specified_by_formal_parameter = false;

                    for (uint8_t c = 0; c < function->formal_parameters.handles_length; c++)
                    {
                        Formal_Parameter* formal_parameter = lookup_formal_parameter(function->formal_parameters.handles[c]);
                        if (is_formal_type_parameter_specified_by_a_formal_parameter(formal_parameter->type, formal_type_parameter->name))
                        {
                            specified_by_formal_parameter = true;
                            break;
                        }
                    }

                    if (!specified_by_formal_parameter)
                        print_span_error(file, reference->span, "%I could not be inferred from the call site. Specify it explicitly.", formal_type_parameter->name);
                }

                // Lookup previously monophosized functions. 
                for (uint8_t b = 0; b < functions_length; b++)
                {
                    Function* monomorphic_function = &functions[b];
                    if (!compare_functions(monomorphic_function->template, (Function_Handle) { .index = b }))
                        continue;

                    if (expression_types_match(monomorphic_function->signature, reference->type))
                        return;
                }

                acquire_polymorphic_type_mapping();
                current_polymorphic_type_mapping->length = function->formal_type_parameters.handles_length;
                current_polymorphic_type_mapping->file = file;
                current_polymorphic_type_mapping->origin = reference->span;

                for (uint8_t b = 0; b < function->formal_parameters.handles_length; b++)
                {
                    Type_Reference* formal_parameter_type = lookup_type_reference(lookup_formal_parameter(function->formal_parameters.handles[b])->type);
                    for (uint8_t c = 0; c < inferred_type->function.formal_parameters.handles_length; c++)
                        map_formal_type_parameter(file, function, inferred_type->function.formal_parameters.handles[c], formal_parameter_type, reference->span);
                }

                monomorphisize_function(a_handle);
                release_polymorphic_type_mapping();

                return;
            }

            if (function_with_the_same_name_exists)
            {
                print_declaration_header(file, reference->span.start, "No matching signature for %I.", reference->name);
                for (uint16_t i = 0; i < file->functions_in_scope.handles_length; i++)
                {
                    Function* function = lookup_function(file->functions_in_scope.handles[i]);
                    if (compare_interned_strings(function->name, reference->name))
                        print_function_declaration(function);
                }

                exit_with_error_code();
            }
            else
                print_span_error(file, reference->span, "%I is undefined.", reference->name);
        } 
    }
}

void deep_copy_reference(Reference* restrict destination, const Reference* restrict source)
{
    destination->name = source->name;

    reserve_type_reference_handles(&destination->actual_type_parameters, source->actual_type_parameters.handles_length);
    deep_copy_type_references(&destination->actual_type_parameters, &source->actual_type_parameters);
}
