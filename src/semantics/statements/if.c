#include "statement.h"

bool type_check_if_statement(const File* file, const If_Statement* if_statement, const Function* function, bool is_inside_loop)
{
    bool terminated = true;
    bool else_branch = false;
    for (Array_Size i = 0; i < if_statement->branches.handles_length; i++)
    {
        Branch* branch = lookup_branch(branch_at(&if_statement->branches, i));
        if (!is_invalid_statement_handle(branch->declaration_statement))
            type_check_declaration_statement(file, &lookup_statement(branch->declaration_statement)->declaration_statement);
        
        if (is_invalid_expression_handle(branch->condition))
            else_branch = true;
        else
        {
            Expression* condition = lookup_expression(branch->condition);
            type_check_expression(file, condition, invalid_type_handle, Expression_Check_Flags_rhs_value);
            if (!compare_types(condition->type, bool_handle))
                print_span_error(file, condition->span, "%t expected but found %t.", bool_handle, condition->type);
        }

        if (!type_check_statements(file, &branch->body, function, is_inside_loop))
            terminated = false;
    }

    return terminated && else_branch;
}

void deep_copy_branch(Branch* restrict destination, Branch* restrict source)
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

void deep_copy_if_statement(If_Statement* restrict destination, const If_Statement* restrict source)
{
    reserve_branch_handles(&destination->branches, source->branches.handles_length);
    for (Array_Size i = 0; i < source->branches.handles_length; i++)
    {
        add_to_branch_array(&destination->branches, add_branch());
        deep_copy_branch(lookup_branch(branch_at(&destination->branches, i)), lookup_branch(branch_at(&source->branches, i)));
    }
}
