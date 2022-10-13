#include "statement.h"

void type_check_loop_statement(const File* file, Loop_Statement* loop_statement, const Function* function)
{
    if (!invalid(loop_statement->declaration_statement))
        type_check_declaration_statement(file, &lookup(loop_statement->declaration_statement)->declaration_statement);
    
    if (!invalid(loop_statement->condition))
    {
        Expression* condition = lookup(loop_statement->condition);
        type_check_expression(file, condition, invalid_type_handle, Expression_Check_Flags_rhs_value);
        if (!compare(condition->type, bool_handle))
            print_span_error(file, condition->span, "%t expected but found %t.", bool_handle, condition->type);
    }

    if (!invalid(loop_statement->assignment_statement))
        type_check_assignment_statement(file, &lookup(loop_statement->assignment_statement)->assignment_statement);

    type_check_statements(file, &loop_statement->body, function, true);
}

void deep_copy_loop_statement(Loop_Statement* restrict destination, const Loop_Statement* restrict source)
{
    if (invalid(source->declaration_statement))
        destination->declaration_statement = invalid_statement_handle;
    else
    {
        destination->declaration_statement = add_statement();
        deep_copy_declaration_statement(&lookup(destination->declaration_statement)->declaration_statement, &lookup(source->declaration_statement)->declaration_statement);
    }

    if (invalid(source->condition))
        destination->condition = invalid_expression_handle;
    else
    {
        destination->condition = add_expression();
        deep_copy_expression(lookup(destination->condition), lookup(source->condition));
    }

    if (invalid(source->assignment_statement))
        destination->assignment_statement = invalid_statement_handle;
    else
    {
        destination->assignment_statement = add_statement();
        deep_copy_assignment_statement(&lookup(destination->assignment_statement)->assignment_statement, &lookup(source->assignment_statement)->assignment_statement);
    }

    deep_copy_statements(&destination->body, &source->body);    
}
