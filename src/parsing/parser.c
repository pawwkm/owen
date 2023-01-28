#include "parsing.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

static bool look_ahead_1(uint8_t tag)
{
    Lexer start = lexer;
    next_token();

    bool result = lexer.token.tag == tag;
    lexer = start;

    return result;
}

static Expression_Handle parse_expression(int8_t minimum_precedence);
static void parse_expressions(Expression_Handle_Array* array);
static void parse_statements(Statement_Handle_Array* body);
static Statement_Handle parse_declaration_statement(void);

static bool token_is_assignment_operator(void)
{
    return lexer.token.tag >= Token_Tag_plus_equal && lexer.token.tag <= Token_Tag_equal;
}

static void expect(Token_Tag tag, const char* message)
{
    if (lexer.token.tag != tag)
        print_span_error(file, lexer.token.span, message);

    next_token();
}

static bool advance_if(Token_Tag tag)
{
    if (lexer.token.tag != tag)
        return false;

    next_token();
    return true;
}

static bool is_type_ahead(void)
{
    Lexer start = lexer;
    bool result = false;

    while (lexer.token.tag != Token_Tag_eof)
    {
        if (lexer.token.tag == Token_Tag_left_square_bracket || 
            lexer.token.tag == Token_Tag_upper_case_identifier ||
            lexer.token.tag == Token_Tag_readonly || 
            lexer.token.tag == Token_Tag_noalias)
        {
            result = true;
            break;
        }
        else if (lexer.token.tag == Token_Tag_pound_sign)
            next_token();
        else
            break;
    }

    lexer = start;

    return result;
}

static bool parse_type(Type_Reference_Handle* reference_handle, bool allow_qualifiers);
static void parse_types(Type_Reference_Handle_Array* array, const char* type_of_types)
{
    do
    {
        Type_Reference_Handle type_reference;
        if (!parse_type(&type_reference, true))
            print_span_error(file, lexer.token.span, "%s type expected.", type_of_types);

        add_to(array, type_reference);
    } while (advance_if(Token_Tag_comma));
}

static void parse_base_type(Type_Reference_Handle* base_type)
{
    if (!parse_type(base_type, false))
        print_span_error(file, lexer.token.span, "Base type expected.");
}

static bool parse_type(Type_Reference_Handle* reference_handle, bool allow_qualifiers)
{
    if (lexer.token.tag == Token_Tag_left_square_bracket)
    {
        *reference_handle = add_type_reference();

        Position start = lexer.token.span.start;
        next_token();

        if (lexer.token.tag == Token_Tag_integer)
        {
            // type_check_integer_literal(lookup(fu
            // HACK: type_check_integer_literal used to be used for this part
            // when it was done in the samantics pass but it isn' vissible
            // from here. I could forward declare it but the semantics pass
            // is not yet initialized.
            //
            // So do the bare minimum for now.
            //
            // Maybe I could parse numbers here into 64 bit numbers and set the "type"
            // so that the semantics pass only need to be concerned with the number 
            // being within the inferred bounds.
            Fixed_Array_Type_Reference* array = &lookup(*reference_handle)->fixed_array;
            array->size = (uint32_t)atoi(lexer.token.string.text);
            array->tag = Type_Reference_Tag_fixed_array;
            next_token();

            expect(Token_Tag_right_square_bracket, "] expected.");
            parse_base_type(&array->base_type);

            array->span.start = start;
            array->span.end = lookup(array->base_type)->span.end;
        }
        else 
        {
            Dynamic_Array_Type_Reference* array = &lookup(*reference_handle)->dynamic_array;

            expect(Token_Tag_right_square_bracket, "] expected.");
            parse_base_type(&array->base_type);

            array->tag = Type_Reference_Tag_dynamic_array;
            array->span.start = start;
            array->span.end = lookup(array->base_type)->span.end;
        }
    }
    else if (lexer.token.tag == Token_Tag_pound_sign)
    {
        *reference_handle = add_type_reference();

        Pointer_Type_Reference* pointer = &lookup(*reference_handle)->pointer;
        pointer->span.start = lexer.token.span.start;
        pointer->tag = Type_Reference_Tag_pointer;

        next_token();
        parse_base_type(&pointer->base_type);
        pointer->span.end = lookup(pointer->base_type)->span.end;
    }
    else if (lexer.token.tag == Token_Tag_upper_case_identifier)
    {
        *reference_handle = add_type_reference();

        Type_Reference* reference = lookup(*reference_handle);
        
        Interned_String_Handle name = lexer.token.interned;
        reference->span = lexer.token.span;

        next_token();
        Lexer start = lexer;

        if (advance_if(Token_Tag_left_parentheses))
        {
            if (is_type_ahead())
            {
                parse_types(&reference->function.formal_parameters, "Formal parameter");
                expect(Token_Tag_right_parentheses, ") expected.");

                if (advance_if(Token_Tag_colon))
                    parse_types(&reference->function.return_types, "Return");

                reference->span.end = lexer.token.span.end;
                reference->tag = Type_Reference_Tag_function;
            }
            else if (lexer.token.tag == Token_Tag_right_parentheses)
            {
                reference->span.end = lexer.token.span.end;
                reference->tag = Type_Reference_Tag_function;

                next_token();
                if (advance_if(Token_Tag_colon))
                    parse_types(&reference->function.return_types, "Return");
            }
            else
            {
                lexer = start;
                reference->named.name = name;
                reference->tag = Type_Reference_Tag_name;
            }
        }
        else
        {
            reference->named.name = name;
            if (advance_if(Token_Tag_left_square_bracket))
            {
                if (is_type_ahead())
                {
                    do
                    {
                        Type_Reference_Handle handle;
                        if (!parse_type(&handle, true))
                            print_span_error(file, lexer.token.span, "Type expected.");

                        add_to(&reference->polymorphic_compound.actual_type_parameters, handle);
                    } while (advance_if(Token_Tag_comma));

                    reference->span.end = lexer.token.span.end;
                    expect(Token_Tag_right_square_bracket, "] expected.");
                    reference->tag = Type_Reference_Tag_polymorphic_compound;
                }
                else
                {
                    lexer = start;
                    reference->tag = Type_Reference_Tag_name;
                }
            }
            else
                reference->tag = Type_Reference_Tag_name;
        }
    }
    else if (allow_qualifiers && (lexer.token.tag == Token_Tag_readonly || lexer.token.tag == Token_Tag_noalias))
    {
        *reference_handle = add_type_reference();

        Qualified_Type_Reference* qualified = &lookup(*reference_handle)->qualified;
        qualified->span.start = lexer.token.span.start;
        qualified->tag = Type_Reference_Tag_qualified;

        if (advance_if(Token_Tag_readonly))
            qualified->qualifiers |= Qualifier_readonly;

        if (advance_if(Token_Tag_noalias))
            qualified->qualifiers |= Qualifier_noalias;

        parse_base_type(&qualified->unqualified);
        qualified->span.end = lookup(qualified->unqualified)->span.end;
    }
    else
        return false;

    return true;
}

static bool parse_field_initializer(Compound_Literal* literal)
{
    if (lexer.token.tag != Token_Tag_lower_case_identifier)
        return false;
    
    Lexer start = lexer;
    Interned_String_Handle field_name = lexer.token.interned;
    Span field_span = lexer.token.span;
    
    next_token();
    if (!advance_if(Token_Tag_equal))
    {
        lexer = start;
        return false;
    }

    Field_Initializer_Handle handle = add_field_initializer();
    Field_Initializer* initializer = lookup(handle);
    initializer->field = field_name;
    initializer->field_span = field_span;
    
    initializer->expression = parse_expression(0);
    if (invalid(initializer->expression))
        print_span_error(file, lexer.token.span, "Expression expected.");

    add_to(&literal->field_initializers, handle);

    return true;
}

static bool parse_element_initializer(Array_Literal* array_literal)
{
    if (advance_if(Token_Tag_left_square_bracket))
    {
        Element_Initializer_Handle initializer_handle = add_element_initializer();
        Element_Initializer* initializer = lookup(initializer_handle);

        initializer->explicit_index = parse_expression(0);
        if (invalid(initializer->explicit_index))
            print_span_error(file, lexer.token.span, "Expression expected.");

        expect(Token_Tag_right_square_bracket, "] expected.");
        expect(Token_Tag_equal, "= expected in element initializer.");

        initializer->expression = parse_expression(0);
        if (invalid(initializer->expression))
            print_span_error(file, lexer.token.span, "Expression expected.");
    
        add_to(&array_literal->elements, initializer_handle);
    }
    else
    {
        Expression_Handle expression_handle = parse_expression(0);
        if (invalid(expression_handle))
            return false;

        Element_Initializer_Handle initializer_handle = add_element_initializer();
        Element_Initializer* initializer = lookup(initializer_handle);

        initializer->explicit_index = invalid_expression_handle;
        initializer->expression = expression_handle;
        add_to(&array_literal->elements, initializer_handle);
    }

    return true;
}

static Expression_Handle parse_primary_expression(void)
{
    Expression_Handle expression_handle = invalid_expression_handle;
    if (lexer.token.tag == Token_Tag_integer)
    {
        expression_handle = add_expression();

        Unchecked_Number* literal = &lookup(expression_handle)->unchecked_number;
        literal->string = lexer.token.string;
        literal->tag = Expression_Tag_integer;
        literal->type = invalid_type_handle;
        literal->span = lexer.token.span;

        next_token();
    }
    else if (lexer.token.tag == Token_Tag_float)
    {
        expression_handle = add_expression();

        Unchecked_Number* literal = &lookup(expression_handle)->unchecked_number;
        literal->string = lexer.token.string;
        literal->tag = Expression_Tag_float;
        literal->type = invalid_type_handle;
        literal->span = lexer.token.span;

        next_token();
    }
    else if (lexer.token.tag == Token_Tag_lower_case_identifier)
    {
        expression_handle = add_expression();

        Reference* reference = &lookup(expression_handle)->reference;
        reference->name = lexer.token.interned;
        reference->tag = Expression_Tag_reference;
        reference->type = invalid_type_handle;
        reference->span = lexer.token.span;

        next_token();

        Lexer start = lexer;
        if (advance_if(Token_Tag_left_square_bracket))
        {
            if (is_type_ahead())
            {
                do
                {
                    Type_Reference_Handle type_reference_handle;
                    if (!parse_type(&type_reference_handle, true))
                        print_span_error(file, lexer.token.span, "Type expected.");

                    add_to(&reference->actual_type_parameters, type_reference_handle);
                } while (advance_if(Token_Tag_comma));

                reference->span.end = lexer.token.span.end;
                expect(Token_Tag_right_square_bracket, "] expected.");
            }
            else
                lexer = start;
        }
    }
    else if (lexer.token.tag == Token_Tag_true)
    {
        expression_handle = add_expression();

        Expression* literal = lookup(expression_handle);
        literal->tag = Expression_Tag_true;
        literal->type = invalid_type_handle;
        literal->span = lexer.token.span;

        next_token();
    }
    else if (lexer.token.tag == Token_Tag_false)
    {
        expression_handle = add_expression();

        Expression* literal = lookup(expression_handle);
        literal->tag = Expression_Tag_false;
        literal->type = invalid_type_handle;
        literal->span = lexer.token.span;

        next_token();
    }
    else if (lexer.token.tag == Token_Tag_unicode_code_point)
    {
        expression_handle = add_expression();

        Unicode_Code_Point_Literal* literal = &lookup(expression_handle)->code_point;
        literal->code_point = lexer.token.code_point;
        literal->tag = Expression_Tag_unicode_code_point;
        literal->type = invalid_type_handle;
        literal->span = lexer.token.span;

        next_token();
    }
    else if (lexer.token.tag == Token_Tag_utf8_string)
    {
        expression_handle = add_expression();

        Utf8_String_Literal* literal = &lookup(expression_handle)->string_literal;
        literal->string = lexer.token.interned;
        literal->tag = Expression_Tag_utf8_string_literal;
        literal->type = invalid_type_handle;
        literal->span = lexer.token.span;

        next_token();
    }
    else if (lexer.token.tag == Token_Tag_left_curly_bracket)
    {
        expression_handle = add_expression();
        Expression* expression = lookup(expression_handle);
        expression->type = invalid_type_handle;
        expression->span.start = lexer.token.span.start;

        next_token();
        if (parse_field_initializer(&expression->compound_literal))
            expression->tag = Expression_Tag_compound_literal;
        else if (parse_element_initializer(&expression->array_literal))
            expression->tag = Expression_Tag_array_literal;
        else
            print_span_error(file, lexer.token.span, "Field initializer or element initializer expected.");

        while (advance_if(Token_Tag_comma))
        {
            if (expression->tag == Expression_Tag_array_literal)
            {
                if (!parse_element_initializer(&expression->array_literal))
                    print_span_error(file, lexer.token.span, "Element initializer expected.");
            }
            else
            {
                if (!parse_field_initializer(&expression->compound_literal))
                    print_span_error(file, lexer.token.span, "Field initializer expected.");
            }
        }

        expression->span.end = lexer.token.span.end;
        expect(Token_Tag_right_curly_bracket, "Missing } in literal.");
    }
    else if (lexer.token.tag == Token_Tag_null)
    {
        expression_handle = add_expression();

        Expression* literal = lookup(expression_handle);
        literal->tag = Expression_Tag_null;
        literal->type = invalid_type_handle;
        literal->span = lexer.token.span;

        next_token();
    }
    else if (lexer.token.tag == Token_Tag_uninitialized_literal)
    {
        expression_handle = add_expression();

        Expression* literal = lookup(expression_handle);
        literal->tag = Expression_Tag_uninitialized_literal;
        literal->type = invalid_type_handle;
        literal->span = lexer.token.span;

        next_token();
    }
    else if (lexer.token.tag == Token_Tag_sizeof)
    {
        expression_handle = add_expression();

        Size_Of* size_of = &lookup(expression_handle)->size_of;
        size_of->tag = Expression_Tag_sizeof;
        size_of->type = invalid_type_handle;
        size_of->span.start = lexer.token.span.start;

        next_token();
        expect(Token_Tag_left_parentheses, "( expected.");

        if (parse_type(&size_of->t, true))
            size_of->expression = invalid_expression_handle;
        else 
        {
            size_of->expression = parse_expression(0);
            if (invalid(size_of->expression))
                print_span_error(file, lexer.token.span, "Type or expression expected.");

            size_of->t = invalid_type_reference_handle;
        }

        size_of->span.end = lexer.token.span.end;
        expect(Token_Tag_right_parentheses, ") expected.");
    }
    else if (lexer.token.tag == Token_Tag_left_parentheses)
    {
        expression_handle = add_expression();

        Parenthesized* parenthesized = &lookup(expression_handle)->parenthesized;
        parenthesized->tag = Expression_Tag_parenthesized;
        parenthesized->type = invalid_type_handle;
        parenthesized->span.start = lexer.token.span.start;

        next_token();
        parenthesized->expression = parse_expression(0);
        parenthesized->span.end = lexer.token.span.end;

        expect(Token_Tag_right_parentheses, ") expected.");
    }
    else if (lexer.token.tag == Token_Tag_blank_identifier)
    {
        expression_handle = add_expression();

        Expression* expression = lookup(expression_handle);
        expression->tag = Expression_Tag_blank_identifier;
        expression->type = invalid_type_handle;
        expression->span = lexer.token.span;

        next_token();
    }
    else if (lexer.token.tag == Token_Tag_upper_case_identifier)
    {
        expression_handle = add_expression();

        Position start = lexer.token.span.start;
        Type_Reference_Handle type;
        assert(parse_type(&type, false));

        if (advance_if(Token_Tag_dot))
        {
            Enumeration_Constant_Access* access = &lookup(expression_handle)->enumeration_constant_access;
            access->span.start = start;
            access->enumeration = type;
            access->constant = lexer.token.interned;
            access->span.end = lexer.token.span.end;
            access->type = invalid_type_handle;

            expect(Token_Tag_lower_case_identifier, "Enumeration constant expected.");
            access->tag = Expression_Tag_enumeration_constant_access;
        }
        else if (advance_if(Token_Tag_left_parentheses))
        {
            Cast* cast = &lookup(expression_handle)->cast;
            cast->span.start = start;
            cast->type = invalid_type_handle;
            cast->target = type;
            cast->source = parse_expression(0);
            
            if (invalid(cast->source))
                print_span_error(file, lexer.token.span, "Expression expected.");

            cast->tag = Expression_Tag_cast;
            cast->span.end = lexer.token.span.end;
            expect(Token_Tag_right_parentheses, ") expected.");
        }
        else
            print_span_error(file, lexer.token.span, ". or ( epxtected.");
    }
    else if (is_type_ahead())
    {
        expression_handle = add_expression();
        Cast* cast = &lookup(expression_handle)->cast;
        cast->type = invalid_type_handle;
        cast->span.start = lexer.token.span.start;

        parse_type(&cast->target, true);
        expect(Token_Tag_left_parentheses, "( expected.");

        cast->source = parse_expression(0);
        if (invalid(cast->source))
            print_span_error(file, lexer.token.span, "Expression expected.");

        cast->tag = Expression_Tag_cast;

        expect(Token_Tag_right_parentheses, ") expected.");
    }

    return expression_handle;
}

static Expression_Handle parse_postfix_expression(void)
{
    Expression_Handle expression_handle = parse_primary_expression();
    while (!invalid(expression_handle))
    {
        if (advance_if(Token_Tag_dot))
        {
            Expression_Handle field_access_handle = add_expression();
            Field_Access* field_access = &lookup(field_access_handle)->field_access;

            field_access->compound = expression_handle;
            field_access->field = lexer.token.interned;
            field_access->field_span = lexer.token.span;

            field_access->span.start = lookup(expression_handle)->span.start;
            field_access->span.end = lexer.token.span.end;
            field_access->type = invalid_type_handle;
            field_access->tag = Expression_Tag_field_access;

            expect(Token_Tag_lower_case_identifier, "Field expected.");
            expression_handle = field_access_handle;
        }
        else if (lexer.token.tag == Token_Tag_left_parentheses)
        {
            Expression_Handle call_handle = add_expression();
            Call* call = &lookup(call_handle)->call;

            call->span.start = lexer.token.span.start;
            call->callee = expression_handle;
            call->callee_declaration = invalid_function_handle;
            call->type = invalid_type_handle;
            call->tag = Expression_Tag_call;
            call->span.start = lexer.token.span.start;

            next_token();
            parse_expressions(&call->actual_parameters);

            call->span.end = lexer.token.span.end;
            expect(Token_Tag_right_parentheses, ") expected.");
            expression_handle = call_handle;
        }
        else if (lexer.token.tag == Token_Tag_left_square_bracket && !look_ahead_1(Token_Tag_right_square_bracket))
        {
            Expression_Handle array_access_handle = add_expression();
            Array_Access* array_access = &lookup(array_access_handle)->array_access;
            array_access->span.start = lexer.token.span.start;

            next_token();

            array_access->array = expression_handle;
            array_access->index = parse_expression(0);
            if (invalid(array_access->index))
                print_span_error(file, lexer.token.span, "Expression expected.");

            array_access->span.end = lexer.token.span.end;
            expect(Token_Tag_right_square_bracket, "] expected.");

            array_access->type = invalid_type_handle;
            array_access->tag = Expression_Tag_array_access;
            expression_handle = array_access_handle;
        }
        else
            break;
    }

    return expression_handle;
}

static Expression_Handle parse_prefix_expression(void)
{
    if (lexer.token.tag == Token_Tag_exclamation_mark)
    {
        Expression_Handle expression_handle = add_expression();
        Not* not = &lookup(expression_handle)->not;
        not->span.start = lexer.token.span.start;
        not->tag = Expression_Tag_not;
        not->type = invalid_type_handle;

        next_token();
        not->expression = parse_postfix_expression();
        not->span.end = lexer.token.span.end;

        return expression_handle;
    }
    else if (lexer.token.tag == Token_Tag_minus)
    {
        Expression_Handle expression_handle = add_expression();
        Negate* negate = &lookup(expression_handle)->negate;
        negate->span.start = lexer.token.span.start;
        negate->tag = Expression_Tag_negate;
        negate->type = invalid_type_handle;

        next_token();
        negate->expression = parse_postfix_expression();
        negate->span.end = lexer.token.span.end;

        return expression_handle;
    }
    else if (lexer.token.tag == Token_Tag_dereference)
    {
        Expression_Handle expression_handle = add_expression();
        Dereference* dereference = &lookup(expression_handle)->dereference;
        dereference->span.start = lexer.token.span.start;
        dereference->tag = Expression_Tag_dereference;
        dereference->type = invalid_type_handle;

        next_token();
        dereference->expression = parse_postfix_expression();
        dereference->span.end = lexer.token.span.end;

        return expression_handle;
    }
    else if (lexer.token.tag == Token_Tag_pound_sign && !is_type_ahead())
    {
        Expression_Handle expression_handle = add_expression();
        Address_Of* address_of = &lookup(expression_handle)->address_of;
        address_of->span.start = lexer.token.span.start;
        address_of->tag = Expression_Tag_address_of;
        address_of->type = invalid_type_handle;

        next_token();
        address_of->expression = parse_postfix_expression();
        address_of->span.end = lexer.token.span.end;

        return expression_handle;
    }
    else
        return parse_postfix_expression();
}

static int8_t precedence_of_operator(void)
{
    if (lexer.token.tag == Token_Tag_logical_or)
        return 0;
    else if (lexer.token.tag == Token_Tag_logical_and)
        return 1;
    else if (lexer.token.tag >= Token_Tag_equal_equal && lexer.token.tag <= Token_Tag_greater_than)
        return 2;
    else if (lexer.token.tag >= Token_Tag_plus && lexer.token.tag <= Token_Tag_bitwise_xor)
        return 3;
    else if (lexer.token.tag >= Token_Tag_multiply && lexer.token.tag <= Token_Tag_right_shift)
        return 4;
    else
        // All calls to parse_expression uses 0 as the minimum precedence so 
        // by returning -1 the function knows when to stop looking for operators.
        return -1;
}

static Expression_Handle parse_expression(int8_t minimum_precedence)
{
    Expression_Handle expression_handle = parse_prefix_expression();
    if (invalid(expression_handle))
        return expression_handle;

    while (true)
    {
        int8_t current_precedence = precedence_of_operator();
        if (current_precedence < minimum_precedence)
            break;

        Expression_Handle binary_handle = add_expression();

        Binary* binary = &lookup(binary_handle)->binary;
        binary->span.start = lookup(expression_handle)->span.start;
        binary->lhs = expression_handle;
        expression_handle = binary_handle;

        binary->operator_span = lexer.token.span;
        binary->operator = (Binary_Operator)lexer.token.tag;
        binary->type = invalid_type_handle;
        binary->tag = Expression_Tag_binary;

        // Skip operator.
        next_token();

        binary->rhs = parse_expression(current_precedence + 1);
        if (invalid(binary->rhs))
            print_span_error(file, lexer.token.span, "Expression expected.");

        binary->span.end = lookup(binary->rhs)->span.end;
    }

    return expression_handle;
}

static void parse_expressions(Expression_Handle_Array* array)
{
    Expression_Handle expression = parse_expression(0);
    if (invalid(expression))
        return;

    add_to(array, expression);
    while (advance_if(Token_Tag_comma))
    {
        expression = parse_expression(0);
        if (invalid(expression))
            print_span_error(file, lexer.token.span, "Expression expected.");

        add_to(array, expression);
    }
}

static void parse_assignment_statement(Assignment_Statement* assignment_statement)
{
    // We have already parsed the first expression.
    if (advance_if(Token_Tag_comma))
    {
        parse_expressions(&assignment_statement->lhs);
        if (assignment_statement->lhs.handles_length == 1)
            print_span_error(file, lexer.token.span, "Expression expected.");
    }

    if (!token_is_assignment_operator())
        print_span_error(file, lexer.token.span, "=, +=, -=, *=, /=, &=, |=, ^=, %=, <<= or >>= expected.");

    assignment_statement->operator = lexer.token.tag;
    assignment_statement->operator_span = lexer.token.span;
    next_token();

    parse_expressions(&assignment_statement->rhs);
    if (!assignment_statement->rhs.handles_length)
        print_span_error(file, lexer.token.span, "Expression expected.");
}

static Statement_Handle parse_if_statement(void)
{
    Statement_Handle statement_handle = add_statement();
    Statement* statement = lookup(statement_handle);

    do
    {
        Branch_Handle branch_handle = add_branch();
        Branch* branch = lookup(branch_handle);

        branch->declaration_statement = parse_declaration_statement();
        if (!invalid(branch->declaration_statement))
            expect(Token_Tag_semicolon, "; expected after declaration statement.");

        branch->condition = parse_expression(0);
        if (invalid(branch->condition))
        {
            if (invalid(branch->declaration_statement))
                print_span_error(file, lexer.token.span, "Declaration or condition expected.");
            else
                print_span_error(file, lexer.token.span, "Condition expected.");
        }

        parse_statements(&branch->body);
        add_to(&statement->if_statement.branches, branch_handle);
    }
    while (advance_if(Token_Tag_elif));
    
    if (advance_if(Token_Tag_else))
    {
        Branch_Handle branch_handle = add_branch();
        Branch* branch = lookup(branch_handle);

        branch->condition = invalid_expression_handle;
        branch->declaration_statement = invalid_statement_handle;

        parse_statements(&branch->body);
        add_to(&statement->if_statement.branches, branch_handle);
    }

    expect(Token_Tag_end, "Missing end keyword in branch.");

    statement->tag = Statement_Tag_if;
    return statement_handle;
}

static Statement_Handle parse_for_statement(void)
{
    Statement_Handle statement_handle = add_statement();
    Statement* statement = lookup(statement_handle);

    statement->loop_statement.declaration_statement = parse_declaration_statement();
    if (!invalid(statement->loop_statement.declaration_statement))
        expect(Token_Tag_semicolon, "; expected after declaration statement.");

    statement->loop_statement.condition = parse_expression(0);
    if (invalid(statement->loop_statement.condition))
        print_span_error(file, lexer.token.span, "Condition expected.");

    expect(Token_Tag_semicolon, "; expected after condition.");

    Expression_Handle first = parse_expression(0);
    if (invalid(first))
        print_span_error(file, lexer.token.span, "Expression expected.");

    statement->loop_statement.assignment_statement = add_statement();
    Assignment_Statement* assignment = &lookup(statement->loop_statement.assignment_statement)->assignment_statement;

    add_to(&assignment->lhs, first);
    parse_assignment_statement(assignment);

    parse_statements(&statement->loop_statement.body);
    expect(Token_Tag_end, "Missing end keyword in for loop.");

    statement->tag = Statement_Tag_loop;

    return statement_handle;
}

static Statement_Handle parse_while_statement(void)
{
    Statement_Handle statement_handle = add_statement();
    Statement* statement = lookup(statement_handle);

    statement->loop_statement.declaration_statement = parse_declaration_statement();
    if (!invalid(statement->loop_statement.declaration_statement))
        expect(Token_Tag_semicolon, "; expected after declaration statement.");

    statement->loop_statement.condition = parse_expression(0);
    if (invalid(statement->loop_statement.condition))
    {
        if (invalid(statement->loop_statement.declaration_statement))
            print_span_error(file, lexer.token.span, "Declaration or condition expected.");
        else
            print_span_error(file, lexer.token.span, "Condition expected.");
    }
    
    parse_statements(&statement->loop_statement.body);
    expect(Token_Tag_end, "Missing end keyword in while loop.");

    statement->loop_statement.assignment_statement = invalid_statement_handle;
    statement->tag = Statement_Tag_loop;

    return statement_handle;
}

static Statement_Handle parse_break_statement(void)
{
    Statement_Handle statement_handle = add_statement();
    
    Statement* statement = lookup(statement_handle);
    statement->tag = Statement_Tag_break;
    statement->break_statement.span = lexer.token.span;

    next_token();

    return statement_handle;
}

static Statement_Handle parse_continue_statement(void)
{
    Statement_Handle statement_handle = add_statement();
    
    Statement* statement = lookup(statement_handle);
    statement->tag = Statement_Tag_continue;
    statement->continue_statement.span = lexer.token.span;

    next_token();

    return statement_handle;
}

static Statement_Handle parse_return_statement(void)
{
    Statement_Handle statement_handle = add_statement();
    
    Statement* statement = lookup(statement_handle);
    statement->return_statement.span = lexer.token.span;

    next_token();
    parse_expressions(&statement->return_statement.values);

    if (statement->return_statement.values.handles_length)
    {
        Expression_Handle last_handle = handle_at(&statement->return_statement.values, statement->return_statement.values.handles_length - 1);
        statement->return_statement.span.end = lookup(last_handle)->span.end;
    }

    statement->tag = Statement_Tag_return;
    
    return statement_handle;
}

static Variable_Handle parse_variable(bool first_variable)
{
    Type_Reference_Handle type_reference;
    if (!parse_type(&type_reference, true))
    {
        if (first_variable)
            return invalid_variable_handle;
        else
            print_span_error(file, lexer.token.span, "Variable type expected.");
    }

    Variable_Handle variable_handle = add_variable();
    Variable* variable = lookup(variable_handle);

    variable->type_reference = type_reference;
    variable->name = lexer.token.interned;
    variable->name_span = lexer.token.span;
    if (advance_if(Token_Tag_lower_case_identifier))
        ;
    else if (advance_if(Token_Tag_blank_identifier))
        variable->is_blank = true;
    else
        print_span_error(file, lexer.token.span, "Variable name expected.");

    return variable_handle;
}

static Statement_Handle parse_declaration_statement(void)
{
    Variable_Handle first = parse_variable(true);
    if (invalid(first))
        return invalid_statement_handle;

    Statement_Handle statement_handle = add_statement();
    Statement* statement = lookup(statement_handle);
    statement->tag = Statement_Tag_declaration;

    add_to(&statement->declaration_statement.variables, first);
    while (advance_if(Token_Tag_comma))
        add_to(&statement->declaration_statement.variables, parse_variable(false));
        
    if (advance_if(Token_Tag_equal))
    {
        parse_expressions(&statement->declaration_statement.expressions);
        if (!statement->declaration_statement.expressions.handles_length)
            print_span_error(file, lexer.token.span, "Expression expected.");
    }
    
    return statement_handle;
}

static void parse_statements(Statement_Handle_Array* body)
{
    while (true)
    {
        Statement_Handle statement_handle;
        bool last_statement = false;

        if (is_type_ahead())
            statement_handle = parse_declaration_statement();
        else
        {
            Expression_Handle expression_handle = parse_expression(0);
            if (invalid(expression_handle))
            {
                if (advance_if(Token_Tag_if))
                    statement_handle = parse_if_statement();
                else if (advance_if(Token_Tag_for))
                    statement_handle = parse_for_statement();
                else if (advance_if(Token_Tag_while))
                    statement_handle = parse_while_statement();
                else if (lexer.token.tag == Token_Tag_version)
                    not_implemented(__FILE__, __LINE__, "Token_Tag_version");
                else if (lexer.token.tag == Token_Tag_break)
                {
                    statement_handle = parse_break_statement();
                    last_statement = true;
                }
                else if (lexer.token.tag == Token_Tag_continue)
                {
                    statement_handle = parse_continue_statement();
                    last_statement = true;
                }
                else if (lexer.token.tag == Token_Tag_return)
                {
                    statement_handle = parse_return_statement();
                    last_statement = true;
                }
                else
                    return;
            }
            else
            {
                statement_handle = add_statement();
                Statement* statement = lookup(statement_handle);

                if (lexer.token.tag == Token_Tag_comma || token_is_assignment_operator())
                {
                    add_to(&statement->assignment_statement.lhs, expression_handle);
                    parse_assignment_statement(&statement->assignment_statement);
                    statement->tag = Statement_Tag_assignment;
                }
                else
                {
                    statement->expression_statement = expression_handle;
                    statement->tag = Statement_Tag_expression;
                }
            }
        }

        add_to(body, statement_handle);
        if (last_statement)
            return;
    }
}

static bool parse_function_declaration(bool is_public)
{
    if (!advance_if(Token_Tag_function))
        return false;

    Function_Handle function_handle = add_function();
    Function* function = lookup(function_handle);
    function->file = file_handle;
    function->name = lexer.token.interned;
    function->name_span = lexer.token.span;
    function->template = invalid_function_handle;
    function->ir = invalid_ir_function_handle;

    if (is_public)
        function->attributes = Function_Attribute_is_public;

    expect(Token_Tag_lower_case_identifier, "Function name expected.");
    if (advance_if(Token_Tag_left_square_bracket))
    {
        do
        {
            Type_Reference_Handle type_reference_handle;
            if (!parse_type(&type_reference_handle, true))
                print_span_error(file, lexer.token.span, "Formal type parameter expected.");

            Type_Reference* type_reference = lookup(type_reference_handle);
            if (type_reference->tag != Type_Reference_Tag_name)
                print_span_error(file, type_reference->span, "Formal type parameter expected.");

            add_to(&function->formal_type_parameters, type_reference_handle);
        } while (advance_if(Token_Tag_comma));

        expect(Token_Tag_right_square_bracket, "] expected.");
        function->attributes |= Function_Attribute_is_polymorphic;
    }

    expect(Token_Tag_left_parentheses, "( expected.");
    if (lexer.token.tag != Token_Tag_right_parentheses)
    {
        do
        {
            Formal_Parameter_Handle parameter_handle = add_formal_parameter();
            Formal_Parameter* parameter = lookup(parameter_handle);
            if (!parse_type(&parameter->type, true))
                print_span_error(file, lexer.token.span, "Formal parameter type expected.");

            parameter->name = lexer.token.interned;
            parameter->name_span = lexer.token.span;
            expect(Token_Tag_lower_case_identifier, "Formal parameter name expected.");

            add_to(&function->formal_parameters, parameter_handle);
        } while (advance_if(Token_Tag_comma));
    }

    expect(Token_Tag_right_parentheses, ") expected.");
    if (advance_if(Token_Tag_colon))
        parse_types(&function->return_types, "Return");

    parse_statements(&function->body);
    expect(Token_Tag_end, "Missing end keyword in function.");

    add_to(&file->function_declarations, function_handle);

    return true;
}

static void parse_field(Compound_Type* compound)
{
    Field_Handle field_handle = add_field(); 
    Field* field = lookup(field_handle);

    if (!parse_type(&field->type_reference, true))
        print_span_error(file, lexer.token.span, "Field type expected.");

    field->type = invalid_type_handle;
    field->name = lexer.token.interned;
    field->name_span = lexer.token.span;
    expect(Token_Tag_lower_case_identifier, "Field name expected.");

    add_to(&compound->fields, field_handle);
}

static bool parse_compound_declaration(bool is_public)
{
    if (lexer.token.tag != Token_Tag_structure && lexer.token.tag != Token_Tag_union)
        return false;
    
    Type_Handle handle = add_type();
    Compound_Type* compound = &lookup(handle)->compound;

    compound->tag = Type_Tag_compound;
    compound->file = file_handle;
    compound->template = invalid_type_handle;
    if (lexer.token.tag == Token_Tag_union)
        compound->attributes |= Compound_Attribute_is_union;

    if (is_public)
        compound->attributes |= Compound_Attribute_is_public;

    // Skip structure/union keyword.
    next_token();

    compound->name = lexer.token.interned;
    compound->name_span = lexer.token.span;

    next_token();

    Lexer start = lexer;
    if (advance_if(Token_Tag_left_square_bracket))
    {
        if (lexer.token.tag == Token_Tag_integer || lexer.token.tag == Token_Tag_right_square_bracket)
            lexer = start;
        else
        {
            do
            {
                Type_Reference_Handle type_reference_handle;
                if (!parse_type(&type_reference_handle, false))
                    print_span_error(file, lexer.token.span, "Formal type parameter expected.");

                Type_Reference* type_reference = lookup(type_reference_handle);
                if (type_reference->tag != Type_Reference_Tag_name)
                    print_span_error(file, type_reference->span, "Formal type parameter expected.");

                add_to(&compound->formal_type_parameters, type_reference_handle);
            } while (advance_if(Token_Tag_comma));

            expect(Token_Tag_right_square_bracket, "] expected.");
            compound->attributes |= Compound_Attribute_is_polymorphic;
        }
    }

    do
        parse_field(compound);
    while (lexer.token.tag == Token_Tag_upper_case_identifier);

    expect(Token_Tag_end, "Missing end keyword in compound.");
    add_to(&file->type_declarations, handle);

    return true;
}

static bool parse_enumeration_declaration(bool is_public)
{
    if (!advance_if(Token_Tag_enumeration))
        return false;

    Type_Handle handle = add_type();
    Enumeration_Type* enumeration = &lookup(handle)->enumeration;

    enumeration->tag = Type_Tag_enumeration;
    enumeration->file = file_handle;
    enumeration->is_public = is_public;
    enumeration->name = lexer.token.interned;
    enumeration->name_span = lexer.token.span;

    expect(Token_Tag_upper_case_identifier, "Type expected.");
    expect(Token_Tag_colon, ": expected.");

    if (!parse_type(&enumeration->underlying_type_reference, true))
        print_span_error(file, lexer.token.span, "Type expected.");
   
    Type_Reference* underlying_type_reference = lookup(enumeration->underlying_type_reference);
    if (underlying_type_reference->tag != Type_Reference_Tag_name)
        print_span_error(file, underlying_type_reference->span, "Type expected.");

    while (lexer.token.tag == Token_Tag_lower_case_identifier)
    {
        Constant_Handle constant_handle = add_constant();
        Constant* constant = lookup(constant_handle);
        constant->name = lexer.token.interned;
        constant->name_span = lexer.token.span;
        constant->explicit_value = invalid_expression_handle;

        next_token();
        if (advance_if(Token_Tag_equal))
        {
            constant->explicit_value = parse_expression(0);
            if (invalid(constant->explicit_value))
                print_span_error(file, lexer.token.span, "Expression expected.");
        }

        add_to(&enumeration->constants, constant_handle);
    }

    expect(Token_Tag_end, "Missing end keyword in enumeration.");
    add_to(&file->type_declarations, handle);

    return true;
}

static bool parse_version_declaration(void)
{
    if (!advance_if(Token_Tag_version))
        return false;

    not_implemented(__FILE__, __LINE__, "Parsing version.");

    return true;
}

static void parse_declaration(void)
{
    bool is_public = advance_if(Token_Tag_public);
    if (is_public)
    {
        if (!parse_function_declaration(is_public) &&
            !parse_compound_declaration(is_public) &&
            !parse_enumeration_declaration(is_public))
            print_span_error(file, lexer.token.span, "Function, external function, enumeration, structure or union expected.");
    }
    else if (!parse_function_declaration(is_public) &&
             !parse_compound_declaration(is_public) &&
             !parse_enumeration_declaration(is_public) &&
             !parse_version_declaration())
        print_span_error(file, lexer.token.span, "Function, external function, enumeration, structure, union, version or proposition expected.");
}

void parse_uses(void)
{
    while (advance_if(Token_Tag_use))
    {
        if (lexer.token.tag != Token_Tag_upper_case_identifier)
            print_span_error(file, lexer.token.span, "Namespace expected.");

        bool already_used = false;
        for (uint8_t i = 0; i < file->uses.handles_length; i++)
        {
            Namespace* use = lookup_in(&file->uses, i);
            if (compare(lexer.token.interned, use->name))
            {
                already_used = true;
                break;
            }
        }

        if (!already_used)
        {
            Namespace_Handle handle = add_namespace();
            Namespace* use = lookup(handle);
            use->name_span = lexer.token.span;
            use->name = lexer.token.interned;

            add_to(&file->uses, handle);
        }

        next_token();
    }
}

static void parse_namespace(void)
{
    expect(Token_Tag_namespace, "Namespace declaration expected.");

    Namespace_Handle handle = add_namespace();
    file->namespace = handle;

    Namespace* namespace = lookup(handle);
    namespace->name = lexer.token.interned;
    namespace->name_span = lexer.token.span;

    expect(Token_Tag_upper_case_identifier, "Namespace expected.");
}

void parse_file(char* path)
{
    file_handle = add_file();
    file = lookup(file_handle);
    file->path = path;

    init_lexer(path);
    if (lexer.token.tag == Token_Tag_eof)
        print_error("\"%s\" is empty.", file->path);

    parse_namespace();
    parse_uses();

    while (lexer.token.tag != Token_Tag_eof)
        parse_declaration();
}
