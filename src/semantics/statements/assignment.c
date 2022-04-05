#include "statement.h"

static bool operator_is_defined(Type_Handle expected, uint8_t operator_tag, Type_Handle actual)
{
    bool types_match     = expression_types_match(expected, actual);
    uint8_t expected_tag = lookup_type(expected)->tag;
    uint8_t actual_tag   = lookup_type(actual)->tag;

    if (operator_tag == Assignment_Operator_plus_equal || operator_tag == Assignment_Operator_minus_equal)
        return types_match && actual_tag <= Type_Tag_f64 || 
               expected_tag == Type_Tag_pointer && actual_tag >= Type_Tag_u8 && actual_tag <= Type_Tag_u64;
    else if (operator_tag == Assignment_Operator_multiply_equal || operator_tag == Assignment_Operator_divide_equal)
        return types_match && actual_tag <= Type_Tag_f64;
    else if (operator_tag == Assignment_Operator_bitwise_or_equal || operator_tag == Assignment_Operator_bitwise_xor_equal)
        return types_match && (actual_tag <= Type_Tag_u64 || actual_tag == Type_Tag_enumeration);
    else if (operator_tag == Assignment_Operator_bitwise_and_equal)
        return types_match && (actual_tag <= Type_Tag_u64 || actual_tag == Type_Tag_enumeration);
    else if (operator_tag == Assignment_Operator_modulo_equal)
        return types_match && actual_tag <= Type_Tag_f64;
    else if (operator_tag == Assignment_Operator_left_shift_equal || operator_tag == Assignment_Operator_right_shift_equal)
        return types_match && (actual_tag <= Type_Tag_u64 || actual_tag == Type_Tag_enumeration);
    else if (operator_tag == Assignment_Operator_equal)
        return types_match;

    return false;
}

static void type_check_tuple_assignment_statement(const File* file, Assignment_Statement* assignment_statement)
{
    Expression* expression = lookup_expression(assignment_statement->rhs.handles[0]);
    type_check_expression(file, expression, invalid_type_handle, Expression_Check_Flags_rhs_value);

    for (Type_Handle handle = { 0 }; handle.index < types_length; handle.index++)
    {
        Tuple_Type* tuple = &lookup_type(handle)->tuple;
        if (tuple->tag != Type_Tag_tuple || tuple->types.handles_length != assignment_statement->lhs.handles_length)
            continue;

        bool is_match = true;
        for (uint8_t b = 0; b < assignment_statement->lhs.handles_length; b++)
        {
            Expression* lhs = lookup_expression(assignment_statement->lhs.handles[b]);
            Type_Handle rhs_type = tuple->types.handles[b];

            if (lhs->tag == Expression_Tag_blank_identifier)
            {
                lhs->type = tuple->types.handles[b];
                if (assignment_statement->operator != Assignment_Operator_equal)
                    print_span_error(file, assignment_statement->operator_span, "Operator not defined for _.");
            }
                
            if (!expression_types_match(lhs->type, rhs_type) || !operator_is_defined(lhs->type, assignment_statement->operator, rhs_type))
                // While we do know that this triggers a compile error we can't break
                // since we might not be done assigning types to lhs _
                is_match = false;
        }

        if (is_match)
            break;
        
        Type_Handle type_handle = add_type();
        Type* variable_tuple = lookup_type(type_handle);
        variable_tuple->tag = Type_Tag_tuple;

        reserve_type_handles(&variable_tuple->tuple.types, assignment_statement->lhs.handles_length);
        for (uint8_t i = 0; i < assignment_statement->lhs.handles_length; i++)
        {
            Expression* lhs = lookup_expression(assignment_statement->lhs.handles[i]);
            add_to_type_array(&variable_tuple->tuple.types, lhs->type);
        }

        print_span_error(file, expression->span, "%t expected but found %t.", type_handle, expression->type);
    }
}

static void type_check_ballanced_assignment_statement(const File* file, Assignment_Statement* assignment_statement)
{
    for (uint8_t i = 0; i < assignment_statement->lhs.handles_length; i++)
    {
        Expression* lhs = lookup_expression(assignment_statement->lhs.handles[i]);
        Expression* rhs = lookup_expression(assignment_statement->rhs.handles[i]);

        if (lhs->tag == Expression_Tag_blank_identifier)
            print_span_error(file, lhs->span, "_ only allowed in tuple declarations and assignments.");

        Type* lhs_type = lookup_type(lhs->type);
        Expression_Check_Flags flags = Expression_Check_Flags_rhs_value; 
        if (lhs_type->tag == Type_Tag_pointer && lhs_type->pointer.privileges & Pointer_Type_Privilege_retained)
            flags |= Expression_Check_Flags_retain;

        type_check_expression(file, rhs, lhs->type, flags);

        if (!operator_is_defined(lhs->type, assignment_statement->operator, rhs->type))
            print_span_error(file, assignment_statement->operator_span, "Operator not defined for %t and %t.", lhs->type, rhs->type);
    }
}

void type_check_assignment_statement(const File* file, Assignment_Statement* assignment_statement)
{
    for (uint8_t i = 0; i < assignment_statement->lhs.handles_length; i++)
    {
        Expression* lhs = lookup_expression(assignment_statement->lhs.handles[i]);

        type_check_expression(file, lhs, invalid_type_handle, Expression_Check_Flags_lhs_value);
        if (!is_addressable(lhs->tag))
            print_span_error(file, lhs->span, "Expression is not addressable.");
    }

    if (assignment_statement->lhs.handles_length > 1 && assignment_statement->rhs.handles_length == 1)
        type_check_tuple_assignment_statement(file, assignment_statement);
    else if (assignment_statement->lhs.handles_length == assignment_statement->rhs.handles_length)
        type_check_ballanced_assignment_statement(file, assignment_statement);
    else
    {
        Expression_Handle last;
        if (assignment_statement->lhs.handles_length > assignment_statement->rhs.handles_length)
            last = assignment_statement->rhs.handles[assignment_statement->rhs.handles_length - 1];
        else
            last = assignment_statement->lhs.handles[assignment_statement->lhs.handles_length - 1];

        print_span_error(file, lookup_expression(last)->span, "%u %s expected but found %u.", 
                                                              assignment_statement->lhs.handles_length,
                                                              assignment_statement->lhs.handles_length == 1 ? "expression" : "expressions",
                                                              assignment_statement->rhs.handles_length);
    }
}

void deep_copy_assignment_statement(Assignment_Statement* restrict destination, const Assignment_Statement* restrict source)
{
    deep_copy_expressions(&destination->lhs, &source->lhs);
    deep_copy_expressions(&destination->rhs, &source->rhs);

    destination->operator = source->operator;
    destination->operator_span = source->operator_span;
}