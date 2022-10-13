#include "expression.h"

static bool is_castable(const Type* a, const Type* b)
{
    if (a->tag == Type_Tag_enumeration)
        return is_castable(lookup(a->enumeration.underlying_type), b);
    else if (b->tag == Type_Tag_enumeration)
        return is_castable(a, lookup(b->enumeration.underlying_type));

    return a->tag <= Type_Tag_u64 && b->tag <= Type_Tag_u64 ||
           a->tag == Type_Tag_f32 && b->tag == Type_Tag_f64 ||
           a->tag == Type_Tag_f64 && b->tag == Type_Tag_f32 ||
           a->tag == Type_Tag_f32 && b->tag == Type_Tag_f32 ||
           a->tag == Type_Tag_f64 && b->tag == Type_Tag_f64 ||
           a->tag <= Type_Tag_u64 && (b->tag == Type_Tag_f32 || b->tag == Type_Tag_f64) ||
           b->tag <= Type_Tag_u64 && (a->tag == Type_Tag_f32 || a->tag == Type_Tag_f64);
}

void type_check_cast(const File* file, Cast* cast, Expression_Check_Flags flags)
{
    if (flags & Expression_Check_Flags_constant)
        print_span_error(file, cast->span, "Cast is not constant.");

    cast->type = lookup_type_by_reference(file, cast->target, true);
    Type* target_type = lookup(cast->type);

    Expression* source = lookup(cast->source);
    type_check_expression(file, source, cast->type, flags);
    if (target_type->tag != Type_Tag_function && !is_castable(lookup(source->type), target_type))
        print_span_error(file, cast->span, "Cannot cast %t to %t", source->type, cast->type);
}

void deep_copy_cast(Cast* restrict destination, const Cast* restrict source)
{
    destination->source = add_expression();
    deep_copy_expression(lookup(destination->source), lookup(source->source));

    destination->target = add_type_reference();
    deep_copy_type_reference(destination->target, source->target);
}
