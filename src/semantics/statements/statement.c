#include "statement.h"

bool type_check_statements(const File* file, Statement_Handle_Array* body, const Function* function, bool is_inside_loop)
{
    bool terminated = false;

    uint8_t initial_symbol_length = current_symbol_table->symbols_length;
    for (uint16_t i = 0; i < body->handles_length; i++)
    {
        Statement* statement = lookup_statement(body->handles[i]);
        if (statement->tag == Statement_Tag_declaration)
            type_check_declaration_statement(file, &statement->declaration_statement);
        else if (statement->tag == Statement_Tag_assignment)
            type_check_assignment_statement(file, &statement->assignment_statement);
        else if (statement->tag == Statement_Tag_expression)
            type_check_expression_statement(file, lookup_expression(statement->expression_statement));
        else if (statement->tag == Statement_Tag_if)
            terminated = type_check_if_statement(file, &statement->if_statement, function, is_inside_loop) && i == body->handles_length - 1;
        else if (statement->tag == Statement_Tag_for)
            type_check_for_statement(file, &statement->for_statement, function);
        else if (statement->tag == Statement_Tag_while)
            type_check_while_statement(file, &statement->while_statement, function);
        else if (statement->tag == Statement_Tag_break)
            type_check_break_statement(file, &statement->break_statement, is_inside_loop);
        else if (statement->tag == Statement_Tag_continue)
            type_check_continue_statement(file, &statement->continue_statement, is_inside_loop);
        else if (statement->tag == Statement_Tag_return)
        {
            type_check_return_statement(file, &statement->return_statement, function);
            terminated = true;
        }
    }

    check_for_unreferenced_variables(file, initial_symbol_length);
    current_symbol_table->symbols_length = initial_symbol_length;

    return terminated;
}

static void deep_copy_statement(Statement* restrict destination, const Statement* restrict source)
{
    destination->tag = source->tag;
    if (source->tag == Statement_Tag_declaration)
        deep_copy_declaration_statement(&destination->declaration_statement, &source->declaration_statement);
    else if (source->tag == Statement_Tag_assignment)
        deep_copy_assignment_statement(&destination->assignment_statement, &source->assignment_statement);
    else if (source->tag == Statement_Tag_expression)
    {
        destination->expression_statement = add_expression();
        deep_copy_expression(lookup_expression(destination->expression_statement), lookup_expression(source->expression_statement));
    }
    else if (source->tag == Statement_Tag_if)
        deep_copy_if_statement(&destination->if_statement, &source->if_statement);
    else if (source->tag == Statement_Tag_for)
        deep_copy_for_statement(&destination->for_statement, &source->for_statement);
    else if (source->tag == Statement_Tag_while)
        deep_copy_while_statement(&destination->while_statement, &source->while_statement);
    else if (source->tag == Statement_Tag_break)
        deep_copy_break_statement(&destination->break_statement, &source->break_statement);
    else if (source->tag == Statement_Tag_continue)
        deep_copy_continue_statement(&destination->continue_statement, &source->continue_statement);
    else if (source->tag == Statement_Tag_return)
        deep_copy_return_statement(&destination->return_statement, &source->return_statement);
}

void deep_copy_statements(Statement_Handle_Array* destination, const Statement_Handle_Array* source)
{
    reserve_statement_handles(destination, source->handles_length);
    for (uint8_t i = 0; i < source->handles_length; i++)
    {
        add_to_statement_array(destination, add_statement());
        deep_copy_statement(lookup_statement(destination->handles[i]), lookup_statement(source->handles[i]));
    }
}

