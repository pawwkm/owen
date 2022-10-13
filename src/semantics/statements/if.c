#include "statement.h"

bool type_check_if_statement(const File* file, const If_Statement* if_statement, const Function* function, bool is_inside_loop)
{
    bool terminated = true;
    bool else_branch = false;
    for (Array_Size i = 0; i < if_statement->branches.handles_length; i++)
    {
        Branch* branch = lookup_in(&if_statement->branches, i);
        if (!invalid(branch->declaration_statement))
            type_check_declaration_statement(file, &lookup(branch->declaration_statement)->declaration_statement);
        
        if (invalid(branch->condition))
            else_branch = true;
        else
        {
            Expression* condition = lookup(branch->condition);
            type_check_expression(file, condition, invalid_type_handle, Expression_Check_Flags_rhs_value);
            if (!compare(condition->type, bool_handle))
                print_span_error(file, condition->span, "%t expected but found %t.", bool_handle, condition->type);
        }

        if (!type_check_statements(file, &branch->body, function, is_inside_loop))
            terminated = false;
    }

    return terminated && else_branch;
}

void deep_copy_branch(Branch* restrict destination, Branch* restrict source)
{
    if (invalid(source->declaration_statement))
        destination->declaration_statement = invalid_statement_handle;
    else
    {
        destination->declaration_statement = add_statement();
        deep_copy_declaration_statement(&lookup(destination->declaration_statement)->declaration_statement, &lookup(source->declaration_statement)->declaration_statement);
    }

    destination->condition = add_expression();
    deep_copy_expression(lookup(destination->condition), lookup(source->condition));
    deep_copy_statements(&destination->body, &source->body);
}

void deep_copy_if_statement(If_Statement* restrict destination, const If_Statement* restrict source)
{
    reserve(&destination->branches, source->branches.handles_length);
    for (Array_Size i = 0; i < source->branches.handles_length; i++)
    {
        add_to(&destination->branches, add_branch());
        deep_copy_branch(lookup_in(&destination->branches, i), lookup_in(&source->branches, i));
    }
}
