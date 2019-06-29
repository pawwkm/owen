#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "ast.h"
#include "string.h"
#include "errors.h"
#include "semantics.h"

void generateExpression(Expression* expression, StringBuilder* c)
{
    switch (expression->tag)
    {
        case EXPRESSION_CALL:
            buildFromString(c, expression->identifier.value);
            buildFromString(c, "()");
            break;
        case EXPRESSION_VARIABLE_REFERENCE:
        case EXPRESSION_VARIABLE_DECLARATION:
        case EXPRESSION_IDENTIFIER:
            buildFromString(c, expression->identifier.value);
            break;
        case EXPRESSION_NUMBER:
            buildFromSlice(c, &expression->number.value);
            break;
        case EXPRESSION_BOOL:
            if (expression->boolean.value)
                buildFromString(c, "true");
            else
                buildFromString(c, "false");
        default:
            break;
    }
}

void generateStatements(Scope*scope, StatementList* statements, StringBuilder* c)
{
    for (int32_t s = 0; s < statements->count; s++)
    {
        Statement* statement = &statements->elements[s];
        switch (statement->tag)
        {
            case STATEMENT_ASSIGNMENT:
                if (statement->assignment.variables.count > 1)
                    error("Multiple assignment not yet supported.");

                if (statement->assignment.variables.elements[0].tag == EXPRESSION_VARIABLE_DECLARATION)
                {
                    Symbol* symbol = resolve(scope, statement->assignment.variables.elements[0].identifier.value);
                    switch (symbol->type.tag) {
                        case TYPE_FUNCTION:
                        case TYPE_ENUMERATION:
                        case TYPE_STRUCTURE:
                        case TYPE_UNION:
                            error("Only primitives are supported.");
                            break;
                        default:
                            buildFromString(c, typeToString(&symbol->type));
                            buildFromString(c, " ");
                            break;
                    }
                }

                buildFromString(c, statement->assignment.variables.elements[0].identifier.value);
                buildFromString(c, " = ");
                generateExpression(&statement->assignment.values.elements[0], c);
                buildFromString(c, ";\n");
                break;
            case STATEMENT_RETURN:
                buildFromString(c, "return ");
                generateExpression(&statement->value, c);
                buildFromString(c, ";\n");
                break;
            case STATEMENT_EXPRESSION:
                generateExpression(&statement->value, c);
                buildFromString(c, ";\n");
                break;
            case STATEMENT_IF:
                for (int32_t i = 0; i < statement->ifs->count; i++)
                {
                    ConditionalBlock* block = &statement->ifs->elements[i];
                    if (i == 0)
                        buildFromString(c, "if (");
                    else if (block->condition.tag != EXPRESSION_NONE)
                        buildFromString(c, "else if (");
                    else
                        buildFromString(c, "else");

                    if (block->condition.tag != EXPRESSION_NONE)
                    {
                        generateExpression(&block->condition, c);
                        buildFromString(c, ")");
                    }

                    buildFromString(c, " {\n");

                    generateStatements(&block->scope, &block->body, c);

                    buildFromString(c, "}\n");
                }
                break;
            case STATEMENT_WHILE:
            {
                ConditionalBlock* block = statement->whileBlock;
                buildFromString(c, "while (");
                generateExpression(&block->condition, c);
                buildFromString(c, ") {\n");

                generateStatements(&block->scope, &block->body, c);

                buildFromString(c, "}\n");
                break;
            }
        }
    }
}

void generateSignature(FunctionDeclaration* declaration, StringBuilder* c)
{
    if (declaration->output.count == 0)
        buildFromString(c, "void ");
    else if (declaration->output.count == 1)
    {
        buildFromString(c, declaration->output.elements[0].value);
        buildFromString(c, " ");
    }

    buildFromString(c, declaration->name.value);
    buildFromString(c, "()");
}

void generatePrototypes(Program* program, StringBuilder* c)
{
    for (int32_t i = 0; i < program->files.count; i++)
    {
        File* file = &program->files.elements[i];
        for (int32_t f = 0; f < file->functions.count; f++)
        {
            FunctionDeclaration* declaration = &file->functions.elements[f];
            generateSignature(declaration, c);
            buildFromString(c, ";\n\n");
        }
    }
}

void generateFunctions(Program* program, StringBuilder* c)
{
    for (int32_t i = 0; i < program->files.count; i++)
    {
        File* file = &program->files.elements[i];
        for (int32_t f = 0; f < file->functions.count; f++)
        {
            FunctionDeclaration* declaration = &file->functions.elements[f];
            generateSignature(declaration, c);
            buildFromString(c, " {\n");

            generateStatements(&declaration->scope, &declaration->body, c);

            buildFromString(c, "}\n\n");
        }
    }
}

void callCCompiler(char* code, char* output)
{
    FILE* out = fopen("out.c", "w");
    fputs(code, out);
    fclose(out);

    printf("%s\n", code);
    StringBuilder command =
    {
        .string = NULL,
        .count = 0,
        .capacity = 0
    };

    buildFromString(&command, "/usr/bin/clang out.c -o ");
    buildFromString(&command, output);

    system(terminateBuilder(&command));

    remove("out.c");
}

void generate(Program* program, char* output)
{
    StringBuilder c =
    {
        .string = NULL,
        .count = 0,
        .capacity = 0
    };

    buildFromString(&c, "#include <stdbool.h>\n"
                        "#include <stdint.h>\n\n"

                        "typedef int8_t i8;\n"
                        "typedef int16_t i16;\n"
                        "typedef int32_t i32;\n"
                        "typedef int64_t i64;\n"
                        "typedef uint8_t u8;\n"
                        "typedef uint16_t u16;\n"
                        "typedef uint32_t u32;\n"
                        "typedef uint64_t u64;\n\n");

    generatePrototypes(program, &c);
    generateFunctions(program, &c);

    callCCompiler(terminateBuilder(&c), output);
}
