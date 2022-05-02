#include "statement.h"

void type_check_loop_statement(const File* file, Loop_Statement* loop_statement, const Function* function)
{
    if (!is_invalid_statement_handle(loop_statement->declaration_statement))
        type_check_declaration_statement(file, &lookup_statement(loop_statement->declaration_statement)->declaration_statement);
    
    if (!is_invalid_expression_handle(loop_statement->condition))
    {
        Expression* condition = lookup_expression(loop_statement->condition);
        type_check_expression(file, condition, invalid_type_handle, Expression_Check_Flags_rhs_value);
        if (!compare_types(condition->type, bool_handle))
            print_span_error(file, condition->span, "%t expected but found %t.", bool_handle, condition->type);
    }

    if (!is_invalid_statement_handle(loop_statement->assignment_statement))
        type_check_assignment_statement(file, &lookup_statement(loop_statement->assignment_statement)->assignment_statement);

    type_check_statements(file, &loop_statement->body, function, true);
}

void deep_copy_loop_statement(Loop_Statement* restrict destination, const Loop_Statement* restrict source)
{
    if (is_invalid_statement_handle(source->declaration_statement))
        destination->declaration_statement = invalid_statement_handle;
    else
    {
        destination->declaration_statement = add_statement();
        deep_copy_declaration_statement(&lookup_statement(destination->declaration_statement)->declaration_statement, &lookup_statement(source->declaration_statement)->declaration_statement);
    }

    if (is_invalid_expression_handle(source->condition))
        destination->condition = invalid_expression_handle;
    else
    {
        destination->condition = add_expression();
        deep_copy_expression(lookup_expression(destination->condition), lookup_expression(source->condition));
    }

    if (is_invalid_statement_handle(source->assignment_statement))
        destination->assignment_statement = invalid_statement_handle;
    else
    {
        destination->assignment_statement = add_statement();
        deep_copy_assignment_statement(&lookup_statement(destination->assignment_statement)->assignment_statement, &lookup_statement(source->assignment_statement)->assignment_statement);
    }

    deep_copy_statements(&destination->body, &source->body);    
}
