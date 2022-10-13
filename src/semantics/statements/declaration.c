#include "statement.h"

static void type_check_tuple_declaration_statement(const File* file, Declaration_Statement* declaration_statement)
{
    Expression* expression = lookup_in(&declaration_statement->expressions, 0);
    type_check_expression(file, expression, invalid_type_handle, Expression_Check_Flags_rhs_value);

    for (Type_Handle handle = { 0 }; handle.index < types_length; handle.index++)
    {
        Type* type = lookup(handle);
        if (type->tag != Type_Tag_tuple || type->tuple.types.handles_length != declaration_statement->variables.handles_length)
            continue;

        bool is_match = true;
        for (Array_Size b = 0; b < declaration_statement->variables.handles_length; b++)
        {
            Variable* variable = lookup_in(&declaration_statement->variables, b);
            if (!expression_types_match(variable->type, handle_at(&type->tuple.types, b)))
            {
                is_match = false;
                break;
            }
        }

        if (is_match)
            break;

        Type_Handle type_handle = add_type();
        Type* variable_tuple = lookup(type_handle);
        variable_tuple->tag = Type_Tag_tuple;

        for (Array_Size i = 0; i < type->tuple.types.handles_length; i++)
        {
            Variable* lhs = lookup_in(&declaration_statement->variables, i);
            add_to(&variable_tuple->tuple.types, lhs->type);
        }

        print_span_error(file, expression->span, "%t expected but found %t.", type_handle, expression->type);
    }

    for (Array_Size i = 0; i < declaration_statement->variables.handles_length; i++)
    {
        Variable* variable = lookup_in(&declaration_statement->variables, i);
        if (!variable->is_blank)
        {
            check_that_identifier_is_undefined(file, variable->name, variable->name_span);
            add_symbol(variable->name, variable->name_span, variable->type);
        }
    }
}

static void type_check_ballanced_declaration_statement(const File* file, Declaration_Statement* declaration_statement)
{
    for (Array_Size i = 0; i < declaration_statement->variables.handles_length; i++)
    {
        Variable* variable = lookup_in(&declaration_statement->variables, i);
        if (variable->is_blank)
            print_span_error(file, variable->name_span, "_ only allowed in tuple declarations and assignments.");

        check_that_identifier_is_undefined(file, variable->name, variable->name_span);

        Type* variable_type = lookup(variable->type);
        Expression* expression = lookup_in(&declaration_statement->expressions, i);
        
        Expression_Check_Flags flags = Expression_Check_Flags_allow_unitialized_literals | Expression_Check_Flags_rhs_value;
        if (variable_type->tag == Type_Tag_pointer && variable_type->pointer.privileges & Pointer_Type_Privilege_retained)
            flags |= Expression_Check_Flags_retain;

        type_check_expression(file, expression, variable->type, flags);
        if (!expression_types_match(variable->type, expression->type))
            print_span_error(file, expression->span, "%t expected but found %t.", variable->type, expression->type);

        add_symbol(variable->name, variable->name_span, variable->type);
    }
}

void type_check_declaration_statement(const File* file, Declaration_Statement* declaration_statement)
{
    for (Array_Size i = 0; i < declaration_statement->variables.handles_length; i++)
    {
        Variable* variable = lookup_in(&declaration_statement->variables, i);
        variable->type = lookup_type_by_reference(file, variable->type_reference, true);
    }

    if (!declaration_statement->expressions.handles_length)
    {
        for (Array_Size i = 0; i < declaration_statement->variables.handles_length; i++)
        {
            Variable* variable = lookup_in(&declaration_statement->variables, i);
            if (variable->is_blank)
                print_span_error(file, variable->name_span, "_ cannot be zeroed.");

            check_that_identifier_is_undefined(file, variable->name, variable->name_span);
            add_symbol(variable->name, variable->name_span, variable->type);
        }
    }
    else if (declaration_statement->variables.handles_length > 1 && declaration_statement->expressions.handles_length == 1)
        type_check_tuple_declaration_statement(file, declaration_statement); 
    else if (declaration_statement->variables.handles_length == declaration_statement->expressions.handles_length)
        type_check_ballanced_declaration_statement(file, declaration_statement);
    else
    {
        Array_Size i;
        if (declaration_statement->expressions.handles_length >= declaration_statement->variables.handles_length)
            i = declaration_statement->expressions.handles_length - declaration_statement->variables.handles_length - 1;
        else
            i = declaration_statement->expressions.handles_length - 1;
        
        Expression_Handle last_handle = handle_at(&declaration_statement->expressions, i);
        Expression* last = lookup(last_handle);

        print_span_error(file, last->span, "%u %s expected but found %u.", 
                                           declaration_statement->variables.handles_length,
                                           declaration_statement->variables.handles_length == 1 ? "expression" : "expressions",
                                           declaration_statement->expressions.handles_length);
    }
}

static void deep_copy_variable(Variable* restrict destination, const Variable* restrict source)
{
    destination->name = source->name;

    destination->type_reference = add_type_reference();
    deep_copy_type_reference(destination->type_reference, source->type_reference);
}

static void deep_copy_variables(Variable_Handle_Array* restrict destination, const Variable_Handle_Array* restrict source)
{
    reserve(destination, source->handles_length);
    for (Array_Size i = 0; i < source->handles_length; i++)
    {
        add_to(destination, add_variable());
        deep_copy_variable(lookup_in(destination, i), lookup_in(source, i));
    }
}

void deep_copy_declaration_statement(Declaration_Statement* restrict destination, const Declaration_Statement* restrict source)
{
    deep_copy_variables(&destination->variables, &source->variables);
    deep_copy_expressions(&destination->expressions, &source->expressions);
}
