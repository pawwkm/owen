#include "compiler.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

void print_type_reference_for_compound(const Compound_Type* compound, FILE* file)
{
    print_string_by_handle(compound->name, file);
    if (compound->attributes & Compound_Attribute_is_polymorphic)
    {
        fputc('[', file);
        print_string_by_handle(lookup_type_reference(compound->formal_type_parameters.handles[0])->named.name, file);

        for (uint8_t i = 1; i < compound->formal_type_parameters.handles_length; i++)
        {
            fputs(", ", file);
            print_string_by_handle(lookup_type_reference(compound->formal_type_parameters.handles[i])->named.name, file);
        }

        fputc(']', file);
    }
    else if (!is_invalid_type_handle(compound->template))
    {
        fputc('[', file);
        print_type_reference_for_type(compound->actual_type_parameters.handles[0], file);

        for (uint8_t i = 1; i < compound->actual_type_parameters.handles_length; i++)
        {
            fputs(", ", file);
            print_type_reference_for_type(compound->actual_type_parameters.handles[i], file);
        }

        fputc(']', file);
    }
}

static void print_type_reference_for_function(const Function_Type* function, FILE* file)
{
    fputs("Function(", file);
    if (function->formal_parameters.handles_length)
    {
        print_type_reference_for_type(function->formal_parameters.handles[0], file);
        for (uint8_t i = 1; i < function->formal_parameters.handles_length; i++)
        {
            fputs(", ", file);
            print_type_reference_for_type(function->formal_parameters.handles[i], file);
        }
    }

    fputc(')', file);
    if (function->return_types.handles_length)
    {
        fputs(" : ", file);
        print_type_reference_for_type(function->return_types.handles[0], file);

        for (uint8_t i = 1; i < function->return_types.handles_length; i++)
        {
            fputs(", ", file);
            print_type_reference_for_type(function->return_types.handles[i], file);
        }
    }
}

void print_type_reference_for_tuple(const Tuple_Type* tuple, FILE* file)
{
    print_type_reference_for_type(tuple->types.handles[0], file);
    for (uint8_t i = 1; i < tuple->types.handles_length; i++)
    {
        fputs(", ", file);
        print_type_reference_for_type(tuple->types.handles[i], file);
    }
}

void print_type_reference_for_type(const Type_Handle type_handle, FILE* file)
{
    Type* type = lookup_type(type_handle);
    if (type->tag <= Type_Tag_bool || type->tag == Type_Tag_none)
        print_string_by_handle(type->primitive.name, file);
    else if (type->tag == Type_Tag_enumeration)
        print_string_by_handle(type->enumeration.name, file);
    else if (type->tag == Type_Tag_compound)
        print_type_reference_for_compound(&type->compound, file);
    else if (type->tag == Type_Tag_pointer)
    {
        fputc('#', file);
        if (Pointer_Type_Privilege_retained & type->pointer.privileges)
            fputc('`', file);

        if (Pointer_Type_Privilege_alias & type->pointer.privileges)
            fputc('!', file);

        if (Pointer_Type_Privilege_readable & type->pointer.privileges)
            fputc('?', file);

        if (Pointer_Type_Privilege_writable & type->pointer.privileges)
            fputc('$', file);

        print_type_reference_for_type(type->pointer.base_type, file);
    }
    else if (type->tag == Type_Tag_function)
        print_type_reference_for_function(&type->function, file);
    else if (type->tag == Type_Tag_dynamic_array)
    {
        fputs("[]", file);
        print_type_reference_for_type(type->dynamic_array.base_type, file);
    }
    else if (type->tag == Type_Tag_fixed_array)
    {
        fprintf(file, "[%u]", type->fixed_array.size);
        print_type_reference_for_type(type->fixed_array.base_type, file);
    }
    else if (type->tag == Type_Tag_tuple)
        print_type_reference_for_tuple(&type->tuple, file);
}

void print_operator(uint8_t operator, FILE* file)
{
    assert(operator >= Binary_Operator_logical_or && operator <= Binary_Operator_right_shift);
    static char* operators[] =
    {
        [Binary_Operator_logical_or]            = "||",
        [Binary_Operator_logical_and]           = "&&",
        [Binary_Operator_equal_equal]           = "==",
        [Binary_Operator_not_equal]             = "!=",
        [Binary_Operator_less_than_or_equal]    = "<=",
        [Binary_Operator_greater_than_or_equal] = ">=",
        [Binary_Operator_less_than]             = "<",
        [Binary_Operator_greater_than]          = ">",
        [Binary_Operator_plus]                  = "+",
        [Binary_Operator_minus]                 = "-",
        [Binary_Operator_bitwise_or]            = "|",
        [Binary_Operator_bitwise_xor]           = "^",
        [Binary_Operator_multiply]              = "*",
        [Binary_Operator_divide]                = "/",
        [Binary_Operator_modulo]                = "%",
        [Binary_Operator_bitwise_and]           = "&",
        [Binary_Operator_left_shift]            = "<<",
        [Binary_Operator_right_shift]           = ">>"
    };

    fputs(operators[operator], file);
}

void print_string(String string, FILE* file)
{
    fprintf(file, "%.*s", string.length, string.text);
}

void print_string_by_handle(Interned_String_Handle handle, FILE* file)
{
    Interned_String* string = lookup_interned_string(handle);
    fprintf(file, "%.*s", string->length, string->text);
}

static void print_expression(const Expression* expression, bool is_not_polymorphic);

static void print_statements(const Statement_Handle_Array* body, bool is_not_polymorphic);

static uint8_t indentation;
static void print_indentation(void)
{
    for (uint8_t i = 0; i < indentation; i++)
        printf("    ");
}

static void print_type_reference(Type_Reference_Handle type_reference_handle, FILE* file)
{
    Type_Reference* type_reference = lookup_type_reference(type_reference_handle);
    if (type_reference->tag == Type_Reference_Tag_name)
        print_string_by_handle(type_reference->named.name, file);
    else if (type_reference->tag == Type_Reference_Tag_pointer)
    {
        fprintf(file, "#");
        if (type_reference->pointer.privileges & Pointer_Type_Privilege_retained)
            fprintf(file, "`");

        if (type_reference->pointer.privileges & Pointer_Type_Privilege_alias)
            fprintf(file, "!");

        if (type_reference->pointer.privileges & Pointer_Type_Privilege_readable)
            fprintf(file, "?");

        if (type_reference->pointer.privileges & Pointer_Type_Privilege_writable)
            fprintf(file, "$");

        print_type_reference(type_reference->pointer.base_type, file);
    }
    else if (type_reference->tag == Type_Reference_Tag_dynamic_array)
    {
        fprintf(file, "[]");
        print_type_reference(type_reference->dynamic_array.base_type, file);
    }
    else if (type_reference->tag == Type_Reference_Tag_fixed_array)
    {
        fprintf(file, "[");
        print_string(type_reference->fixed_array.size, file);
        fprintf(file, "]");

        print_type_reference(type_reference->fixed_array.base_type, file);
    }
    else if (type_reference->tag == Type_Reference_Tag_function)
    {
        fprintf(file, "Function(");
        if (type_reference->function.formal_parameters.handles_length)
        {
            print_type_reference(type_reference->function.formal_parameters.handles[0], file);
            for (uint8_t i = 1; i < type_reference->function.formal_parameters.handles_length; i++)
            {
                fprintf(file, ", ");
                print_type_reference(type_reference->function.formal_parameters.handles[i], file);
            }
        }

        fprintf(file, ")");
        if (type_reference->function.return_types.handles_length)
        {
            fprintf(file, " : ");
            print_type_reference(type_reference->function.return_types.handles[0], file);
            for (uint8_t i = 1; i < type_reference->function.return_types.handles_length; i++)
            {
                fprintf(file, ", ");
                print_type_reference(type_reference->function.return_types.handles[i], file);
            }
        }
    }
    else if (type_reference->tag == Type_Reference_Tag_polymorphic_compound)
    {
        print_string_by_handle(type_reference->polymorphic_compound.name, file);

        fprintf(file, "[");
        print_type_reference(type_reference->polymorphic_compound.actual_type_parameters.handles[0], file);
        for (uint8_t i = 1; i < type_reference->polymorphic_compound.actual_type_parameters.handles_length; i++)
        {
            fprintf(file, ", ");
            print_type_reference(type_reference->polymorphic_compound.actual_type_parameters.handles[i], file);
        }

        fprintf(file, "]");
    }
    else
        unexpected_reference_type(__FILE__, __LINE__, type_reference->tag);
}

static void print_compound_declaration(const Compound_Type* compound)
{
    print_indentation();
    printf(compound->attributes & Compound_Attribute_is_union ? "union\n" : "structure\n");
    indentation++;

    print_indentation();
    printf("name: ");
    print_string_by_handle(compound->name, stdout);
    putchar('\n');

    print_indentation();
    printf(compound->attributes & Compound_Attribute_is_public ? "is_public: true\n" : "is_public: false\n");

    if (compound->attributes & Compound_Attribute_is_polymorphic)
    {
        for (uint8_t i = 0; i < compound->formal_type_parameters.handles_length; i++)
        {
            print_indentation();
            printf("formal_type_parameter: ");
            print_type_reference(compound->formal_type_parameters.handles[i], stdout);
            putchar('\n');
        }
    }
    else if (!is_invalid_type_handle(compound->template))
    {
        for (uint8_t i = 0; i < compound->actual_type_parameters.handles_length; i++)
        {
            print_indentation();
            printf("actual_type_parameter: ");
            print_type_reference_for_type(compound->actual_type_parameters.handles[i], stdout);
            putchar('\n');
        }
    }

    for (uint8_t i = 0; i < compound->fields.handles_length; i++)
    {
        print_indentation();
        printf("field\n");

        indentation++;

        print_indentation();

        Field* field = lookup_field(compound->fields.handles[i]);
        if (compound->attributes & Compound_Attribute_is_polymorphic)
        {
            printf("type_reference: ");
            print_type_reference(field->type_reference, stdout);
        }
        else
        {
            printf("type: ");
            print_type_reference_for_type(field->type, stdout);
        }

        putchar('\n');

        print_indentation();
        printf("name: ");
        print_string_by_handle(field->name, stdout);
        putchar('\n');
        indentation--;
    }

    indentation--;
}

static void print_enumeration_declaration(const Enumeration_Type* enumeration)
{
    print_indentation();
    printf("enumeration\n");
    indentation++;

    print_indentation();
    printf("name: ");
    print_string_by_handle(enumeration->name, stdout);
    putchar('\n');

    print_indentation();
    printf("underlying_type: ");
    print_type_reference_for_type(enumeration->underlying_type, stdout);
    putchar('\n');

    print_indentation();
    printf(enumeration->is_public ? "is_public: true\n" : "is_public: false\n");

    for (uint8_t i = 0; i < enumeration->constants.handles_length; i++)
    {
        Constant* constant = lookup_constant(enumeration->constants.handles[i]);
        
        print_indentation();
        printf("constant\n");
        indentation++;
        
        print_indentation();
        printf("name: ");
        print_string_by_handle(constant->name, stdout);
        putchar('\n');

        print_indentation();
        printf("value: ");

        if (compare_types(enumeration->underlying_type, i8_handle))
            printf("%" PRId8 "\n", constant->value.i8);
        else if (compare_types(enumeration->underlying_type, i16_handle))
            printf("%" PRId16 "\n", constant->value.i16);
        else if (compare_types(enumeration->underlying_type, i32_handle))
            printf("%" PRId32 "\n", constant->value.i32);
        else if (compare_types(enumeration->underlying_type, i64_handle))
            printf("%" PRId64 "\n", constant->value.i64);
        else if (compare_types(enumeration->underlying_type, u8_handle))
            printf("%" PRIu8 "\n", constant->value.u8);
        else if (compare_types(enumeration->underlying_type, u16_handle))
            printf("%" PRIu16 "\n", constant->value.u16);
        else if (compare_types(enumeration->underlying_type, u32_handle))
            printf("%" PRIu32 "\n", constant->value.u32);
        else if (compare_types(enumeration->underlying_type, u64_handle))
            printf("%" PRIu64 "\n", constant->value.u64);
        else
            printf("?%%\n");

        indentation--;
    }

    indentation--;
}

static void print_binary(Type_Handle type, const Binary* binary, bool is_not_polymorphic)
{
    print_indentation();
    printf("binary\n");
    
    indentation++;
    if (is_not_polymorphic)
    {
        print_indentation();
        printf("type: ");
        print_type_reference_for_type(type, stdout);
        putchar('\n');
    }
    
    print_indentation();
    printf("operator: ");
    print_operator(binary->operator, stdout);
    putchar('\n');

    print_expression(lookup_expression(binary->lhs), is_not_polymorphic);
    print_expression(lookup_expression(binary->rhs), is_not_polymorphic);
    
    indentation--;
}

static void print_reference_expression(const Reference* reference, bool is_not_polymorphic)
{
    print_indentation();
    printf("reference_expression\n");
    
    indentation++;
    if (is_not_polymorphic)
    {
        print_indentation();
        printf("type: ");
        print_type_reference_for_type(reference->type, stdout);
        putchar('\n');
    }

    print_indentation();
    printf("name: ");
    print_string_by_handle(reference->name, stdout);
    putchar('\n');

    for (uint8_t i = 0; i < reference->actual_type_parameters.handles_length; i++)
    {
        print_indentation();
        printf("actual_type_parameter: ");
        print_type_reference(reference->actual_type_parameters.handles[i], stdout);
        putchar('\n');
    }

    indentation--;
}

static void print_integer_literal(const Expression* value, bool is_not_polymorphic)
{
    print_indentation();
    printf("integer_literal\n");

    indentation++;
    if (is_not_polymorphic)
    {
        print_indentation();
        printf("type: ");
        print_type_reference_for_type(value->type, stdout);
        putchar('\n');
    }
    
    print_indentation();
    printf("value: ");

    if (is_invalid_type_handle(value->type))
    {
        print_string(value->unchecked_number.string, stdout);
        putchar('\n');
    }
    else if (compare_types(value->type, i8_handle))
        printf("%" PRId8 "\n", value->number.value.i8);
    else if (compare_types(value->type, i16_handle))
        printf("%" PRId16 "\n", value->number.value.i16);
    else if (compare_types(value->type, i32_handle))
        printf("%" PRId32 "\n", value->number.value.i32);
    else if (compare_types(value->type, i64_handle))
        printf("%" PRId64 "\n", value->number.value.i64);
    else if (compare_types(value->type, u8_handle))
        printf("%" PRIu8 "\n", value->number.value.u8);
    else if (compare_types(value->type, u16_handle))
        printf("%" PRIu16 "\n", value->number.value.u16);
    else if (compare_types(value->type, u32_handle))
        printf("%" PRIu32 "\n", value->number.value.u32);
    else if (compare_types(value->type, u64_handle))
        printf("%" PRIu64 "\n", value->number.value.u64);
    else
        printf("?\n");

    indentation--;
}

static void print_float_literal(const Expression* value, bool is_not_polymorphic)
{
    print_indentation();
    printf("float_literal\n");

    indentation++;
    if (is_not_polymorphic)
    {
        print_indentation();
        printf("type: ");
        print_type_reference_for_type(value->type, stdout);
        putchar('\n');
    }

    print_indentation();
    printf("value: ");

    if (is_invalid_type_handle(value->type))
    {
        print_string(value->unchecked_number.string, stdout);
        putchar('\n');
    }
    else if (compare_types(value->type, f32_handle))
        printf("%g\n", value->number.value.f32);
    else if (compare_types(value->type, f64_handle))
        printf("%g\n", value->number.value.f64);
    else
        printf("?\n");
    
    indentation--;
}

static void print_boolean_literal(const Expression* value, bool is_not_polymorphic)
{
    print_indentation();
    printf("boolean_literal\n");

    indentation++;
    if (is_not_polymorphic)
    {
        print_indentation();
        printf("type: ");
        print_type_reference_for_type(value->type, stdout);
        putchar('\n');
    }
    
    print_indentation();
    printf(value->tag == Expression_Tag_true ? "value: true\n" : "value: false\n"); 

    indentation--;
}

static void print_enumeration_constant_access(const Enumeration_Constant_Access* constant_access, bool is_not_polymorphic)
{
    print_indentation();
    printf("enumeration_constant_access\n");

    indentation++;
    if (is_not_polymorphic)
    {
        print_indentation();
        printf("type: ");
        print_type_reference_for_type(constant_access->type, stdout);
        putchar('\n');
    }

    print_indentation();
    printf("value: ");
    print_string_by_handle(constant_access->constant, stdout);
    putchar('\n');

    indentation--;
}

static void print_compound_literal(const Compound_Literal* compound_literal, bool is_not_polymorphic)
{
    print_indentation();
    printf("compound_literal\n");

    indentation++;
    if (is_not_polymorphic)
    {
        print_indentation();
        printf("type: ");
        print_type_reference_for_type(compound_literal->type, stdout);
        putchar('\n');
    }

    Type* type;
    if (is_not_polymorphic)
        type = lookup_type(compound_literal->type);

    for (uint8_t i = 0; i < compound_literal->field_initializers.handles_length; i++)
    {
        Field_Initializer* initializer = lookup_field_initializer(compound_literal->field_initializers.handles[i]);

        print_indentation();
        printf("field_initializer\n");

        indentation++;
        if (is_not_polymorphic)
        {
            Field* field = lookup_field_by_name(&type->compound, initializer->field, initializer->field_span);

            print_indentation();
            printf("type: ");
            print_type_reference_for_type(field->type, stdout);
            putchar('\n');
        }

        print_indentation();
        printf("name: ");
        print_string_by_handle(initializer->field, stdout);
        putchar('\n');

        print_expression(lookup_expression(initializer->expression), is_not_polymorphic);

        indentation--;
    }

    indentation--;
}

static void print_array_literal(const Array_Literal* array_literal, bool is_not_polymorphic)
{
    print_indentation();
    printf("array_literal\n");

    indentation++;
    if (is_not_polymorphic)
    {
        print_indentation();
        printf("type: ");
        print_type_reference_for_type(array_literal->type, stdout);
        putchar('\n');
    }

    for (uint8_t i = 0; i < array_literal->elements.handles_length; i++)
    {
        Element_Initializer* initializer = lookup_element_initializer(array_literal->elements.handles[i]);

        print_indentation();
        printf("element_initializer\n");

        indentation++;
        
        if (is_not_polymorphic)
        {
            print_indentation();
            printf("index: %u\n", initializer->index);
        }
        
        if (!is_invalid_expression_handle(initializer->explicit_index))
            print_expression(lookup_expression(initializer->explicit_index), is_not_polymorphic);

        print_expression(lookup_expression(initializer->expression), is_not_polymorphic);

        indentation--;

    }

    indentation--;
}

static void print_call_expression(const Call* call, bool is_not_polymorphic)
{
    print_indentation();
    printf("call_expression\n");

    indentation++;
    if (is_not_polymorphic)
    {
        print_indentation();
        printf("type: ");
        print_type_reference_for_type(call->type, stdout);
        putchar('\n');
    }

    print_expression(lookup_expression(call->callee), is_not_polymorphic);
    for (uint8_t i = 0; i < call->actual_parameters.handles_length; i++)
        print_expression(lookup_expression(call->actual_parameters.handles[i]), is_not_polymorphic);

    indentation--;
}

static void print_field_access(const Field_Access* field_access, bool is_not_polymorphic)
{
    print_indentation();
    printf("field_access\n");

    indentation++;
    if (is_not_polymorphic)
    {
        print_indentation();
        printf("type: ");
        print_type_reference_for_type(field_access->type, stdout);
        putchar('\n');
    }

    print_expression(lookup_expression(field_access->compound), is_not_polymorphic);

    print_indentation();
    printf("field: ");
    print_string_by_handle(field_access->field, stdout);
    putchar('\n');

    indentation--;
}

static void print_array_access(const Array_Access* array_access, bool is_not_polymorphic)
{
    print_indentation();
    printf("array_access\n");

    indentation++;
    if (is_not_polymorphic)
    {
        print_indentation();
        printf("type: ");
        print_type_reference_for_type(array_access->type, stdout);
        putchar('\n');
    }

    print_expression(lookup_expression(array_access->array), is_not_polymorphic);
    print_expression(lookup_expression(array_access->index), is_not_polymorphic);

    indentation--;
}

static void print_cast(const Cast* cast, bool is_not_polymorphic)
{
    print_indentation();
    printf("cast\n");

    indentation++;
    print_indentation();
    if (is_not_polymorphic)
    {
        printf("type: ");
        print_type_reference_for_type(cast->type, stdout);
        putchar('\n');
    }
    else
    {
        printf("type_reference: ");
        print_type_reference(cast->target, stdout);
        putchar('\n');
    }

    print_expression(lookup_expression(cast->source), is_not_polymorphic);

    indentation--;
}

static void print_utf8_string_literal(const Utf8_String_Literal* literal, bool is_not_polymorphic)
{
    print_indentation();
    printf("utf8_string_literal\n");

    indentation++;
    if (is_not_polymorphic)
    {
        print_indentation();
        printf("type: ");
        print_type_reference_for_type(literal->type, stdout);
        putchar('\n');
    }

    print_indentation();
    printf("value: \"");
    print_string_by_handle(literal->string, stdout);
    printf("\"\n");

    indentation--;
}

static void print_node_with_type(Type_Handle type, const char* node, bool is_not_polymorphic)
{
    print_indentation();
    printf("%s\n", node);

    if (is_not_polymorphic)
    {
        indentation++;

        print_indentation();
        printf("type: ");
        print_type_reference_for_type(type, stdout);
        putchar('\n');

        indentation--;
    } 
}

static void print_node_with_sub_expression(Type_Handle expression_type, const char* node, Expression_Handle sub_expression_handle, bool is_not_polymorphic)
{
    print_indentation();
    printf("%s\n", node);

    Expression* sub_expression = lookup_expression(sub_expression_handle);

    indentation++;
    if (is_not_polymorphic)
    {
        print_indentation();
        printf("type: ");
        print_type_reference_for_type(expression_type, stdout);
        putchar('\n');
    }

    print_expression(sub_expression, is_not_polymorphic);

    indentation--;
}

static void print_expression(const Expression* expression, bool is_not_polymorphic)
{
    if (expression->tag == Expression_Tag_binary)
        print_binary(expression->type, &expression->binary, is_not_polymorphic);
    else if (expression->tag == Expression_Tag_compound_literal)
        print_compound_literal(&expression->compound_literal, is_not_polymorphic);
    else if (expression->tag == Expression_Tag_array_literal)
        print_array_literal(&expression->array_literal, is_not_polymorphic);
    else if (expression->tag == Expression_Tag_reference)
        print_reference_expression(&expression->reference, is_not_polymorphic);
    else if (expression->tag == Expression_Tag_false || expression->tag == Expression_Tag_true)
        print_boolean_literal(expression, is_not_polymorphic);
    else if (expression->tag == Expression_Tag_integer || expression->tag == Expression_Tag_unicode_code_point)
        print_integer_literal(expression, is_not_polymorphic);
    else if (expression->tag == Expression_Tag_float)
        print_float_literal(expression, is_not_polymorphic);
    else if (expression->tag == Expression_Tag_enumeration_constant_access)
        print_enumeration_constant_access(&expression->enumeration_constant_access, is_not_polymorphic);
    else if (expression->tag == Expression_Tag_call)
        print_call_expression(&expression->call, is_not_polymorphic);
    else if (expression->tag == Expression_Tag_field_access ||
             expression->tag == Expression_Tag_fixed_array_length ||
             expression->tag == Expression_Tag_dynamic_array_length ||
             expression->tag == Expression_Tag_dynamic_array_capacity ||
             expression->tag == Expression_Tag_dynamic_array_elements)
        print_field_access(&expression->field_access, is_not_polymorphic);
    else if (expression->tag == Expression_Tag_array_access)
        print_array_access(&expression->array_access, is_not_polymorphic);
    else if (expression->tag == Expression_Tag_parenthesized)
        print_node_with_sub_expression(expression->type, "parenthesized", expression->parenthesized.expression, is_not_polymorphic);
    else if (expression->tag == Expression_Tag_cast)
        print_cast(&expression->cast, is_not_polymorphic);
    else if (expression->tag == Expression_Tag_address_of)
        print_node_with_sub_expression(expression->type, "address_of", expression->address_of.expression, is_not_polymorphic);
    else if (expression->tag == Expression_Tag_dereference)
        print_node_with_sub_expression(expression->type, "dereference", expression->dereference.expression, is_not_polymorphic);
    else if (expression->tag == Expression_Tag_not)
        print_node_with_sub_expression(expression->type, "not", expression->not.expression, is_not_polymorphic);
    else if (expression->tag == Expression_Tag_uninitialized_literal)
        print_node_with_type(expression->type, "uninitialized_literal", is_not_polymorphic);
    else if (expression->tag == Expression_Tag_negate)
        print_node_with_sub_expression(expression->type, "negate", expression->negate.expression, is_not_polymorphic);
    else if (expression->tag == Expression_Tag_blank_identifier)
        print_node_with_type(expression->type, "blank_identifier", is_not_polymorphic);
    else if (expression->tag == Expression_Tag_null)
        print_node_with_type(expression->type, "null_literal", is_not_polymorphic);
    else if (expression->tag == Expression_Tag_utf8_string_literal)
        print_utf8_string_literal(&expression->string_literal, is_not_polymorphic);
    else
        unexpected_expression(__FILE__, __LINE__, expression->tag);
}

static void print_expressions(const Expression_Handle_Array* array, bool is_not_polymorphic)
{
    for (uint8_t i = 0; i < array->handles_length; i++)
        print_expression(lookup_expression(array->handles[i]), is_not_polymorphic);
}

static void print_declaration_statement(const Declaration_Statement* declaration_statement, bool is_not_polymorphic)
{
    printf("declaration_statement\n");
   
    indentation++;
    for (uint8_t i = 0; i < declaration_statement->variables.handles_length; i++)
    {
        Variable*  variable = lookup_variable(declaration_statement->variables.handles[i]);

        print_indentation();
        printf("variable\n");

        indentation++;
        print_indentation();
        
        if (is_not_polymorphic)
        {
            printf("type: ");
            print_type_reference_for_type(variable->type, stdout);
        }
        else
        {
            printf("type_reference: ");
            print_type_reference(variable->type_reference, stdout);
        }

        putchar('\n');
        
        print_indentation();
        printf("name: ");
        if (variable->is_blank)
            printf("_");
        else
            print_string_by_handle(variable->name, stdout);
        
        putchar('\n');

        indentation--;
    }

    print_expressions(&declaration_statement->expressions, is_not_polymorphic);
    indentation--;
}

static void print_assignment_statement(const Assignment_Statement* assignment_statement, bool is_not_polymorphic)
{
    printf("assignment_statement\n");

    indentation++;
    print_indentation();
    printf("operator: ");

    if (assignment_statement->operator == Token_Tag_equal)
        fprintf(stdout, "=\n");
    else if (assignment_statement->operator == Token_Tag_plus_equal)
        fprintf(stdout, "+=\n");
    else if (assignment_statement->operator == Token_Tag_minus_equal)
        fprintf(stdout, "-=\n");
    else if (assignment_statement->operator == Token_Tag_bitwise_or_equal)
        fprintf(stdout, "|=\n");
    else if (assignment_statement->operator == Token_Tag_bitwise_xor_equal)
        fprintf(stdout, "^=\n");
    else if (assignment_statement->operator == Token_Tag_multiply_equal)
        fprintf(stdout, "*=\n");
    else if (assignment_statement->operator == Token_Tag_divide_equal)
        fprintf(stdout, "/=\n");
    else if (assignment_statement->operator == Token_Tag_modulo_equal)
        fprintf(stdout, "%%=\n");
    else if (assignment_statement->operator == Token_Tag_bitwise_and_equal)
        fprintf(stdout, "&=\n");
    else if (assignment_statement->operator == Token_Tag_left_shift_equal)
        fprintf(stdout, "<<=\n");
    else if (assignment_statement->operator == Token_Tag_right_shift_equal)
        fprintf(stdout, ">>=\n");
    else
        unexpected_token(__FILE__, __LINE__, assignment_statement->operator);

    print_expressions(&assignment_statement->lhs, is_not_polymorphic);
    print_expressions(&assignment_statement->rhs, is_not_polymorphic);
    indentation--;
}

static void print_if_statement(const If_Statement* if_statement, bool is_not_polymorphic)
{
    printf("if_statement\n");
    indentation++;

    for (uint8_t i = 0; i < if_statement->branches.handles_length; i++)
    {
        print_indentation();
        printf("branch\n");
        indentation++;

        Branch* branch = lookup_branch(if_statement->branches.handles[i]);
        if (!is_invalid_statement_handle(branch->declaration_statement))
        {
            print_indentation();
            print_declaration_statement(&lookup_statement(branch->declaration_statement)->declaration_statement, is_not_polymorphic);
        }

        if (!is_invalid_expression_handle(branch->condition))
            print_expression(lookup_expression(branch->condition), is_not_polymorphic);

        indentation--;
        print_statements(&branch->body, is_not_polymorphic);
    }

    indentation--;
}

static void print_loop_statement(const Loop_Statement* loop_statement, bool is_not_polymorphic)
{
    printf("loop_statement\n");
    
    indentation++;
    if (!is_invalid_statement_handle(loop_statement->declaration_statement))
    {
        print_indentation();
        print_declaration_statement(&lookup_statement(loop_statement->declaration_statement)->declaration_statement, is_not_polymorphic);
    }

    if (!is_invalid_expression_handle(loop_statement->condition)) 
        print_expression(lookup_expression(loop_statement->condition), is_not_polymorphic);
    
    if (!is_invalid_statement_handle(loop_statement->assignment_statement))
    {
        print_indentation();
        print_assignment_statement(&lookup_statement(loop_statement->assignment_statement)->assignment_statement, is_not_polymorphic);
    }

    indentation--;
    print_statements(&loop_statement->body, is_not_polymorphic);
}

static void print_return_statement(const Return_Statement* return_statement, bool is_not_polymorphic)
{
    printf("return_statement\n");
    indentation++;
    print_expressions(&return_statement->values, is_not_polymorphic);
    indentation--;
}

static void print_statements(const Statement_Handle_Array* body, bool is_not_polymorphic)
{
    indentation++;
    for (uint16_t i = 0; i < body->handles_length; i++)
    {
        Statement* statement = lookup_statement(body->handles[i]);
        
        // All expressions do indentation on their own, but that indents to far
        // if the statement is an expression statement. So skip indentation 
        // here and let the print_expression function do its thing for 
        // expression statements.
        if (statement->tag != Statement_Tag_expression)
            print_indentation();
        
        if (statement->tag == Statement_Tag_declaration)
            print_declaration_statement(&statement->declaration_statement, is_not_polymorphic);
        else if (statement->tag == Statement_Tag_assignment)
            print_assignment_statement(&statement->assignment_statement, is_not_polymorphic);
        else if (statement->tag == Statement_Tag_expression)
            print_expression(lookup_expression(statement->expression_statement), is_not_polymorphic);
        else if (statement->tag == Statement_Tag_if)
            print_if_statement(&statement->if_statement, is_not_polymorphic);
        else if (statement->tag == Statement_Tag_loop)
            print_loop_statement(&statement->loop_statement, is_not_polymorphic);
        else if (statement->tag == Statement_Tag_break)
            printf("break_statement\n");
        else if (statement->tag == Statement_Tag_continue)
            printf("continue_statement\n");
        else if (statement->tag == Statement_Tag_return)
            print_return_statement(&statement->return_statement, is_not_polymorphic);
        else
            unexpected_statement(__FILE__, __LINE__, statement->tag);
    }

    indentation--;
}

static void print_namespace(const File* file)
{
    Namespace* namespace = lookup_namespace(file->namespace);

    print_indentation();
    printf("namespace: ");
    print_string_by_handle(namespace->name, stdout);
    putchar('\n');
}

static void print_uses(const File* file)
{
    if (!file->uses.handles)
        return;

    print_indentation();
    for (uint8_t i = 0; i < file->uses.handles_length; i++)
    {
        Namespace* namespace = lookup_namespace(file->uses.handles[i]);

        print_indentation();
        printf("use: ");
        print_string_by_handle(namespace->name, stdout);
        putchar('\n');
    }

    putchar('\n');
}

static void print_type_declarations(const File* file)
{
    for (uint16_t a = 0; a < file->type_declarations.handles_length; a++)
    {
        Type_Handle a_type_handle = file->type_declarations.handles[a];
        Type* a_type = lookup_type(a_type_handle);
        if (a_type->tag == Type_Tag_compound)
        {
            print_compound_declaration(&a_type->compound);
            if (a_type->compound.attributes & Compound_Attribute_is_polymorphic)
            {
                for (uint16_t b = 0; b < types_length; b++)
                {
                    Type* b_type = &types[b];
                    if (b_type->tag == Type_Tag_compound && compare_types(b_type->compound.template, a_type_handle))
                        print_compound_declaration(&b_type->compound);
                }
            }
        }
        else if (a_type->tag == Type_Tag_enumeration)
            print_enumeration_declaration(&a_type->enumeration);
    }
}

static void print_function(Function_Handle function_handle)
{
    Function* function = lookup_function(function_handle);

    print_indentation();
    printf("function\n");

    indentation++;
    print_indentation();
    printf("name: ");
    print_string_by_handle(function->name, stdout);
    putchar('\n');

    if (function->attributes & Function_Attribute_is_polymorphic)
    {
        for (uint8_t i = 0; i < function->formal_type_parameters.handles_length; i++)
        {
            print_indentation();
            printf("formal_type_parameter: ");
            print_type_reference(function->formal_type_parameters.handles[i], stdout);
            putchar('\n');
        }

        for (uint8_t i = 0; i < function->formal_parameters.handles_length; i++)
        {
            print_indentation();
            printf("formal_parameter\n");

            Formal_Parameter* formal_parameter = lookup_formal_parameter(function->formal_parameters.handles[i]);

            indentation++;
            print_indentation();
            printf("type_reference: ");
            print_type_reference(formal_parameter->type, stdout);
            putchar('\n');

            print_indentation();
            printf("name: ");
            print_string_by_handle(formal_parameter->name, stdout);
            putchar('\n');
            indentation--;
        }

        for (uint8_t i = 0; i < function->return_types.handles_length; i++)
        {
            print_indentation();
            printf("return_type_reference: ");
            print_type_reference(function->return_types.handles[i], stdout);
            putchar('\n');
        }
    }
    else
    {
        //assert(!is_invalid_function_handle(function->template));
        for (uint8_t i = 0; i < function->actual_type_parameters.handles_length; i++)
        {
            print_indentation();
            printf("actual_type_parameter: ");
            print_type_reference_for_type(function->actual_type_parameters.handles[i], stdout);
            putchar('\n');
        }

        Function_Type* function_type = &lookup_type(function->signature)->function;
        for (uint8_t i = 0; i < function_type->formal_parameters.handles_length; i++)
        {
            print_indentation();
            printf("formal_parameter\n");

            indentation++;
            print_indentation();
            printf("type: ");
            print_type_reference_for_type(function_type->formal_parameters.handles[i], stdout);
            putchar('\n');

            print_indentation();
            printf("name: ");
            print_string_by_handle(lookup_formal_parameter(function->formal_parameters.handles[i])->name, stdout);
            putchar('\n');
            indentation--;
        }

        for (uint8_t i = 0; i < function_type->return_types.handles_length; i++)
        {
            print_indentation();
            printf("return_type: ");
            print_type_reference_for_type(function_type->return_types.handles[i], stdout);
            putchar('\n');
        }
    }

    indentation--;
    print_statements(&function->body, !(function->attributes & Function_Attribute_is_polymorphic));

    if (function->attributes & Function_Attribute_is_polymorphic)
    {
        for (uint16_t i = 0; i < functions_length; i++)
        {
            Function* f = &functions[i];
            if (compare_functions(f->template, function_handle))
                print_function((Function_Handle) { .index = i});
        }
    }
}

static void print_functions(const File* file)
{
    for (uint16_t i = 0; i < file->function_declarations.handles_length; i++)
        print_function(file->function_declarations.handles[i]);
}

void print_semantics(void)
{
    for (uint8_t i = 0; i < files_length; i++)
    {
        File* file = &files[i];
        
        printf("file\n");
        indentation++;

        print_indentation();
        printf("path: %s\n", file->path);
        
        print_namespace(file);
        print_uses(file);
        print_type_declarations(file);
        print_functions(file);

        indentation--;
    }
}

const char* expression_tag_as_string(uint8_t tag)
{
    switch (tag)
    {
        case Expression_Tag_reference:                   return "Expression_Tag_reference";
        case Expression_Tag_integer:                     return "Expression_Tag_integer";
        case Expression_Tag_float:                       return "Expression_Tag_float";
        case Expression_Tag_true:                        return "Expression_Tag_true";
        case Expression_Tag_false:                       return "Expression_Tag_false";
        case Expression_Tag_unicode_code_point:          return "Expression_Tag_unicode_code_point";
        case Expression_Tag_call:                        return "Expression_Tag_call";
        case Expression_Tag_binary:                      return "Expression_Tag_binary";
        case Expression_Tag_field_access:                return "Expression_Tag_field_access";
        case Expression_Tag_enumeration_constant_access: return "Expression_Tag_enumeration_constant_access";
        case Expression_Tag_array_access:                return "Expression_Tag_array_access";
        case Expression_Tag_array_literal:               return "Expression_Tag_array_literal";
        case Expression_Tag_parenthesized:               return "Expression_Tag_parenthesized";
        case Expression_Tag_compound_literal:            return "Expression_Tag_compound_literal";
        case Expression_Tag_fixed_array_length:          return "Expression_Tag_fixed_array_length";
        case Expression_Tag_dynamic_array_length:        return "Expression_Tag_dynamic_array_length";
        case Expression_Tag_dynamic_array_capacity:      return "Expression_Tag_dynamic_array_capacity";
        case Expression_Tag_dynamic_array_elements:      return "Expression_Tag_dynamic_array_elements";
        case Expression_Tag_cast:                        return "Expression_Tag_cast";
        case Expression_Tag_address_of:                  return "Expression_Tag_address_of";
        case Expression_Tag_dereference:                 return "Expression_Tag_dereference";
        case Expression_Tag_not:                         return "Expression_Tag_not";
        case Expression_Tag_uninitialized_literal:       return "Expression_Tag_uninitialized_literal";
        case Expression_Tag_negate:                      return "Expression_Tag_negate";
        case Expression_Tag_blank_identifier:            return "Expression_Tag_blank_identifier";
        case Expression_Tag_null:                        return "Expression_Tag_null";
        case Expression_Tag_sizeof:                      return "Expression_Tag_sizeof";
        default:                                         return "Invalid Expression_Tag";
    }
}

const char* token_tag_as_string(uint8_t tag)
{
    switch (tag)
    {
        case Token_Tag_integer:               return "Token_Tag_integer";
        case Token_Tag_float:                 return "Token_Tag_float";
        case Token_Tag_upper_case_identifier: return "Token_Tag_upper_case_identifier";
        case Token_Tag_lower_case_identifier: return "Token_Tag_lower_case_identifier";
        case Token_Tag_namespace:             return "Token_Tag_namespace";
        case Token_Tag_use:                   return "Token_Tag_use";
        case Token_Tag_public:                return "Token_Tag_public";
        case Token_Tag_external:              return "Token_Tag_external";
        case Token_Tag_function:              return "Token_Tag_function";
        case Token_Tag_end:                   return "Token_Tag_end";
        case Token_Tag_if:                    return "Token_Tag_if";
        case Token_Tag_elif:                  return "Token_Tag_elif";
        case Token_Tag_else:                  return "Token_Tag_else";
        case Token_Tag_for:                   return "Token_Tag_for";
        case Token_Tag_while:                 return "Token_Tag_while";
        case Token_Tag_break:                 return "Token_Tag_break";
        case Token_Tag_continue:              return "Token_Tag_continue";
        case Token_Tag_structure:             return "Token_Tag_structure";
        case Token_Tag_enumeration:           return "Token_Tag_enumeration";
        case Token_Tag_sizeof:                return "Token_Tag_sizeof";
        case Token_Tag_union:                 return "Token_Tag_union";
        case Token_Tag_return:                return "Token_Tag_return";
        case Token_Tag_true:                  return "Token_Tag_true";
        case Token_Tag_false:                 return "Token_Tag_false";
        case Token_Tag_null:                  return "Token_Tag_null";
        case Token_Tag_version:               return "Token_Tag_version";
        case Token_Tag_unicode_code_point:    return "Token_Tag_unicode_code_point";
        case Token_Tag_utf8_string:           return "Token_Tag_utf8_string";
        case Token_Tag_logical_or:            return "Token_Tag_logical_or";
        case Token_Tag_logical_and:           return "Token_Tag_logical_and";
        case Token_Tag_plus_equal:            return "Token_Tag_plus_equal";
        case Token_Tag_minus_equal:           return "Token_Tag_minus_equal";
        case Token_Tag_multiply_equal:        return "Token_Tag_multiply_equal";
        case Token_Tag_divide_equal:          return "Token_Tag_divide_equal";
        case Token_Tag_bitwise_and_equal:     return "Token_Tag_bitwise_and_equal";
        case Token_Tag_bitwise_or_equal:      return "Token_Tag_bitwise_or_equal";
        case Token_Tag_bitwise_xor_equal:     return "Token_Tag_bitwise_xor_equal";
        case Token_Tag_modulo_equal:          return "Token_Tag_modulo_equal";
        case Token_Tag_left_shift_equal:      return "Token_Tag_left_shift_equal";
        case Token_Tag_right_shift_equal:     return "Token_Tag_right_shift_equal";
        case Token_Tag_equal:                 return "Token_Tag_equal";
        case Token_Tag_equal_equal:           return "Token_Tag_equal_equal";
        case Token_Tag_not_equal:             return "Token_Tag_not_equal";
        case Token_Tag_less_than_or_equal:    return "Token_Tag_less_than_or_equal";
        case Token_Tag_greater_than_or_equal: return "Token_Tag_greater_than_or_equal";
        case Token_Tag_less_than:             return "Token_Tag_less_than";
        case Token_Tag_greater_than:          return "Token_Tag_greater_than";
        case Token_Tag_plus:                  return "Token_Tag_plus";
        case Token_Tag_minus:                 return "Token_Tag_minus";
        case Token_Tag_bitwise_or:            return "Token_Tag_bitwise_or";
        case Token_Tag_bitwise_xor:           return "Token_Tag_bitwise_xor";
        case Token_Tag_multiply:              return "Token_Tag_multiply";
        case Token_Tag_divide:                return "Token_Tag_divide";
        case Token_Tag_modulo:                return "Token_Tag_modulo";
        case Token_Tag_bitwise_and:           return "Token_Tag_bitwise_and";
        case Token_Tag_left_shift:            return "Token_Tag_left_shift";
        case Token_Tag_right_shift:           return "Token_Tag_right_shift";
        case Token_Tag_exclamation_mark:      return "Token_Tag_exclamation_mark";
        case Token_Tag_pound_sign:            return "Token_Tag_pound_sign";
        case Token_Tag_dereference:           return "Token_Tag_dereference";
        case Token_Tag_dot:                   return "Token_Tag_dot";
        case Token_Tag_left_parentheses:      return "Token_Tag_left_parentheses";
        case Token_Tag_right_parentheses:     return "Token_Tag_right_parentheses";
        case Token_Tag_left_square_bracket:   return "Token_Tag_left_square_bracket";
        case Token_Tag_right_square_bracket:  return "Token_Tag_right_square_bracket";
        case Token_Tag_left_curly_bracket:    return "Token_Tag_left_curly_bracket";
        case Token_Tag_right_curly_bracket:   return "Token_Tag_right_curly_bracket";
        case Token_Tag_comma:                 return "Token_Tag_comma";
        case Token_Tag_semicolon:             return "Token_Tag_semicolon";
        case Token_Tag_colon:                 return "Token_Tag_colon";
        case Token_Tag_backtick:              return "Token_Tag_backtick";
        case Token_Tag_question_mark:         return "Token_Tag_question_mark";
        case Token_Tag_dollar_sign:           return "Token_Tag_dollar_sign";
        case Token_Tag_uninitialized_literal: return "Token_Tag_uninitialized_literal";
        case Token_Tag_blank_identifier:      return "Token_Tag_blank_identifier";
        case Token_Tag_eof:                   return "Token_Tag_eof";
        default:                              return "Invalid Token_Tag";
    }
}

const char* statement_tag_as_string(uint8_t tag)
{
    switch (tag)
    {
        case Statement_Tag_declaration: return "Statement_Tag_declaration";
        case Statement_Tag_assignment:  return "Statement_Tag_assignment";
        case Statement_Tag_expression:  return "Statement_Tag_expression";
        case Statement_Tag_if:          return "Statement_Tag_if";
        case Statement_Tag_loop:        return "Statement_Tag_loop";
        case Statement_Tag_break:       return "Statement_Tag_break";
        case Statement_Tag_continue:    return "Statement_Tag_continue";
        case Statement_Tag_return:      return "Statement_Tag_return";
        default:                        return "Invalid Statement_Tag";
    }
}

const char* type_reference_tag_as_string(uint8_t tag)
{
    switch (tag)
    {
        case Type_Reference_Tag_name:                 return "Type_Reference_Tag_name";
        case Type_Reference_Tag_pointer:              return "Type_Reference_Tag_pointer";
        case Type_Reference_Tag_dynamic_array:        return "Type_Reference_Tag_dynamic_array";
        case Type_Reference_Tag_fixed_array:          return "Type_Reference_Tag_fixed_array";
        case Type_Reference_Tag_function:             return "Type_Reference_Tag_function";
        case Type_Reference_Tag_polymorphic_compound: return "Type_Reference_Tag_polymorphic_compound";
        default:                                      return "Invalid Type_Reference_Tag";
    }
}

const char* type_tag_as_string(uint8_t tag)
{
    switch (tag)
    {
        case Type_Tag_i8:            return "Type_Tag_i8";
        case Type_Tag_i16:           return "Type_Tag_i16";
        case Type_Tag_i32:           return "Type_Tag_i32";
        case Type_Tag_i64:           return "Type_Tag_i64";
        case Type_Tag_u8:            return "Type_Tag_u8";
        case Type_Tag_u16:           return "Type_Tag_u16";
        case Type_Tag_u32:           return "Type_Tag_u32";
        case Type_Tag_u64:           return "Type_Tag_u64";
        case Type_Tag_f32:           return "Type_Tag_f32";
        case Type_Tag_f64:           return "Type_Tag_f64";
        case Type_Tag_bool:          return "Type_Tag_bool";
        case Type_Tag_enumeration:   return "Type_Tag_enumeration";
        case Type_Tag_compound:      return "Type_Tag_compound";
        case Type_Tag_function:      return "Type_Tag_function";
        case Type_Tag_pointer:       return "Type_Tag_pointer";
        case Type_Tag_dynamic_array: return "Type_Tag_dynamic_array";
        case Type_Tag_fixed_array:   return "Type_Tag_fixed_array";
        case Type_Tag_none:          return "Type_Tag_none";
        case Type_Tag_tuple:         return "Type_Tag_tuple";
        default:                     return "Invalid Type_Tag";
    }
}

const char* instruction_tag_as_string(uint8_t tag)
{
    switch (tag)
    {
        case Ir_Instruction_Tag_return: return "Ir_Instruction_Tag_return";
        default:                        return "Invalid Ir_Instruction_Tag";
    }
}

// Error formats:
// An error that is not tied to any file.
 
// path_to_file.owen:123:24: This is the message. 

// path_to_file.owen:123:24: Somthing about declarations. 
// Declarations:
//     path/to_file.owen:12:20: Direction
//     path/to_file.owen:16:20: Direction[B]

// path_to_file.owen:123:24: Some error that happened burried in a polymorphic context.
// Polymorphic stack:
//     path/to_file.owen:12:20: do_stuff[Direction](Direction a)
//     path/to_file.owen:16:20: do_stuff[Direction](Direction a, Direction b)

static void print_position_error_by_va_list(const File* file, Position position, const char* message, va_list arguments);

static void print_error_line_by_va_list(const char* message, va_list arguments)
{
    while (*message)
    {
        if (*message == '%')
        {
            char format = *++message;
            if (format == 'u')
                fprintf(stderr, "%u", va_arg(arguments, uint32_t));
            else if (format == 'U')
            {
                uint32_t code_point = va_arg(arguments, uint32_t);
                fprintf(stderr, code_point > UINT16_MAX ? "U+%08X" : "U+%04X", code_point);
            }
            else if (format == 'l' && message[1] == 'l' && message[2] == 'u')
            {
                message += 2;
                fprintf(stderr, "%llu", va_arg(arguments, uint64_t));
            }
            else if (format == 'h' && message[1] == 'h' && message[2] == 'i')
            {
                message += 2;
                fprintf(stderr, "%hhi", va_arg(arguments, int8_t));
            }
            else if (format == 'h' && message[1] == 'i')
            {
                message++;
                fprintf(stderr, "%hi", va_arg(arguments, int16_t));
            }
            else if (format == 'i')
                fprintf(stderr, "%i", va_arg(arguments, int32_t));
            else if (format == 'l' && message[1] == 'l' && message[2] == 'i')
            {
                message += 2;
                fprintf(stderr, "%lli", va_arg(arguments, int64_t));
            }
            else if (format == 's')
                fprintf(stderr, "%s", va_arg(arguments, char*));
            else if (format == 'I')
                print_string_by_handle(va_arg(arguments, Interned_String_Handle), stderr);
            else if (format == 'S')
                print_string(va_arg(arguments, String), stderr);
            else if (format == 't')
                print_type_reference_for_type(va_arg(arguments, Type_Handle), stderr);
            else if (format == 'c')
                print_type_reference_for_compound(va_arg(arguments, Compound_Type*), stderr);
            else if (format == 'o')
                print_operator(va_arg(arguments, uint8_t), stderr);
            else
            {
                fputc('%', stderr);
                fputc(format, stderr);
            }
        }
        else
            fputc(*message, stderr);

        message++;
    }
    
    fputc('\n', stderr);
}

static void print_polymorphic_stack(void)
{
    if (!polymorphic_type_mappings_length)
        return;
    
    for (uint8_t a = 0; a < polymorphic_type_mappings_length; a++)
    {
        Polymorphic_Type_Mapping* map = &polymorphic_type_mappings[a];
        if (!a)
            fprintf(stderr, "Polymorphic stack:");

        assert(map->origin.start.line && map->origin.start.column);
        fprintf(stderr, "\n    %s:%u:%u: ", map->file->path, map->origin.start.line, map->origin.start.column);
        
        if (map->monomorphisized_function)
        {
            print_string_by_handle(map->monomorphisized_function->name, stderr);
            fputc('[', stderr);

            print_type_reference_for_type(map->monomorphic_types[0], stderr);
            for (uint8_t b = 1; b < map->length; b++)
            {
                fprintf(stderr, ", ");
                print_type_reference_for_type(map->monomorphic_types[b], stderr);
            }

            fprintf(stderr, "](");
            Function_Type* signature = &lookup_type(map->monomorphisized_function->signature)->function;
            if (signature->formal_parameters.handles_length)
            {
                print_type_reference_for_type(signature->formal_parameters.handles[0], stderr);
                fputc(' ', stderr);
                print_string_by_handle(lookup_formal_parameter(map->monomorphisized_function->formal_parameters.handles[0])->name, stderr);

                for (uint8_t b = 1; b < signature->formal_parameters.handles_length; b++)
                {
                    fprintf(stderr, ", ");
                    print_type_reference_for_type(signature->formal_parameters.handles[b], stderr);
                    fputc(' ', stderr);
                    print_string_by_handle(lookup_formal_parameter(map->monomorphisized_function->formal_parameters.handles[b])->name, stderr);
                }
            }

            fputc(')', stderr);
            if (signature->return_types.handles_length)
            {
                fprintf(stderr, " : ");
                print_type_reference_for_type(signature->return_types.handles[0], stderr);

                for (uint8_t b = 1; b < signature->return_types.handles_length; b++)
                {
                    fprintf(stderr, ", ");
                    print_type_reference_for_type(signature->return_types.handles[b], stderr);
                }
            }
        }
        else if (map->monomorphisized_compound)
        {
            print_string_by_handle(map->monomorphisized_compound->name, stderr);
            fputc('[', stderr);

            print_type_reference_for_type(map->monomorphic_types[0], stderr);
            for (uint8_t b = 1; b < map->length; b++)
            {
                fprintf(stderr, ", ");
                print_type_reference_for_type(map->monomorphic_types[b], stderr);
            }

            fputc(']', stderr);
        }
        else
            ice(__FILE__, __LINE__, "No monomorphisized_function or monomorphisized_compound set on the Polymorphic_Type_Mapping.");
    }

    fprintf(stderr, "\n");
}

static void print_file_position(const File* file, Position position)
{
    fprintf(stderr, "%s:%u:%u", file->path, position.line, position.column);
}

// Spits out an error with no file:line:column going on.
void print_error(const char* message, ...)
{
    va_list arguments;
    va_start(arguments, message);
    
    print_error_line_by_va_list(message, arguments);
    
    exit_with_error_code();
}

void print_span_error(const File* file, Span span, const char* message, ...)
{
    va_list arguments;
    va_start(arguments, message);
    print_position_error_by_va_list(file, span.start, message, arguments);

    exit_with_error_code();
}

// "Primitives" for errors like multiple definitions in scope.
void print_declaration_header(const File* file, Position position, const char* message, ...)
{
    print_file_position(file, position);
    fprintf(stderr, ": ");

    va_list arguments;
    va_start(arguments, message);

    print_error_line_by_va_list(message, arguments);
    fprintf(stderr, "Declarations:\n");

    va_end(arguments);
}

void print_function_declaration(const Function* function)
{
    fprintf(stderr, "    ");
    print_file_position(lookup_file(function->file), function->name_span.start);
    fprintf(stderr, ": ");

    print_string_by_handle(function->name, stderr);
    if (function->attributes & Function_Attribute_is_polymorphic)
    {
        fprintf(stderr, "[");

        print_type_reference(function->formal_type_parameters.handles[0], stderr);
        for (uint8_t i = 1; i < function->formal_type_parameters.handles_length; i++)
        {
            fprintf(stderr, ", ");
            print_type_reference(function->formal_type_parameters.handles[i], stderr);
        }

        fprintf(stderr, "]");
    }

    fprintf(stderr, "(");
    if (function->formal_parameters.handles_length)
    {
        Formal_Parameter* formal_parameter = lookup_formal_parameter(function->formal_parameters.handles[0]);
        print_type_reference(formal_parameter->type, stderr);
        fprintf(stderr, " ");
        print_string_by_handle(formal_parameter->name, stderr);

        for (uint8_t i = 1; i < function->formal_parameters.handles_length; i++)
        {
            fprintf(stderr, ", ");
            formal_parameter = lookup_formal_parameter(function->formal_parameters.handles[i]);
            print_type_reference(formal_parameter->type, stderr);
            fprintf(stderr, " ");
            print_string_by_handle(formal_parameter->name, stderr);
        }
    }

    fprintf(stderr, ")");
    if (function->return_types.handles_length)
    {
        fprintf(stderr, " : ");
        print_type_reference(function->return_types.handles[0], stderr);
        for (uint8_t i = 1; i < function->return_types.handles_length; i++)
        {
            fprintf(stderr, ", ");
            print_type_reference(function->return_types.handles[i], stderr);
        }
    }

    fprintf(stderr, "\n");
}

void print_type_declaration(Type_Handle type_handle)
{
    Type* type = lookup_type(type_handle);

    fprintf(stderr, "    ");
    if (type->tag == Type_Tag_compound)
        print_file_position(lookup_file(type->compound.file), type->compound.name_span.start);
    else if (type->tag == Type_Tag_enumeration)
        print_file_position(lookup_file(type->enumeration.file), type->enumeration.name_span.start);
    else
        fprintf(stderr, "Primitive");

    fprintf(stderr, ": ");
    print_type_reference_for_type(type_handle, stderr);
    fprintf(stderr, "\n");
}

// Convenience functions for printing errors about a specific token, type reference, expression etc.
// with a file:line:column prefixing it.
static void print_position_error_by_va_list(const File* file, Position position, const char* message, va_list arguments)
{
    print_file_position(file, position);
    fprintf(stderr, ": ");
 
    print_error_line_by_va_list(message, arguments);
    print_polymorphic_stack();
    
    exit_with_error_code();
}

void not_implemented(const char* path, uint32_t line, const char* thing)
{
    print_error("%s:%u: %s not implemented.", path, line, thing);
}

void out_of_memory(const char* path, uint32_t line, const char* type)
{
    print_error("%s:%u: Out of %s memory.", path, line, type);
}

void ice(const char* path, uint32_t line, const char* message)
{
    print_error("%s:%u: ICE: %s", path, line, message);
}

void unexpected_expression(const char* path, uint32_t line, uint8_t tag)
{
    print_error("%s:%u: ICE: Unexpected %s", path, line, expression_tag_as_string(tag));
}

void unexpected_reference_type(const char* path, uint32_t line, uint8_t tag)
{
    print_error("%s:%u: ICE: Unexpected %s", path, line, type_reference_tag_as_string(tag));
}

void unexpected_type(const char* path, uint32_t line, uint8_t tag)
{
    print_error("%s:%u: ICE: Unexpected %s", path, line, type_tag_as_string(tag));
}

void unexpected_statement(const char* path, uint32_t line, uint8_t tag)
{
    print_error("%s:%u: ICE: Unexpected %s", path, line, statement_tag_as_string(tag));
}

void unexpected_token(const char* path, uint32_t line, uint8_t tag)
{
    print_error("%s:%u: ICE: Unexpected %s", path, line, token_tag_as_string(tag));
}

void unexpected_instruction(const char* path, uint32_t line, uint8_t tag)
{
    print_error("%s:%u: ICE: Unexpected %s", path, line, instruction_tag_as_string(tag));
}
