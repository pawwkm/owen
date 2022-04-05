#include "statement.h"

void type_check_while_statement(const File* file, While_Statement* while_statement, const Function* function)
{
    if (!is_invalid_statement_handle(while_statement->declaration_statement))
        type_check_declaration_statement(file, &lookup_statement(while_statement->declaration_statement)->declaration_statement);
    
    Expression* condition = lookup_expression(while_statement->condition);
    type_check_expression(file, condition, invalid_type_handle, Expression_Check_Flags_rhs_value);
    if (!compare_types(condition->type, bool_handle))
        print_span_error(file, condition->span, "%t expected but found %t.", bool_handle, condition->type);

    type_check_statements(file, &while_statement->body, function, true);
}

void deep_copy_while_statement(While_Statement* restrict destination, const While_Statement* restrict source)
{
    if (is_invalid_statement_handle(source->declaration_statement))
        destination->declaration_statement = invalid_statement_handle;
    else
    {
        destination->declaration_statement = add_statement();
        deep_copy_declaration_statement(&lookup_statement(destination->declaration_statement)->declaration_statement, &lookup_statement(source->declaration_statement)->declaration_statement);
    }

    destination->condition = add_expression();
    deep_copy_expression(lookup_expression(destination->condition), lookup_expression(source->condition));

    deep_copy_statements(&destination->body, &source->body);
}
