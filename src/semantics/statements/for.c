#include "statement.h"

void type_check_for_statement(const File* file, For_Statement* for_statement, const Function* function)
{
    type_check_declaration_statement(file, &lookup_statement(for_statement->declaration_statement)->declaration_statement);
    
    Expression* condition = lookup_expression(for_statement->condition);
    type_check_expression(file, condition, invalid_type_handle, Expression_Check_Flags_rhs_value);
    if (!compare_types(condition->type, bool_handle))
        print_span_error(file, condition->span, "%t expected but found %t.", bool_handle, condition->type);

    type_check_assignment_statement(file, &lookup_statement(for_statement->assignment_statement)->assignment_statement);
    type_check_statements(file, &for_statement->body, function, true);
}

void deep_copy_for_statement(For_Statement* restrict destination, const For_Statement* restrict source)
{
    destination->declaration_statement = add_statement();
    deep_copy_declaration_statement(&lookup_statement(destination->declaration_statement)->declaration_statement, &lookup_statement(source->declaration_statement)->declaration_statement);
    
    destination->condition = add_expression();
    deep_copy_expression(lookup_expression(destination->condition), lookup_expression(source->condition));

    destination->assignment_statement = add_statement();
    deep_copy_assignment_statement(&lookup_statement(destination->assignment_statement)->assignment_statement, &lookup_statement(source->assignment_statement)->assignment_statement);

    deep_copy_statements(&destination->body, &source->body);    
}
