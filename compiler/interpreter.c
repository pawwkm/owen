#include <stdlib.h>
#include <stdio.h>

#include "ast.h"
#include "semantics.h"

void interpretExpression(Expression* expression)
{
    switch (expression->tag)
    {
        case EXPRESSION_NUMBER:
        {
            if (expression->ctfeValue == NULL)
            {
                expression->ctfeValue = malloc(sizeof(int32_t));
                *(int32_t *)expression->ctfeValue = atoi(expression->number.value.start);
            }

            break;
        }
    }
}

void interpretStatements(StatementList* statements, Scope* scope, ExpressionList* output)
{
    for (int32_t i = 0; i < statements->count; i++)
    {
        Statement* statement = &statements->elements[i];
        switch (statement->tag)
        {
            case STATEMENT_RETURN:
            {
                if (statement->value.tag != EXPRESSION_NONE)
                {
                    interpretExpression(&statement->value);
                    appendExpression(output, statement->value);
                }

                return;
            }
        }
    }
}

void interpretFunction(Scope* file, FunctionDeclaration* declaration, ExpressionList* output)
{
    if (!declaration->analyzed)
        analyzeFunction(file, declaration);

    interpretStatements(&declaration->body, &declaration->scope, output);
}

ExpressionList interpret(Scope* file, Scope* local, Expression* expression)
{
    switch (expression->tag)
    {
        case EXPRESSION_CALL:
        {
            ExpressionList output = NEW_LIST();
            Symbol* symbol = resolve(local, expression->call.name.value);

            interpretFunction(file, symbol->type.function.declaration, &output);

            // Convert the ctfe value to a lexeme
            return output;
        }
    }
}
