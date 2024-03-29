#include "expression.h"

static void each_field_is_initialized_at_most_once(const File* file, const Compound_Literal* literal)
{
    for (Array_Size a = 0; a < literal->field_initializers.handles_length; a++)
    {
        Field_Initializer* a_initializer = lookup_in(&literal->field_initializers, a);
        for (Array_Size b = a + 1; b < literal->field_initializers.handles_length; b++)
        {
            Field_Initializer* b_initializer = lookup_in(&literal->field_initializers, b);
            if (compare(a_initializer->field, b_initializer->field))
                print_span_error(file, b_initializer->field_span, "%I is initialized multiple times.", b_initializer->field);
        }
    }
}

static void type_check_field_initializers(const File* file, const Compound_Type* type, const Compound_Literal* compound_literal, Expression_Check_Flags flags, Qualifier qualifiers)
{
    for (Array_Size i = 0; i < compound_literal->field_initializers.handles_length; i++)
    {
        Field_Initializer* initializer = lookup_in(&compound_literal->field_initializers, i);
        Field* field = lookup_field_by_name(type, initializer->field, initializer->field_span);
        Expression* expression = lookup(initializer->expression);

        Type_Handle qualified_field_type = qualifiers ? find_or_add_qualified_type(qualifiers, field->type) : field->type;

        type_check_expression(file, expression, qualified_field_type, flags);
        if (!expression_types_match(qualified_field_type, expression->type))
            print_span_error(file, expression->span, "%t expected but found %t.", qualified_field_type, expression->type);
    }
}

void type_check_compound_literal(const File* file, Compound_Literal* literal, Type_Handle inferred_type_handle, Expression_Check_Flags flags)
{
    if (flags & Expression_Check_Flags_constant)
        print_span_error(file, literal->span, "Compound literal is not constant.");

    if (invalid(inferred_type_handle))
        print_span_error(file, literal->span, "Compound type cannot be inferred.");

    Qualified_Type* qualified_type = &lookup(inferred_type_handle)->qualified;
    Compound_Type* unqualified_compound_type = &unqualified(inferred_type_handle)->compound;
    
    if (unqualified_compound_type->tag != Type_Tag_compound)
        print_span_error(file, literal->span, "Cannot infer literal as %t.", inferred_type_handle);

    literal->type = inferred_type_handle;
    each_field_is_initialized_at_most_once(file, literal);
    type_check_field_initializers(file, unqualified_compound_type, literal, flags, qualified_type->tag == Type_Tag_qualified ? qualified_type->qualifiers : 0);
}

static void deep_copy_field_initializer(Field_Initializer* destination, const Field_Initializer* source)
{
    destination->expression = add_expression();
    deep_copy_expression(lookup(destination->expression), lookup(source->expression));

    destination->field = source->field;
}

static void deep_copy_field_initializers(Field_Initializer_Handle_Array* restrict destination, const Field_Initializer_Handle_Array* restrict source)
{
    reserve(destination, source->handles_length);
    for (Array_Size i = 0; i < source->handles_length; i++)
    {
        add_to(destination, add_field_initializer());
        deep_copy_field_initializer(lookup_in(destination, i), lookup_in(source, i));
    }
}

void deep_copy_compound_literal(Compound_Literal* restrict destination, const Compound_Literal* restrict source)
{
    deep_copy_field_initializers(&destination->field_initializers, &source->field_initializers);
}
