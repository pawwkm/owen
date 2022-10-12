#include "semantics.h"
#include <assert.h>

Field* lookup_field_by_name(const Compound_Type* compound, Interned_String_Handle name, Span name_span)
{
    for (Array_Size i = 0; i < compound->fields.handles_length; i++)
    {
        Field* field = lookup_field(field_at(&compound->fields, i));
        if (compare_interned_strings(field->name, name))
            return field;
    }

    print_span_error(lookup_file(compound->file), name_span, "%c does not declare field %I.", compound, name);
    return NULL;
}

static bool check_compound_for_recursive_fields_continued(const Compound_Type* compound, const Field* first_field)
{
    for (Array_Size i = 0; i < compound->fields.handles_length; i++)
    {
        Field* field = lookup_field(field_at(&compound->fields, i));
        if (is_invalid_type_handle(field->type))
        {
            // structure A<T>
            //     B<T> b
            // end
            //
            // structure B<T>
            //     A<T> a
            // end
            //
            // structure C
            //     A<I32> a
            // end

            // Polymorphic compounds that are indirectly recursive exclusivly 
            // through polymorphic compounds causes a chain of monomorphization 
            // and type lookups where the last field->type in the chain 
            // is null (`B<I32> b` in this case).

            return true;
        }

        Type* type = lookup_type(field->type);
        if (type->tag == Type_Tag_compound)
        {
            if (field == first_field || check_compound_for_recursive_fields_continued(&type->compound, first_field))
                return true;
        }
        else if (type->tag == Type_Tag_fixed_array)
        {
            Type* base_type = lookup_type(type->fixed_array.base_type);
            if (field == first_field || base_type->tag == Type_Tag_compound && check_compound_for_recursive_fields_continued(&base_type->compound, first_field))
                return true;
        }
        else if (type->tag == Type_Tag_dynamic_array)
        {
            Type* base_type = lookup_type(type->dynamic_array.base_type);
            if (field == first_field || base_type->tag == Type_Tag_compound && check_compound_for_recursive_fields_continued(&base_type->compound, first_field))
                return true;
        }
    }

    return false;
}

void check_compound_for_recursive_fields(const Compound_Type* compound)
{
    for (Array_Size i = 0; i < compound->fields.handles_length; i++)
    {
        Field* field = lookup_field(field_at(&compound->fields, i));
        Type* type = lookup_type(field->type); 
        
        if (type->tag == Type_Tag_compound)
        {
            if (!check_compound_for_recursive_fields_continued(&type->compound, field))
                continue;
        }
        else if (type->tag == Type_Tag_fixed_array)
        {
            Type* base_type = lookup_type(type->fixed_array.base_type);
            if (base_type->tag != Type_Tag_compound || !check_compound_for_recursive_fields_continued(&base_type->compound, field))
                continue;
        }
        else if (type->tag == Type_Tag_dynamic_array)
        {
            Type* base_type = lookup_type(type->dynamic_array.base_type);
            if (base_type->tag != Type_Tag_compound || !check_compound_for_recursive_fields_continued(&base_type->compound, field))
                continue;
        }
        else
            continue;

        print_span_error(lookup_file(compound->file), lookup_type_reference(field->type_reference)->span, "%c is recursive.", compound);
    }
}

void assign_types_to_compound_fields(const Compound_Type* compound)
{
    File* file = lookup_file(compound->file);
    for (Array_Size i = 0; i < compound->fields.handles_length; i++)
    {
        Field* field = lookup_field(field_at(&compound->fields, i));   
        field->type = lookup_type_by_reference(file, field->type_reference, true);
    }
}

void check_public_compound_for_fields_with_non_public_type(const Compound_Type* compound)
{
    if ((compound->attributes & Compound_Attribute_is_public) == 0)
        return;
    
    for (Array_Size i = 0; i < compound->fields.handles_length; i++)
    {
        Field_Handle field_handle = field_at(&compound->fields, i);
        Field* field = lookup_field(field_handle);
        
        if (!is_public_type(field->type))
            print_span_error(lookup_file(compound->file), lookup_type_reference(field->type_reference)->span, "%c exposes %t which is not public.", compound, field->type);
    }
}

// TODO: This is the same ascheck_for_nested_formal_type_parameters_in_reference for functions. But what about Variables?
void check_if_compound_has_nested_formal_type_parameter(const Compound_Type* compound, Type_Reference_Handle reference_handle)
{
    Type_Reference* reference = lookup_type_reference(reference_handle);
    if (reference->tag == Type_Reference_Tag_pointer)
        check_if_compound_has_nested_formal_type_parameter(compound, reference->pointer.base_type);
    else if (reference->tag == Type_Reference_Tag_dynamic_array)
        check_if_compound_has_nested_formal_type_parameter(compound, reference->dynamic_array.base_type);
    else if (reference->tag == Type_Reference_Tag_fixed_array)
        check_if_compound_has_nested_formal_type_parameter(compound, reference->fixed_array.base_type);
    else if (reference->tag == Type_Reference_Tag_function)
        not_implemented(__FILE__, __LINE__, "Type_Reference_Tag_function.");
    else if (reference->tag == Type_Reference_Tag_polymorphic_compound)
    {
        for (Array_Size i = 0; i < compound->formal_type_parameters.handles_length; i++)
        {
            Named_Type_Reference* formal_type_parameter = &lookup_type_reference(type_reference_at(&compound->formal_type_parameters, i))->named;
            if (compare_interned_strings(reference->named.name, formal_type_parameter->name))
                print_span_error(lookup_file(compound->file), reference->named.span, "Type parameters can't be nested.");
        }

        for (Array_Size i = 0; i < reference->polymorphic_compound.actual_type_parameters.handles_length; i++)
            check_if_compound_has_nested_formal_type_parameter(compound, type_reference_at(&reference->polymorphic_compound.actual_type_parameters, i));
    }
}

void check_if_compound_has_nested_formal_type_parameters(const Compound_Type* compound)
{
    for (Array_Size i = 0; i < compound->fields.handles_length; i++)
        check_if_compound_has_nested_formal_type_parameter(compound, lookup_field(field_at(&compound->fields, i))->type_reference);
}

void check_if_compounds_formal_type_parameters_matches_any_types(Type_Handle compound_handle)
{
    Compound_Type* compound = &lookup_type(compound_handle)->compound;
    File* file = lookup_file(compound->file);
    for (Array_Size i = 0; i < compound->formal_type_parameters.handles_length; i++)
    {
        Type_Reference_Handle type_reference_handle = type_reference_at(&compound->formal_type_parameters, i);
        Type_Handle type_handle = lookup_type_by_reference(file, type_reference_handle, false);
        if (is_invalid_type_handle(type_handle))
            continue;

        Named_Type_Reference* formal_type_parameter = &lookup_type_reference(type_reference_handle)->named;
        File* compound_file = lookup_file(compound->file);
        
        print_declaration_header(compound_file, formal_type_parameter->span.start, "Formal type parameter matches %I.", formal_type_parameter->name);
        print_type_declaration(type_handle);
        print_type_declaration(compound_handle);

        exit_with_error_code();
    }
}

void check_if_compound_has_duplicate_formal_type_parameters(const Compound_Type* compound)
{
    for (Array_Size a = 0; a < compound->formal_type_parameters.handles_length; a++)
    {
        Named_Type_Reference* a_reference = &lookup_type_reference(type_reference_at(&compound->formal_type_parameters, a))->named;
        for (Array_Size b = a + 1; b < compound->formal_type_parameters.handles_length; b++)
        {
            Named_Type_Reference* b_reference = &lookup_type_reference(type_reference_at(&compound->formal_type_parameters, b))->named;
            if (compare_interned_strings(a_reference->name, b_reference->name))
                print_span_error(lookup_file(compound->file), b_reference->span, "Duplicate formal type parameter %I.", b_reference->name);
        }
    }
}

void check_if_compound_has_duplicate_field_names(const Compound_Type* compound)
{
    for (Array_Size a = 0; a < compound->fields.handles_length; a++)
    {
        Field* a_field = lookup_field(field_at(&compound->fields, a));
        for (Array_Size b = a + 1; b < compound->fields.handles_length; b++)
        {
            Field* b_field = lookup_field(field_at(&compound->fields, b));
            if (compare_interned_strings(a_field->name, b_field->name))
                print_span_error(lookup_file(compound->file), b_field->name_span, "Duplicate field name %I.", b_field->name);
        }
    }
}

static Field_Handle deep_copy_field(const Field* polymorphic)
{
    Field_Handle handle = add_field();
    Field* monomorphic = lookup_field(handle);
    
    monomorphic->name = polymorphic->name;
    monomorphic->name_span = polymorphic->name_span;
    monomorphic->type_reference = add_type_reference();
    
    deep_copy_type_reference(monomorphic->type_reference, polymorphic->type_reference);
    
    return handle;
}

Type_Handle monomorphisize_compound(Type_Handle polymorphic_handle, const Compound_Type* polymorphic)
{
    Type_Handle handle = add_type();
    
    Compound_Type* monomorphic = &lookup_type(handle)->compound;
    monomorphic->tag = Type_Tag_compound;
    monomorphic->attributes = polymorphic->attributes & ~Compound_Attribute_is_polymorphic;
    monomorphic->name = polymorphic->name;
    monomorphic->name_span = polymorphic->name_span;
    monomorphic->file = polymorphic->file;
    monomorphic->template = polymorphic_handle;
    
    reserve_field_handles(&monomorphic->fields, polymorphic->fields.handles_length);
    for (uint8_t a = 0; a < polymorphic->fields.handles_length; a++)
        add_to_field_array(&monomorphic->fields, deep_copy_field(lookup_field(field_at(&polymorphic->fields, a))));

    for (uint8_t i = 0; i < polymorphic->formal_type_parameters.handles_length; i++)
        add_to_type_array(&monomorphic->actual_type_parameters, current_polymorphic_type_mapping->monomorphic_types[i]);
    
    current_polymorphic_type_mapping->monomorphisized_compound = monomorphic;
    assign_types_to_compound_fields(monomorphic);
    check_public_compound_for_fields_with_non_public_type(monomorphic);
    check_compound_for_recursive_fields(monomorphic);

    return handle;
}
