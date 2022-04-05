#include "statement.h"

static void type_check_tuple_declaration_statement(const File* file, Declaration_Statement* declaration_statement)
{
    Expression* expression = lookup_expression(declaration_statement->expressions.handles[0]);
    type_check_expression(file, expression, invalid_type_handle, Expression_Check_Flags_rhs_value);

    for (Type_Handle handle = { 0 }; handle.index < types_length; handle.index++)
    {
        Type* type = lookup_type(handle);
        if (type->tag != Type_Tag_tuple || type->tuple.types.handles_length != declaration_statement->variables.handles_length)
            continue;

        bool is_match = true;
        for (uint8_t b = 0; b < declaration_statement->variables.handles_length; b++)
        {
            Variable* variable = lookup_variable(declaration_statement->variables.handles[b]);
            if (!expression_types_match(variable->type, type->tuple.types.handles[b]))
            {
                is_match = false;
                break;
            }
        }

        if (is_match)
            break;

        Type_Handle type_handle = add_type();
        Type* variable_tuple = lookup_type(type_handle);
        variable_tuple->tag = Type_Tag_tuple;

        for (uint8_t i = 0; i < type->tuple.types.handles_length; i++)
        {
            Variable* lhs = lookup_variable(declaration_statement->variables.handles[i]);
            add_to_type_array(&variable_tuple->tuple.types, lhs->type);
        }

        print_span_error(file, expression->span, "%t expected but found %t.", type_handle, expression->type);
    }

    for (uint8_t i = 0; i < declaration_statement->variables.handles_length; i++)
    {
        Variable* variable = lookup_variable(declaration_statement->variables.handles[i]);
        if (!variable->is_blank)
        {
            check_that_identifier_is_undefined(file, variable->name, variable->name_span);
            add_symbol(variable->name, variable->name_span, variable->type);
        }
    }
}

static void type_check_ballanced_declaration_statement(const File* file, Declaration_Statement* declaration_statement)
{
    for (uint8_t i = 0; i < declaration_statement->variables.handles_length; i++)
    {
        Variable* variable = lookup_variable(declaration_statement->variables.handles[i]);
        if (variable->is_blank)
            print_span_error(file, variable->name_span, "_ only allowed in tuple declarations and assignments.");

        check_that_identifier_is_undefined(file, variable->name, variable->name_span);

        Type* variable_type = lookup_type(variable->type);
        Expression* expression = lookup_expression(declaration_statement->expressions.handles[i]);
        
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
    for (uint8_t i = 0; i < declaration_statement->variables.handles_length; i++)
    {
        Variable* variable = lookup_variable(declaration_statement->variables.handles[i]);
        variable->type = lookup_type_by_reference(file, variable->type_reference, true);
    }

    if (!declaration_statement->expressions.handles_length)
    {
        for (uint8_t i = 0; i < declaration_statement->variables.handles_length; i++)
        {
            Variable* variable = lookup_variable(declaration_statement->variables.handles[i]);
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
        uint32_t i;
        if (declaration_statement->expressions.handles_length >= declaration_statement->variables.handles_length)
            i = declaration_statement->expressions.handles_length - declaration_statement->variables.handles_length - 1;
        else
            i = declaration_statement->expressions.handles_length - 1;
        
        Expression_Handle last_handle = declaration_statement->expressions.handles[i];
        Expression* last = lookup_expression(last_handle);

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
    reserve_variable_handles(destination, source->handles_length);
    for (uint8_t i = 0; i < source->handles_length; i++)
    {
        add_to_variable_array(destination, add_variable());
        deep_copy_variable(lookup_variable(destination->handles[i]), lookup_variable(source->handles[i]));
    }
}

void deep_copy_declaration_statement(Declaration_Statement* restrict destination, const Declaration_Statement* restrict source)
{
    deep_copy_variables(&destination->variables, &source->variables);
    deep_copy_expressions(&destination->expressions, &source->expressions);
}
