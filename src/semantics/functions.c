#include "semantics.h"

static void check_for_non_public_types_in_public_function_signature(const Function* function)
{
    if ((function->attributes & Function_Attribute_is_public) == 0)
        return;

    Function_Type* function_type = &lookup_type(function->signature)->function;    
    for (uint8_t i = 0; i < function->formal_parameters.handles_length; i++)
    {
        Type_Handle type = function_type->formal_parameters.handles[i];
        if (!is_public_type(type))
            print_span_error(lookup_file(function->file), function->name_span, "%I exposes %t which is not public.", function->name, type);
    }

    for (uint8_t i = 0; i < function->return_types.handles_length; i++)
    {
        Type_Handle type = function_type->return_types.handles[i];
        if (!is_public_type(type))
            print_span_error(lookup_file(function->file), function->name_span, "%I exposes %t which is not public.", function->name, type);
    }
}

static void check_if_signature_has_duplicate_formal_type_parameters(const Function* function)
{
    for (uint8_t a = 0; a < function->formal_type_parameters.handles_length; a++)
    {
        Named_Type_Reference* a_reference = &lookup_type_reference(function->formal_type_parameters.handles[a])->named;
        for (uint8_t b = a + 1; b < function->formal_type_parameters.handles_length; b++)
        {
            Named_Type_Reference* b_reference = &lookup_type_reference(function->formal_type_parameters.handles[b])->named;
            if (compare_interned_strings(a_reference->name, b_reference->name))
                print_span_error(lookup_file(function->file), b_reference->span, "Duplicate formal type parameter %I.", b_reference->name);
        }
    }
}

static void check_if_signatures_formal_type_parameters_matches_any_types(const File* file, Function* function)
{
    for (uint8_t i = 0; i < function->formal_type_parameters.handles_length; i++)
    {
        Type_Reference_Handle type_reference_handle = function->formal_type_parameters.handles[i];
        Type_Handle type_handle = lookup_type_by_reference(file, type_reference_handle, false);

        if (is_invalid_type_handle(type_handle))
            continue;

        Named_Type_Reference* formal_type_parameter = &lookup_type_reference(type_reference_handle)->named;
        print_declaration_header(file, formal_type_parameter->span.start, "Formal type parameter matches declaration for %I in scope.", formal_type_parameter->name);
        print_function_declaration(function);
        print_type_declaration(type_handle);

        exit_with_error_code();
    }
}

static void check_for_nested_formal_type_parameters_in_reference(const File* file, const Function* function, Type_Reference_Handle reference_handle)
{
    Type_Reference* reference = lookup_type_reference(reference_handle);
    if (reference->tag == Type_Reference_Tag_pointer)
        check_for_nested_formal_type_parameters_in_reference(file, function, reference->pointer.base_type);
    else if (reference->tag == Type_Reference_Tag_dynamic_array)
        check_for_nested_formal_type_parameters_in_reference(file, function, reference->dynamic_array.base_type);
    else if (reference->tag == Type_Reference_Tag_fixed_array)
        check_for_nested_formal_type_parameters_in_reference(file, function, reference->fixed_array.base_type);
    else if (reference->tag == Type_Reference_Tag_function)
    {
        for (uint8_t i = 0; i < reference->function.formal_parameters.handles_length; i++)
            check_for_nested_formal_type_parameters_in_reference(file, function, reference->function.formal_parameters.handles[i]);

        for (uint8_t i = 0; i < reference->function.return_types.handles_length; i++)
            check_for_nested_formal_type_parameters_in_reference(file, function, reference->function.return_types.handles[i]);
    }
    else if (reference->tag == Type_Reference_Tag_polymorphic_compound)
    {
        for (uint8_t i = 0; i < function->formal_type_parameters.handles_length; i++)
        {
            Named_Type_Reference* formal_type_parameter = &lookup_type_reference(function->formal_type_parameters.handles[i])->named;
            if (compare_interned_strings(reference->named.name, formal_type_parameter->name))
                print_span_error(file, reference->named.span, "Type parameters can't be nested.");
        }
        
        for (uint8_t i = 0; i < reference->polymorphic_compound.actual_type_parameters.handles_length; i++)
            check_for_nested_formal_type_parameters_in_reference(file, function, reference->polymorphic_compound.actual_type_parameters.handles[i]);
    }
}

static void check_for_nested_formal_type_parameters_in_signature(const File* file, const Function* function)
{
    for (uint8_t i = 0; i < function->formal_parameters.handles_length; i++)
        check_for_nested_formal_type_parameters_in_reference(file, function, lookup_formal_parameter(function->formal_parameters.handles[i])->type);

    for (uint8_t i = 0; i < function->return_types.handles_length; i++)
        check_for_nested_formal_type_parameters_in_reference(file, function, function->return_types.handles[i]);
}

void type_check_signature(const File* file, Function* function)
{
    if (function->attributes & Function_Attribute_is_polymorphic)
    {
        check_if_signature_has_duplicate_formal_type_parameters(function);
        check_if_signatures_formal_type_parameters_matches_any_types(file, function);
        check_for_nested_formal_type_parameters_in_signature(file, function);

        function->signature = invalid_type_handle;
    }
    else
    {
        function->signature = lookup_signature(file, function);
        check_for_non_public_types_in_public_function_signature(function);
    }
}

static void put_formal_parameters_into_scope(const File* file, const Function* function)
{
    Function_Type* function_type = &lookup_type(function->signature)->function;
    for (uint8_t i = 0; i < function->formal_parameters.handles_length; i++)
    {
        Formal_Parameter* formal_parameter = lookup_formal_parameter(function->formal_parameters.handles[i]);
        check_that_identifier_is_undefined(file, formal_parameter->name, formal_parameter->name_span);

        add_symbol(formal_parameter->name, formal_parameter->name_span, function_type->formal_parameters.handles[i]);
    }
}

void type_check_function_body(const File* file, Function* function)
{   
    acquire_symbol_table();
    put_formal_parameters_into_scope(file, function);
    
    Function_Type* signature = &lookup_type(function->signature)->function;
    if (!type_check_statements(file, &function->body, function, false) && signature->return_types.handles_length)
        print_span_error(file, function->name_span, "Missing terminating statement.");

    check_for_unreferenced_variables(file, 0);
    release_symbol_table();
}

static bool possibly_polymorphic_type_references_match(const Function* a_function, const Type_Reference* a_reference, const Function* b_function, const Type_Reference* b_reference)
{
    if (a_reference->tag != b_reference->tag)
        return false;
    else if (a_reference->tag == Type_Reference_Tag_name)
    {
        // No matter the order and names of formal type parameters they match any other formal type parameter.
        for (uint8_t a = 0; a < a_function->formal_type_parameters.handles_length; a++)
        {
            if (!compare_interned_strings(lookup_type_reference(a_function->formal_type_parameters.handles[a])->named.name, a_reference->named.name))
                continue;

            for (uint8_t b = 0; b < b_function->formal_type_parameters.handles_length; b++)
            {
                if (compare_interned_strings(lookup_type_reference(b_function->formal_type_parameters.handles[b])->named.name, b_reference->named.name))
                    return true;
            }
        }

        return compare_interned_strings(a_reference->named.name, b_reference->named.name);
    }
    else if (a_reference->tag == Type_Reference_Tag_pointer)
        not_implemented(__FILE__, __LINE__, "Type_Reference_Tag_pointer");
    else if (a_reference->tag == Type_Reference_Tag_dynamic_array)
        not_implemented(__FILE__, __LINE__, "Type_Reference_Tag_dynamic_array");
    else if (a_reference->tag == Type_Reference_Tag_fixed_array)
        return compare_strings(a_reference->fixed_array.size, b_reference->fixed_array.size) &&
               possibly_polymorphic_type_references_match(a_function, lookup_type_reference(a_reference->fixed_array.base_type), b_function, lookup_type_reference(b_reference->fixed_array.base_type));
    else if (a_reference->tag == Type_Reference_Tag_function)
        not_implemented(__FILE__, __LINE__, "Type_Reference_Tag_function");
    else if (a_reference->tag == Type_Reference_Tag_polymorphic_compound)
    {
        if (!compare_interned_strings(a_reference->named.name, b_reference->named.name) ||
            a_reference->polymorphic_compound.actual_type_parameters.handles_length != b_reference->polymorphic_compound.actual_type_parameters.handles_length)
            return false;

        for (uint8_t i = 0; i < a_reference->polymorphic_compound.actual_type_parameters.handles_length; i++)
        {
            if (!possibly_polymorphic_type_references_match(a_function, lookup_type_reference(a_reference->polymorphic_compound.actual_type_parameters.handles[i]), b_function, lookup_type_reference(b_reference->polymorphic_compound.actual_type_parameters.handles[i])))
                return false;
        }

        return true;
    }
    else
        unexpected_reference_type(__FILE__, __LINE__, a_reference->tag);
    
    return false;
}

static bool possibly_polymorphic_compound_reference_matches_type(const Function* function, Type_Reference_Handle reference_handle, Type_Handle type_handle)
{
    Type_Reference* reference = lookup_type_reference(reference_handle);
    Type* type = lookup_type(type_handle);

    if (reference->tag == Type_Reference_Tag_name)
    {
        for (uint8_t i = 0; i < function->formal_type_parameters.handles_length; i++)
        {
            if (compare_interned_strings(lookup_type_reference(function->formal_type_parameters.handles[i])->named.name, reference->named.name))
                return false;
        }

        if (type->tag <= Type_Tag_bool)
            return compare_interned_strings(reference->named.name, type->primitive.name);
        else if (type->tag == Type_Tag_enumeration)
            return compare_interned_strings(reference->named.name, type->enumeration.name);
        else if (type->tag == Type_Tag_compound)
            return compare_interned_strings(reference->named.name, type->compound.name);
    }
    else if (reference->tag == Type_Reference_Tag_pointer)
        not_implemented(__FILE__, __LINE__, "Type_Reference_Tag_pointer");
    else if (reference->tag == Type_Reference_Tag_dynamic_array)
        return type->tag == Type_Tag_dynamic_array &&
               possibly_polymorphic_compound_reference_matches_type(function, reference->dynamic_array.base_type, type->dynamic_array.base_type);
    else if (reference->tag == Type_Reference_Tag_fixed_array)
    {
        return type->tag == Type_Tag_fixed_array &&
               type->fixed_array.size == type_check_integer_literal(lookup_file(function->file), reference->fixed_array.size, reference->fixed_array.size_span, u32_handle).u32 &&
               possibly_polymorphic_compound_reference_matches_type(function, reference->fixed_array.base_type, type->fixed_array.base_type);
    }
    else if (reference->tag == Type_Reference_Tag_function)
        not_implemented(__FILE__, __LINE__, "Type_Reference_Tag_function");
    else if (reference->tag == Type_Reference_Tag_polymorphic_compound)
    {
        if (type->tag != Type_Tag_compound ||
            !compare_interned_strings(type->compound.name, reference->named.name) ||
            type->compound.actual_type_parameters.handles_length != reference->polymorphic_compound.actual_type_parameters.handles_length)
            return false;

        for (uint8_t i = 0; i < reference->polymorphic_compound.actual_type_parameters.handles_length; i++)
        {
            if (!possibly_polymorphic_compound_reference_matches_type(function, reference->polymorphic_compound.actual_type_parameters.handles[i], type->compound.actual_type_parameters.handles[i]))
                return false;
        }

        return true;
    }
    else
        unexpected_reference_type(__FILE__, __LINE__, reference->tag);

    return false;
}

static bool formal_parameter_types_in_signatures_matches(Type_Handle a_handle, Type_Handle b_handle)
{
    Type* a = lookup_type(a_handle);
    Type* b = lookup_type(b_handle);

    if (a->tag != b->tag)
        return false;
    else if (a->tag <= Type_Tag_bool || a->tag == Type_Tag_none)
        return true;
    else if (a->tag == Type_Tag_compound || a->tag == Type_Tag_enumeration)
        return a == b;
    else if (a->tag == Type_Tag_function)
    {
        if (a->function.formal_parameters.handles_length != b->function.formal_parameters.handles_length)
            return false;

        for (uint8_t i = 0; i < a->function.formal_parameters.handles_length; i++)
        {
            if (!formal_parameter_types_in_signatures_matches(a->function.formal_parameters.handles[i], b->function.formal_parameters.handles[i]))
                return false;
        }

        return true;
    }
    else if (a->tag == Type_Tag_dynamic_array)
        formal_parameter_types_in_signatures_matches(a->pointer.base_type, b->pointer.base_type);
    else if (a->tag == Type_Tag_fixed_array)
        return a->fixed_array.size == b->fixed_array.size && 
               formal_parameter_types_in_signatures_matches(a->fixed_array.base_type, b->fixed_array.base_type);
    else if (a->tag == Type_Tag_pointer)
        return (~a->pointer.privileges & b->pointer.privileges) == 0 && 
               formal_parameter_types_in_signatures_matches(a->pointer.base_type, b->pointer.base_type);
    else
        unexpected_type(__FILE__, __LINE__, a->tag);

    return false;
}

bool signatures_match(Function_Handle a_handle, Function_Handle b_handle)
{
    Function* a_function = lookup_function(a_handle);
    Function* b_function = lookup_function(b_handle);
    
    if ((a_function->attributes & Function_Attribute_is_polymorphic) && (b_function->attributes & Function_Attribute_is_polymorphic))
    {
        if (a_function->formal_parameters.handles_length != b_function->formal_parameters.handles_length)
            return false;

        for (uint8_t i = 0; i < a_function->formal_parameters.handles_length; i++)
        {
            Formal_Parameter* a_formal_parameter = lookup_formal_parameter(a_function->formal_parameters.handles[i]);
            Formal_Parameter* b_formal_parameter = lookup_formal_parameter(b_function->formal_parameters.handles[i]);
            
            if (!possibly_polymorphic_type_references_match(a_function, lookup_type_reference(a_formal_parameter->type), b_function, lookup_type_reference(b_formal_parameter->type)))
                return false;
        }

        return true;
    }
    else if ((a_function->attributes & Function_Attribute_is_polymorphic) && !(b_function->attributes & Function_Attribute_is_polymorphic))
        return signatures_match(b_handle, a_handle);
    else if (!(a_function->attributes & Function_Attribute_is_polymorphic) && (b_function->attributes & Function_Attribute_is_polymorphic))
    {
        if (a_function->formal_parameters.handles_length != b_function->formal_parameters.handles_length)
            return false;

        for (uint8_t i = 0; i < a_function->formal_parameters.handles_length; i++)
        {
            Formal_Parameter* b_formal_parameter = lookup_formal_parameter(b_function->formal_parameters.handles[i]);
            Function_Type* a_function_type = &lookup_type(a_function->signature)->function;
            
            if (!possibly_polymorphic_compound_reference_matches_type(b_function, b_formal_parameter->type, a_function_type->formal_parameters.handles[i]))
                return false;
        }

        return true;
    }
    else if (!a_function->formal_parameters.handles_length && !b_function->formal_parameters.handles_length)
        return true;
    else
        return formal_parameter_types_in_signatures_matches(a_function->signature, b_function->signature);
}

Type_Handle monomorphisize_function(Function_Handle polymorphic_handle)
{
    Function* polymorphic = lookup_function(polymorphic_handle);
    File* file = lookup_file(polymorphic->file); 
    
    Function_Handle monomorphic_handle = add_function();
    Function* monomorphic = lookup_function(monomorphic_handle);
    monomorphic->file = polymorphic->file;
    monomorphic->attributes = polymorphic->attributes & ~Function_Attribute_is_polymorphic;
    monomorphic->name = polymorphic->name;
    monomorphic->template = polymorphic_handle;
    monomorphic->ir = invalid_ir_function_handle;

    reserve_formal_parameter_handles(&monomorphic->formal_parameters, polymorphic->formal_parameters.handles_length);
    for (uint8_t i = 0; i < polymorphic->formal_parameters.handles_length; i++)
    {
        add_to_formal_parameter_array(&monomorphic->formal_parameters, add_formal_parameter());

        Formal_Parameter* monomorphic_formal_parameter = lookup_formal_parameter(monomorphic->formal_parameters.handles[i]);
        Formal_Parameter* polymorphic_formal_parameter = lookup_formal_parameter(polymorphic->formal_parameters.handles[i]);

        monomorphic_formal_parameter->name = polymorphic_formal_parameter->name;
        monomorphic_formal_parameter->type = add_type_reference();

        deep_copy_type_reference(monomorphic_formal_parameter->type, polymorphic_formal_parameter->type);
    }
    
    current_polymorphic_type_mapping->monomorphisized_function = monomorphic;
    deep_copy_type_references(&monomorphic->return_types, &polymorphic->return_types);
    for (uint8_t i = 0; i < current_polymorphic_type_mapping->length; i++)
        add_to_type_array(&monomorphic->actual_type_parameters, current_polymorphic_type_mapping->monomorphic_types[i]);

    deep_copy_statements(&monomorphic->body, &polymorphic->body);
    type_check_signature(file, monomorphic);
    type_check_function_body(file, monomorphic);

    return monomorphic->signature;
}
