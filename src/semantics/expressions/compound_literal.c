#include "expression.h"

static void each_field_is_initialized_at_most_once(const File* file, const Compound_Literal* literal)
{
    for (Array_Size a = 0; a < literal->field_initializers.handles_length; a++)
    {
        Field_Initializer* a_initializer = lookup_field_initializer(field_initializer_at(&literal->field_initializers, a));
        for (Array_Size b = a + 1; b < literal->field_initializers.handles_length; b++)
        {
            Field_Initializer* b_initializer = lookup_field_initializer(field_initializer_at(&literal->field_initializers, b));
            if (compare_interned_strings(a_initializer->field, b_initializer->field))
                print_span_error(file, b_initializer->field_span, "%I is initialized multiple times.", b_initializer->field);
        }
    }
}

static void type_check_field_initializers(const File* file, const Compound_Type* type, const Compound_Literal* compound_literal, Expression_Check_Flags flags)
{
    for (Array_Size i = 0; i < compound_literal->field_initializers.handles_length; i++)
    {
        Field_Initializer* initializer = lookup_field_initializer(field_initializer_at(&compound_literal->field_initializers, i));
        Field* field = lookup_field_by_name(type, initializer->field, initializer->field_span);
        Expression* expression = lookup_expression(initializer->expression);

        type_check_expression(file, expression, field->type, flags);
        if (!expression_types_match(field->type, expression->type))
            print_span_error(file, expression->span, "%t expected but found %t.", field->type, expression->type);
    }
}

void type_check_compound_literal(const File* file, Compound_Literal* literal, Type_Handle inferred_type_handle, Expression_Check_Flags flags)
{
    if (flags & Expression_Check_Flags_constant)
        print_span_error(file, literal->span, "Compound literal is not constant.");

    if (is_invalid_type_handle(inferred_type_handle))
        print_span_error(file, literal->span, "Compound type cannot be inferred.");

    Type* inferred_type = lookup_type(inferred_type_handle);
    if (inferred_type->tag != Type_Tag_compound)
        print_span_error(file, literal->span, "Cannot infer literal as %t.", inferred_type_handle);

    literal->type = inferred_type_handle;
    each_field_is_initialized_at_most_once(file, literal);
    type_check_field_initializers(file, &inferred_type->compound, literal, flags);
}

static void deep_copy_field_initializer(Field_Initializer* destination, const Field_Initializer* source)
{
    destination->expression = add_expression();
    deep_copy_expression(lookup_expression(destination->expression), lookup_expression(source->expression));

    destination->field = source->field;
}

static void deep_copy_field_initializers(Field_Initializer_Handle_Array* restrict destination, const Field_Initializer_Handle_Array* restrict source)
{
    reserve_field_initializer_handles(destination, source->handles_length);
    for (Array_Size i = 0; i < source->handles_length; i++)
    {
        add_to_field_initializer_array(destination, add_field_initializer());
        deep_copy_field_initializer(lookup_field_initializer(field_initializer_at(destination, i)), lookup_field_initializer(field_initializer_at(source, i)));
    }
}

void deep_copy_compound_literal(Compound_Literal* restrict destination, const Compound_Literal* restrict source)
{
    deep_copy_field_initializers(&destination->field_initializers, &source->field_initializers);
}
