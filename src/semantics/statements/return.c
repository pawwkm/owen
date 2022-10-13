#include "statement.h"

void type_check_return_statement(const File* file, const Return_Statement* return_statement, const Function* function)
{
    Function_Type* signature = &lookup(function->signature)->function; 
    if (return_statement->values.handles_length != signature->return_types.handles_length)
    {
        Span span;
        if (!return_statement->values.handles_length)
            span = return_statement->span;
        else if (return_statement->values.handles_length > signature->return_types.handles_length)
            span = lookup_in(&return_statement->values, signature->return_types.handles_length)->span;
        else
            span = lookup_in(&return_statement->values, return_statement->values.handles_length - 1)->span;

        print_span_error(file, span, "%u %s expected but found %u.", 
                                     signature->return_types.handles_length,
                                     signature->return_types.handles_length != 1 ? "expressions" : "expression",
                                     return_statement->values.handles_length);
    }

    for (Array_Size i = 0; i < return_statement->values.handles_length; i++)
    {
        Type_Handle expected_type = handle_at(&signature->return_types, i);
        Expression* returned_value = lookup_in(&return_statement->values, i);

        type_check_expression(file, returned_value, expected_type, Expression_Check_Flags_retain | Expression_Check_Flags_allow_unitialized_literals | Expression_Check_Flags_rhs_value);
        if (!expression_types_match(expected_type, returned_value->type))
            print_span_error(file, returned_value->span, "%t expected but found %t.", expected_type, returned_value->type);
    }
}

void deep_copy_return_statement(Return_Statement* restrict destination, const Return_Statement* restrict source)
{
    destination->span = source->span;
    deep_copy_expressions(&destination->values, &source->values);
}
