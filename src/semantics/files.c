#include "semantics.h"
#include <assert.h>
#include <string.h>

static bool type_declarations_match(const Type* a, const Type* b)
{
    if (a->tag == Type_Tag_compound && b->tag == Type_Tag_compound)
    {
        const Compound_Type* a_compound = &a->compound;
        const Compound_Type* b_compound = &b->compound;
        
        return compare_interned_strings(a_compound->name, b_compound->name) && 
               a_compound->formal_type_parameters.handles_length == b_compound->formal_type_parameters.handles_length;
    }
    else if (a->tag == Type_Tag_enumeration && b->tag == Type_Tag_enumeration)
        return compare_interned_strings(a->enumeration.name, b->enumeration.name);
    else if (a->tag == Type_Tag_compound && b->tag == Type_Tag_enumeration)
        return type_declarations_match(b, a);
    else if (a->tag == Type_Tag_enumeration)
        return compare_interned_strings(a->enumeration.name, b->enumeration.name);
    else if (b->tag == Type_Tag_compound)
        return compare_interned_strings(a->compound.name, b->compound.name) &&
               !b->compound.formal_type_parameters.handles_length;
    else
        return false;
}

static Interned_String_Handle name_of_type(const Type* type)
{
    if (type->tag == Type_Tag_enumeration)
        return type->enumeration.name;
    else if (type->tag == Type_Tag_compound)
        return type->compound.name;
    else if (type->tag <= Type_Tag_bool)
        return type->primitive.name;

    assert(false);
    return invalid_interned_string_handle;
}

File* file_of_type(const Type* type)
{
    if (type->tag == Type_Tag_enumeration)
        return lookup_file(type->enumeration.file);
    else if (type->tag == Type_Tag_compound)
        return lookup_file(type->compound.file);
    else
        return NULL;
}

Position start_of_type(const Type* type)
{
    if (type->tag == Type_Tag_enumeration)
        return type->enumeration.name_span.start;
    else if (type->tag == Type_Tag_compound)
        return type->compound.name_span.start;
    
    assert(false);
    return (Position) { 0 };
}

static void check_that_types_in_file_scope_are_not_matching(const File* file)
{
    bool has_errors = false;
    for (uint16_t a = 0; a < file->types_in_scope.handles_length; a++)
    {
        Type_Handle a_type_handle = file->types_in_scope.handles[a];
        Type* a_type = lookup_type(a_type_handle);
        
        for (uint16_t b = a + 1; b < file->types_in_scope.handles_length; b++)
        {
            Type_Handle b_type_handle = file->types_in_scope.handles[b];
            Type* b_type = lookup_type(b_type_handle);
            
            if (type_declarations_match(a_type, b_type))
            {
                if (!has_errors)
                {
                    print_declaration_header(file, start_of_type(a_type), "Matching declarations for %I in scope.", name_of_type(a_type));
                    print_type_declaration(a_type_handle);
                }

                print_type_declaration(b_type_handle);
                has_errors = true;
            }
        }

        if (has_errors)
            exit_with_error_code();
    }
}

static void check_that_functions_in_scope_are_not_matching(const File* file)
{
    bool has_errors = false;
    for (uint16_t a = 0; a < file->functions_in_scope.handles_length; a++)
    {
        Function_Handle a_function_handle = file->functions_in_scope.handles[a];
        Function* a_function = lookup_function(a_function_handle);
        
        for (uint16_t b = a + 1; b < file->functions_in_scope.handles_length; b++)
        {
            Function_Handle b_function_handle = file->functions_in_scope.handles[b];
            Function* b_function = lookup_function(b_function_handle);
            
            if (!compare_interned_strings(a_function->name, b_function->name))
                continue;

            if (signatures_match(a_function_handle, b_function_handle))   
            {
                if (!has_errors)
                {
                    print_declaration_header(file, a_function->name_span.start, "Matching declarations for %I in scope.", a_function->name);
                    print_function_declaration(a_function);
                }
                
                print_function_declaration(b_function);
                has_errors = true;
            }  
        }

        if (has_errors)
            exit_with_error_code();
    }
}

static void put_declarations_from_used_namespaces_into_scope(File* a_file)
{
    for (uint16_t i = 0; i < a_file->type_declarations.handles_length; i++)
        add_to_type_array(&a_file->types_in_scope, a_file->type_declarations.handles[i]);

    for (uint16_t i = 0; i < a_file->function_declarations.handles_length; i++)
        add_to_function_array(&a_file->functions_in_scope, a_file->function_declarations.handles[i]);

    Namespace* a_namespace = lookup_namespace(a_file->namespace);
    for (uint8_t b = 0; b < files_length; b++)
    {
        File* b_file = &files[b];
        if (a_file == b_file)
            continue;
        
        Namespace* b_namespace = lookup_namespace(b_file->namespace);
        if (!compare_interned_strings(a_namespace->name, b_namespace->name))
        {
            bool b_file_is_used = false;
            for (uint8_t i = 0; i < a_file->uses.handles_length; i++)
            {
                Namespace* use = lookup_namespace(a_file->uses.handles[i]);
                if (compare_interned_strings(use->name, b_namespace->name))
                {
                    b_file_is_used = true;
                    break;
                }
            }

            if (!b_file_is_used)
                continue;
        }

        for (uint16_t i = 0; i < b_file->type_declarations.handles_length; i++)
        {
            if (is_public_type(b_file->type_declarations.handles[i]))
                add_to_type_array(&a_file->types_in_scope, b_file->type_declarations.handles[i]);
        }

        for (uint16_t i = 0; i < b_file->function_declarations.handles_length; i++)
        {
            if (lookup_function(b_file->function_declarations.handles[i])->attributes & Function_Attribute_is_public)
                add_to_function_array(&a_file->functions_in_scope, b_file->function_declarations.handles[i]);
        }
    }
}

Type_Handle lookup_tuple_type_by_signature(Type_Handle signature_handle)
{
    Type* signature = lookup_type(signature_handle);
    assert(signature->tag == Type_Tag_function);
    
    for (uint16_t a = 0; a < types_length; a++)
    {
        Type* type = &types[a];
        if (type->tag != Type_Tag_tuple || 
            type->tuple.types.handles_length != signature->function.return_types.handles_length)
            continue;

        bool is_match = true;
        for (uint8_t b = 0; b < signature->function.return_types.handles_length; b++)
        {
            if (!compare_types(type->tuple.types.handles[b], signature->function.return_types.handles[b]))
            {
                is_match = false;
                break;
            }
        }
        
        if (is_match)
            return (Type_Handle) { .index = a };
    }

    Type_Handle tuple_handle = add_type();
    Type* tuple = lookup_type(tuple_handle);
    
    tuple->tag = Type_Tag_tuple;
    for (uint8_t i = 0; i < signature->function.return_types.handles_length; i++)
        add_to_type_array(&tuple->tuple.types, signature->function.return_types.handles[i]);

    return tuple_handle;
}

bool is_public_type(Type_Handle handle)
{
    Type* type = lookup_type(handle);
    if (type->tag == Type_Tag_compound)
        return type->compound.attributes & Compound_Attribute_is_public;
    else if (type->tag == Type_Tag_enumeration)
        return type->enumeration.is_public;
    else
        return true;
}

Type_Handle i8_handle;
Type_Handle i16_handle;
Type_Handle i32_handle;
Type_Handle i64_handle;
Type_Handle u8_handle;
Type_Handle u16_handle;
Type_Handle u32_handle;
Type_Handle u64_handle;
Type_Handle f32_handle;
Type_Handle f64_handle;
Type_Handle bool_handle;
Type_Handle none_handle;
Type_Handle utf8_string_literal_type_handle;

Type_Handle add_primitive_type(char* name, uint8_t tag, uint16_t bytes)
{
    Type_Handle handle = add_type();
    Primitive_Type* type = &lookup_type(handle)->primitive;

    type->tag = tag;
    type->size_of = bytes;
    type->allignment_of = bytes;
    type->name = add_interned_string((String)
    { 
        .text = name, 
        .length = (uint32_t)strlen(name) 
    });

    return handle;
}

static void add_primitive_types(void)
{
    i8_handle = add_primitive_type("I8", Type_Tag_i8, 1);
    i16_handle = add_primitive_type("I16", Type_Tag_i16, 2);
    i32_handle = add_primitive_type("I32", Type_Tag_i32, 4);
    i64_handle = add_primitive_type("I64", Type_Tag_i64, 8);
    u8_handle = add_primitive_type("U8", Type_Tag_u8, 1);
    u16_handle = add_primitive_type("U16", Type_Tag_u16, 2);
    u32_handle = add_primitive_type("U32", Type_Tag_u32, 4);
    u64_handle = add_primitive_type("U64", Type_Tag_u64, 8);
    f32_handle = add_primitive_type("F32", Type_Tag_f32, 4);
    f64_handle = add_primitive_type("F64", Type_Tag_f64, 8);
    bool_handle = add_primitive_type("Bool", Type_Tag_bool, 1);
    none_handle = add_primitive_type("none", Type_Tag_none, 0);
}

Interned_String_Handle length_field;
Interned_String_Handle capacity_field;
Interned_String_Handle elements_field;

static void intern_array_fields(void)
{
    length_field   = add_interned_string((String) { .text = "length",   .length = 6 });
    capacity_field = add_interned_string((String) { .text = "capacity", .length = 8 });
    elements_field = add_interned_string((String) { .text = "elements", .length = 8 });
}

static void add_utf8_string_literal_type(void)
{
    Type_Handle array_handle = add_type();
    Dynamic_Array_Type* array = &lookup_type(array_handle)->dynamic_array;
    array->tag = Type_Tag_dynamic_array;
    array->base_type = u8_handle;

    utf8_string_literal_type_handle = add_type();
    Pointer_Type* pointer = &lookup_type(utf8_string_literal_type_handle)->pointer;
    pointer->tag = Type_Tag_pointer;
    pointer->base_type = array_handle;
    pointer->privileges = ALL_PRIVILEGES;
}

void analyze_program(void)
{
    start_timer(Timer_analysis);

    add_primitive_types();
    intern_array_fields();
    add_utf8_string_literal_type();

    // Compound types might depend on non-primitive types defined in any file of the program.
    // Therefore all types visible to a file must be put into scope before filling in the type of fields.
    // If not the compiler will run into null pointers depending on file order among other things.
    for (uint8_t a = 0; a < files_length; a++)
        put_declarations_from_used_namespaces_into_scope(&files[a]);

    for (uint8_t a = 0; a < files_length; a++)
    {
        File* file = &files[a];
        for (uint16_t b = 0; b < file->type_declarations.handles_length; b++)
        {
            Compound_Type* type = &lookup_type(file->type_declarations.handles[b])->compound;
            if (type->tag != Type_Tag_compound)
                continue;

            check_if_compound_has_duplicate_field_names(type);
            if (!(type->attributes & Compound_Attribute_is_polymorphic))
            {
                assign_types_to_compound_fields(type);
                check_public_compound_for_fields_with_non_public_type(type);
            }
        }

        check_that_types_in_file_scope_are_not_matching(file);
        for (uint16_t b = 0; b < file->type_declarations.handles_length; b++)
        {
            Compound_Type* type = &lookup_type(file->type_declarations.handles[b])->compound;
            if (type->tag == Type_Tag_compound && (type->attributes & Compound_Attribute_is_polymorphic))
            {
                check_if_compound_has_nested_formal_type_parameters(type);
                check_if_compound_has_duplicate_formal_type_parameters(type);
                check_if_compounds_formal_type_parameters_matches_any_types(file->type_declarations.handles[b]);
            }
        }
    }

    for (uint8_t a = 0; a < files_length; a++)
    {
        File* file = &files[a];
        for (uint16_t b = 0; b < file->type_declarations.handles_length; b++)
        {
            Compound_Type* type = &lookup_type(file->type_declarations.handles[b])->compound;
            if (type->tag == Type_Tag_compound && (type->attributes & Compound_Attribute_is_polymorphic) == 0)
                check_compound_for_recursive_fields(type);
        }
    }

    for (uint8_t a = 0; a < files_length; a++)
    {
        File* file = &files[a];
        for (uint16_t b = 0; b < file->type_declarations.handles_length; b++)
        {
            Enumeration_Type* type = &lookup_type(file->type_declarations.handles[b])->enumeration;
            if (type->tag == Type_Tag_enumeration)
                type_check_enumeration_declaration(type);
        }

        for (uint16_t b = 0; b < file->function_declarations.handles_length; b++)
            type_check_signature(file, lookup_function(file->function_declarations.handles[b]));
    }

    Interned_String_Handle main_name = add_interned_string((String) { .text = "main", .length = 4 });
    for (uint8_t a = 0; a < files_length; a++)
    {
        File* file = &files[a];

        check_that_functions_in_scope_are_not_matching(file);
        for (uint16_t b = 0; b < file->function_declarations.handles_length; b++)
        {
            Function_Handle function_handle = file->function_declarations.handles[b];
            Function* function = lookup_function(function_handle);
            if (compare_interned_strings(function->name, main_name))
            {
                if (!is_invalid_function_handle(main_function))
                    print_span_error(file, function->name_span, "main function redeclared.");

                main_function = function_handle;
            }

            if ((function->attributes & Function_Attribute_is_polymorphic) == 0)
                type_check_function_body(file, function);
        }
    }

    if (is_invalid_function_handle(main_function))
        print_error("No declared main function.");
}

Type_Handle lookup_signature(const File* file, const Function* function)
{
    for (uint16_t a = 0; a < types_length; a++)
    {
        Type* type = &types[a];
        if (type->tag != Type_Tag_function || 
            type->function.formal_parameters.handles_length != function->formal_parameters.handles_length || 
            type->function.return_types.handles_length != function->return_types.handles_length ||
           !type->function.fully_constructed)
            continue;

        bool is_match = true;
        for (uint8_t b = 0; b < function->formal_parameters.handles_length; b++)
        {
            Formal_Parameter* formal_parameter = lookup_formal_parameter(function->formal_parameters.handles[b]);
            if (!compare_types(lookup_type_by_reference(file, formal_parameter->type, true), type->function.formal_parameters.handles[b]))
            {
                is_match = false;
                break;
            }
        }

        if (!is_match)
            continue;

        for (uint8_t b = 0; b < function->return_types.handles_length; b++)
        {
            if (!compare_types(lookup_type_by_reference(file, function->return_types.handles[b], true), type->function.return_types.handles[b]))
            {
                is_match = false;
                break;
            }
        }

        if (is_match)
            return (Type_Handle) { .index = a };
    }

    Type_Handle type_handle = add_type();
    Function_Type* type = &lookup_type(type_handle)->function;
    type->tag = Type_Tag_function;
    
    for (uint8_t i = 0; i < function->formal_parameters.handles_length; i++)
    {
        Formal_Parameter* formal_parameter = lookup_formal_parameter(function->formal_parameters.handles[i]);
        add_to_type_array(&type->formal_parameters, lookup_type_by_reference(file, formal_parameter->type, true));
    }

    for (uint8_t i = 0; i < function->return_types.handles_length; i++)
        add_to_type_array(&type->return_types, lookup_type_by_reference(file, function->return_types.handles[i], true));

    type->fully_constructed = true;

    return type_handle;
}

Type_Handle find_or_add_pointer_type(Type_Handle base_type, Pointer_Type_Privilege privileges)
{
    for (Type_Handle handle = { 0 }; handle.index < types_length; handle.index++)
    {
        Pointer_Type* type = &lookup_type(handle)->pointer;
        if (type->tag == Type_Tag_pointer && type->privileges == privileges && compare_types(type->base_type, base_type))
            return handle;
    }

    Type_Handle handle = add_type();
    Pointer_Type* type = &lookup_type(handle)->pointer;
    type->tag = Type_Tag_pointer;
    type->privileges = privileges;
    type->base_type = base_type;
    type->size_of = 8;

    return handle;
}

Type_Handle lookup_type_by_reference(const File* file, Type_Reference_Handle reference_handle, bool error_if_undefined)
{
    Type_Reference* reference = lookup_type_reference(reference_handle);
    if (reference->tag == Type_Reference_Tag_name)
    {
        // Primitives are implicitly in scope of every file.
        for (Type_Handle handle = i8_handle; handle.index < none_handle.index; handle.index++)
        {
            Primitive_Type* type = &lookup_type(handle)->primitive;
            if (compare_interned_strings(type->name, reference->named.name))
                return handle;
        }

        for (uint16_t i = 0; i < file->types_in_scope.handles_length; i++)
        {
            Type_Handle type_handle = file->types_in_scope.handles[i];
            Type* type = lookup_type(type_handle);
            if (type->tag == Type_Tag_compound)
            {
                if (!compare_interned_strings(type->compound.name, reference->named.name) || (type->compound.attributes & Compound_Attribute_is_polymorphic))
                    continue;
            }
            else if (type->tag == Type_Tag_enumeration)
            {
                if (!compare_interned_strings(type->enumeration.name, reference->named.name))
                    continue;
            }
            else
                continue;

            return type_handle;
        }

        if (current_polymorphic_type_mapping)
        {
            for (uint8_t i = 0; i < current_polymorphic_type_mapping->length; i++)
            {
                Named_Type_Reference* polymorphic = &lookup_type_reference(current_polymorphic_type_mapping->polymorphic_types[i])->named;
                if (compare_interned_strings(reference->named.name, polymorphic->name))
                    return current_polymorphic_type_mapping->monomorphic_types[i];
            }
        }

        if (error_if_undefined)
            print_span_error(file, reference->span, "Undefined type referenced.");

        return invalid_type_handle;
    }
    else if (reference->tag == Type_Reference_Tag_pointer)
        return find_or_add_pointer_type(lookup_type_by_reference(file, reference->pointer.base_type, error_if_undefined), reference->pointer.privileges);
    else if (reference->tag == Type_Reference_Tag_function)
    {
        for (Type_Handle handle = { 0 }; handle.index < types_length; handle.index++)
        {
            Function_Type* type = &lookup_type(handle)->function;
            if (type->tag != Type_Tag_function ||
                type->formal_parameters.handles_length != reference->function.formal_parameters.handles_length ||
                type->return_types.handles_length != reference->function.return_types.handles_length ||
               !type->fully_constructed)
                continue;

            bool is_match = true;
            for (uint8_t i = 0; i < reference->function.formal_parameters.handles_length; i++)
            {
                if (!compare_types(lookup_type_by_reference(file, reference->function.formal_parameters.handles[i], true), type->formal_parameters.handles[i]))
                {
                    is_match = false;
                    break;
                }
            }

            if (!is_match)
                continue;

            for (uint8_t i = 0; i < reference->function.return_types.handles_length; i++)
            {
                if (!compare_types(lookup_type_by_reference(file, reference->function.return_types.handles[i], true), type->return_types.handles[i]))
                {
                    is_match = false;
                    break;
                }
            }

            if (is_match)
                return handle;
        }

        Type_Handle handle = add_type();
        Function_Type* type = &lookup_type(handle)->function;
        type->tag = Type_Tag_function;

        for (uint8_t i = 0; i < reference->function.formal_parameters.handles_length; i++)
            add_to_type_array(&type->formal_parameters, lookup_type_by_reference(file, reference->function.formal_parameters.handles[i], true));

        for (uint8_t i = 0; i < reference->function.return_types.handles_length; i++)
            add_to_type_array(&type->return_types, lookup_type_by_reference(file, reference->function.return_types.handles[i], true));

        type->fully_constructed = true;

        return handle;
    }
    else if (reference->tag == Type_Reference_Tag_dynamic_array)
    {
        Type_Handle base_type = lookup_type_by_reference(file, reference->dynamic_array.base_type, error_if_undefined);
        for (Type_Handle handle = { 0 }; handle.index < types_length; handle.index++)
        {
            Dynamic_Array_Type* type = &lookup_type(handle)->dynamic_array;
            if (type->tag == Type_Tag_dynamic_array && compare_types(type->base_type, base_type))
                return handle;
        }

        Type_Handle handle = add_type();
        Dynamic_Array_Type* type = &lookup_type(handle)->dynamic_array;
        type->tag = Type_Tag_dynamic_array;
        type->base_type = base_type;

        return handle;
    }
    else if (reference->tag == Type_Reference_Tag_fixed_array)
    {
        Type_Handle base_type = lookup_type_by_reference(file, reference->fixed_array.base_type, error_if_undefined);
        Number size = type_check_integer_literal(file, reference->fixed_array.size, reference->fixed_array.size_span, u32_handle);

        for (Type_Handle handle = { 0 }; handle.index < types_length; handle.index++)
        {
            Fixed_Array_Type* type = &lookup_type(handle)->fixed_array;
            if (type->tag == Type_Tag_fixed_array && type->size == size.u32 && compare_types(type->base_type, base_type))
                return handle;
        }

        Type_Handle handle = add_type();
        Fixed_Array_Type* type = &lookup_type(handle)->fixed_array;
        type->tag = Type_Tag_fixed_array;
        type->size = size.u32;
        type->base_type = base_type;
        type->size_of = lookup_type(base_type)->size_of * size.u16;

        return handle;
    }
    else if (reference->tag == Type_Reference_Tag_polymorphic_compound)
    {
        bool found_type = false;
        Type_Handle polymorphic_compound_handle = invalid_type_handle;
        Compound_Type* polymorphic_compound = NULL;

        for (Type_Handle a = { 0 }; a.index < types_length; a.index++)
        {
            Compound_Type* a_type = &lookup_type(a)->compound;
            if (a_type->tag != Type_Tag_compound ||
               (a_type->attributes & Compound_Attribute_is_polymorphic) == 0 ||
                !compare_interned_strings(a_type->name, reference->polymorphic_compound.name) ||
                a_type->formal_type_parameters.handles_length != reference->polymorphic_compound.actual_type_parameters.handles_length)
                continue;

            polymorphic_compound_handle = a;
            found_type = true;

            // Monomorphic compounds are always placed after their polymorphic variant 
            // so skip types before the polymorphic compound.
            for (Type_Handle b = { .index = a.index + 1 }; b.index < types_length; b.index++)
            {
                Compound_Type* b_type = &lookup_type(b)->compound;
                if (b_type->tag != Type_Tag_compound || !compare_types(polymorphic_compound_handle, b_type->template))
                    continue;

                bool is_match = true;
                for (uint8_t c = 0; c < b_type->actual_type_parameters.handles_length; c++)
                {
                    Type_Handle actual_type_parameter = lookup_type_by_reference(file, reference->polymorphic_compound.actual_type_parameters.handles[c], error_if_undefined);
                    if (!compare_types(b_type->actual_type_parameters.handles[c], actual_type_parameter))
                    {
                        is_match = false;
                        break;
                    }
                }

                if (is_match)
                    return b;
            }

            polymorphic_compound = a_type;

            break;
        }

        if (!polymorphic_compound)
            print_span_error(file, lookup_type_reference(reference_handle)->span, found_type ? "Mismatched type parameters" : "Undefined type referenced.");

        Polymorphic_Type_Mapping map = { 0 };
        map.length = reference->polymorphic_compound.actual_type_parameters.handles_length;
        map.file = file;
        map.origin = reference->span;

        for (uint8_t i = 0; i < map.length; i++)
        {
            map.polymorphic_types[i] = polymorphic_compound->formal_type_parameters.handles[i];
            map.monomorphic_types[i] = lookup_type_by_reference(file, reference->polymorphic_compound.actual_type_parameters.handles[i], error_if_undefined);
        }

        acquire_polymorphic_type_mapping();
        *current_polymorphic_type_mapping = map;

        Type_Handle monomorphic_compound = monomorphisize_compound(polymorphic_compound_handle, polymorphic_compound);

        release_polymorphic_type_mapping();

        return monomorphic_compound;
    }
    else
        unexpected_reference_type(__FILE__, __LINE__, reference->tag);

    return invalid_type_handle;
}

void deep_copy_type_references(Type_Reference_Handle_Array* restrict destination, const Type_Reference_Handle_Array* restrict source)
{
    reserve_type_reference_handles(destination, source->handles_length);
    for (uint8_t i = 0; i < source->handles_length; i++)
    {
        add_to_type_reference_array(destination, add_type_reference());
        deep_copy_type_reference(destination->handles[i], source->handles[i]);
    }
}

static void type_to_type_reference(Type_Reference_Handle destination_handle, Type_Handle source_handle, Span span_of_t)
{
    Type* source = lookup_type(source_handle);
    Type_Reference* destination = lookup_type_reference(destination_handle);

    destination->span = span_of_t;
    if (source->tag <= Type_Tag_bool)
    {
        destination->tag = Type_Reference_Tag_name;
        destination->named.name = source->primitive.name;
    }
    else if (source->tag == Type_Tag_enumeration)
    {
        destination->tag = Type_Reference_Tag_name;
        destination->named.name = source->enumeration.name;
    }
    else if (source->tag == Type_Tag_compound)
    {
        if (is_invalid_type_handle(source->compound.template))
        {
            destination->tag = Type_Reference_Tag_name;
            destination->named.name = source->compound.name;
        }
        else
        {
            destination->tag = Type_Reference_Tag_polymorphic_compound;
            reserve_type_reference_handles(&destination->polymorphic_compound.actual_type_parameters, source->compound.actual_type_parameters.handles_length);
            for (uint8_t i = 0; i < source->compound.actual_type_parameters.handles_length; i++)
            {
                add_to_type_reference_array(&destination->polymorphic_compound.actual_type_parameters, add_type_reference());
                type_to_type_reference(destination->polymorphic_compound.actual_type_parameters.handles[i], source->compound.actual_type_parameters.handles[i], span_of_t);
            }
        }
    }
    else if (source->tag == Type_Tag_function)
        not_implemented(__FILE__, __LINE__, "Type_Tag_function");
    else if (source->tag == Type_Tag_pointer)
    {
        destination->tag = Type_Reference_Tag_pointer;
        destination->pointer.privileges = source->pointer.privileges;
        destination->pointer.base_type = add_type_reference();
        
        type_to_type_reference(destination->pointer.base_type, source->pointer.base_type, span_of_t);
    }
    else if (source->tag == Type_Tag_dynamic_array)
        not_implemented(__FILE__, __LINE__, "Type_Tag_dynamic_array");
    else if (source->tag == Type_Tag_fixed_array)
        not_implemented(__FILE__, __LINE__, "Type_Tag_fixed_array");
    else
        unexpected_type(__FILE__, __LINE__, source->tag);
}

void deep_copy_type_reference(Type_Reference_Handle destination_handle, Type_Reference_Handle source_handle)
{
    Type_Reference* destination = lookup_type_reference(destination_handle);
    Type_Reference* source = lookup_type_reference(source_handle);

    destination->tag = source->tag;
    destination->span = source->span;

    if (source->tag == Type_Reference_Tag_name)
    {
        if (current_polymorphic_type_mapping)
        {
            for (uint8_t i = 0; i < current_polymorphic_type_mapping->length; i++)
            {
                Named_Type_Reference* polymorphic = &lookup_type_reference(current_polymorphic_type_mapping->polymorphic_types[i])->named;
                if (compare_interned_strings(source->named.name, polymorphic->name))
                {
                    type_to_type_reference(destination_handle, current_polymorphic_type_mapping->monomorphic_types[i], source->span);
                    return;
                }
            }
        }

        destination->named = source->named;
    }
    else if (source->tag == Type_Reference_Tag_pointer)
    {
        destination->pointer.privileges = source->pointer.privileges;
        destination->pointer.base_type = add_type_reference();

        deep_copy_type_reference(destination->pointer.base_type, source->pointer.base_type);
    }
    else if (source->tag == Type_Reference_Tag_dynamic_array)
    {
        destination->dynamic_array.base_type = add_type_reference();
        deep_copy_type_reference(destination->dynamic_array.base_type, source->dynamic_array.base_type);
    }
    else if (source->tag == Type_Reference_Tag_fixed_array)
    {
        destination->fixed_array.size = source->fixed_array.size;
        destination->fixed_array.base_type = add_type_reference();
        deep_copy_type_reference(destination->fixed_array.base_type, source->fixed_array.base_type);
    }
    else if (source->tag == Type_Reference_Tag_function)
    {
        deep_copy_type_references(&destination->function.formal_parameters, &source->function.formal_parameters);
        deep_copy_type_references(&destination->function.return_types, &source->function.return_types);
    }
    else if (source->tag == Type_Reference_Tag_polymorphic_compound)
    {
        destination->polymorphic_compound.name = source->polymorphic_compound.name;
        reserve_type_reference_handles(&destination->polymorphic_compound.actual_type_parameters, source->polymorphic_compound.actual_type_parameters.handles_length);
        for (uint8_t i = 0; i < source->polymorphic_compound.actual_type_parameters.handles_length; i++)
        {
            Type_Reference_Handle type_parameter_destination = add_type_reference();

            deep_copy_type_reference(type_parameter_destination, source->polymorphic_compound.actual_type_parameters.handles[i]);
            add_to_type_reference_array(&destination->polymorphic_compound.actual_type_parameters, type_parameter_destination);
        }
    }
    else
        unexpected_reference_type(__FILE__, __LINE__, source->tag);
}

Symbol* symbol_of_reference(Interned_String_Handle name)
{
    for (uint16_t i = 0; i < current_symbol_table->symbols_length; i++)
    {
        Symbol* symbol = &current_symbol_table->symbols[i];
        if (compare_interned_strings(symbol->name, name))
        {
            symbol->is_referenced = true;
            return symbol;
        }
    }

    return NULL;
}

void check_that_identifier_is_undefined(const File* file, Interned_String_Handle name, Span name_span)
{
    if (symbol_of_reference(name))
        print_span_error(file, name_span, "%I is already defined.", name); 

    for (uint16_t i = 0; i < file->functions_in_scope.handles_length; i++)
    {
        Function* function = lookup_function(file->functions_in_scope.handles[i]);
        if (compare_interned_strings(function->name, name))
            print_span_error(file, name_span, "%I is already defined.", name);
    }
}

void check_for_unreferenced_variables(const File* file, uint8_t start_from_index)
{
    for (uint8_t i = start_from_index; i < current_symbol_table->symbols_length; i++)
    {
        Symbol* symbol = &current_symbol_table->symbols[i];
        if (!symbol->is_referenced)
            print_span_error(file, symbol->name_span, "Unused variable.");
    }
}
