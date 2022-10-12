#include "statements.h"

void lower_statements(Ir_Basic_Block* block, const Statement_Handle_Array* body)
{
    for (Array_Size i = 0; i < body->handles_length; i++)
    {
        Statement* statement = lookup_statement(statement_at(body, i));
        if (statement->tag == Statement_Tag_return)
            lower_return_statement(block, &statement->return_statement.values);
        else
            unexpected_statement(__FILE__, __LINE__, statement->tag);
    }
}
