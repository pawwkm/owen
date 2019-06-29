#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "ast.h"
#include "parser.h"
#include "errors.h"
#include "string.h"

void statements(Source* source, StringList* table, StatementList* list);

char* keywords[] =
{
    "namespace",
    "use",
    "public",
    "function",
    "input",
    "output",
    "end",
    "if",
    "else",
    "for",
    "each",
    "in",
    "while",
    "break",
    "structure",
    "proposition",
    "enumeration",
    "of",
    "size",
    "union",
    "return",
    "ctfe",
    "true",
    "false",
    0
};

DECLARE_LIST_APPEND(Slice)
DECLARE_LIST_APPEND(FunctionDeclaration)
DECLARE_LIST_APPEND(Identifier)
DECLARE_LIST_APPEND(File)
DECLARE_LIST_APPEND(Statement)
DECLARE_LIST_APPEND(Expression)

void appendConditionalBlock(ConditionalBlockList* list, ConditionalBlock element)
{
    if (list->count == list->capacity)
    {
        list->capacity = list->capacity < 4 ? 4 : list->capacity * 2;
        list->elements = (ConditionalBlock *)realloc(list->elements, list->capacity * sizeof(ConditionalBlock));
    }

    list->elements[list->count++] = element;
}

void whitespace(Source* source)
{
    while (*source->current)
    {
        switch (*source->current)
        {
            case ' ':
            case '\n':
            case '\t':
                source->current++;
            break;
            case '/':
                if (*(source->current + 1) == '/')
                {
                    while (*source->current != '\n')
                        source->current++;
                }
                break;
            default:
                return;
        }
    }
}

bool literal(Source* source, char* literal)
{
    size_t length = strlen(literal);
    if (!strncmp(source->current, literal, length))
    {
        source->current += length;
        whitespace(source);

        return true;
    }
    else
        return false;
}

bool identifier(Source* source, StringList* table, Identifier* identifier)
{
    bool isKeyword(char* start, size_t length)
    {
        int32_t i = 0;
        while (true)
        {
            char* keyword = keywords[i];
            if (!keyword)
                break;

            if (length == strlen(keyword) && !strncmp(start, keyword, length))
                return true;

            i++;
        }

        return false;
    }

    Slice slice;
    slice.start = source->current;

    if (isalpha(*source->current))
    {
        identifier->position = *source;
        while (isalpha(*source->current) || isdigit(*source->current))
            source->current++;

        if (isKeyword(slice.start, source->current - slice.start))
        {
            source->current = slice.start;

            return false;
        }
        else
        {
            slice.length = source->current - slice.start;
            identifier->value = internSlice(table, &slice);
            whitespace(source);
        }
    }

    return true;
}

void number(Source* source, Expression* e)
{
    e->tag = EXPRESSION_NUMBER;
    e->number = (Number)
    {
        .tag = NUMBER_INTEGER_TO_BE_INFERRED,
        .value = (Slice)
        {
            .start = NULL,
            .length = 0
        }
    };

    e->number.value.start = source->current;
    while (isdigit(*source->current))
        source->current++;

    if (literal(source, "."))
    {
        if (isdigit(*source->current))
        {
            while (isdigit(*source->current))
                source->current++;

            e->number.value.length = source->current - e->number.value.start;
            if (literal(source, "f32"))
                e->number.tag = NUMBER_F32;
            else if (literal(source, "f64"))
                e->number.tag = NUMBER_F64;
            else
                e->number.tag = NUMBER_FLOAT_TO_BE_INFERRED;
        }
        else
            errorAt(source, "Expected decimal part of the float.");
    }
    else
    {
        e->number.value.length = source->current - e->number.value.start;
        if (literal(source, "i8"))
            e->number.tag = NUMBER_I8;
        else if (literal(source, "i16"))
            e->number.tag = NUMBER_I16;
        else if (literal(source, "i32"))
            e->number.tag = NUMBER_I32;
        else if (literal(source, "i64"))
            e->number.tag = NUMBER_I64;
        else if (literal(source, "u8"))
            e->number.tag = NUMBER_U8;
        else if (literal(source, "u16"))
            e->number.tag = NUMBER_U16;
        else if (literal(source, "u32"))
            e->number.tag = NUMBER_U32;
        else if (literal(source, "u64"))
            e->number.tag = NUMBER_U64;
    }

    e->number.position.path = source->path;
    e->number.position.contents = source->contents;
    e->number.position.current = e->number.value.start;

    whitespace(source);
}

bool callExpression(Source* source, StringList* table, Expression* expression)
{
    char* start = source->current;
    Identifier name;

    if (identifier(source, table, &name))
    {
        if (literal(source, "("))
        {
            if (literal(source, ")"))
            {
                expression->tag = EXPRESSION_CALL;
                expression->call.name = name;

                return true;
            }
            else
                errorAt(source, ") expected.");
        }
    }

    source->current = start;

    return false;
}

void expression(Source* source, StringList* table, Expression* value)
{
    char* start = source->current;
    if (literal(source, "ctfe"))
    {
        while (literal(source, "ctfe"))
            ;

        value->tag = EXPRESSION_CTFE;
        value->expression = malloc(sizeof(Expression));
        expression(source, table, value->expression);
    }
    else if (callExpression(source, table, value))
        ;
    else if (isdigit(*source->current))
        number(source, value);
    else if (literal(source, "true"))
    {
        value->tag = EXPRESSION_BOOL;
        value->boolean.value = true;
        value->boolean.position = *source;
        value->boolean.position.current = start;
    }
    else if (literal(source, "false"))
    {
        value->tag = EXPRESSION_BOOL;
        value->boolean.value = false;
        value->boolean.position = *source;
        value->boolean.position.current = start;
    }
    else if (isalpha(*source->current))
    {
        if (identifier(source, table, &value->identifier))
            value->tag = EXPRESSION_IDENTIFIER;
        else
            value->tag = EXPRESSION_NONE;
    }
    else
        value->tag = EXPRESSION_NONE;

    value->ctfeValue = NULL;
}

bool assignmentStatement(Source* source, StringList* table, Statement* statement)
{
    char* start = source->current;
    statement->assignment.variables = (ExpressionList) NEW_LIST();
    Expression e;

    expression(source, table, &e);
    if (e.tag == EXPRESSION_NONE)
        return false;
    else
    {
        appendExpression(&statement->assignment.variables, e);
        while (literal(source, ","))
        {
            expression(source, table, &e);
            if (e.tag == EXPRESSION_NONE)
                errorAt(source, "Expression expected.");

            appendExpression(&statement->assignment.variables, e);
        }

        if (literal(source, "="))
        {
            statement->assignment.values = (ExpressionList) NEW_LIST();

            expression(source, table, &e);
            if (e.tag == EXPRESSION_NONE)
                errorAt(source, "Expression expected.");

            appendExpression(&statement->assignment.values, e);
            while (literal(source, ","))
            {
                expression(source, table, &e);
                if (e.tag == EXPRESSION_NONE)
                    errorAt(source, "Expression expected.");

                appendExpression(&statement->assignment.values, e);
            }
        }
        else
        {
            source->current = start;
            return false;
        }

        statement->tag = STATEMENT_ASSIGNMENT;

        return true;
    }
}

void ifStatement(Source* source, StringList* table, Statement* statement)
{
    statement->tag = STATEMENT_IF;
    statement->ifs = malloc(sizeof(ConditionalBlockList));
    *statement->ifs = (ConditionalBlockList)NEW_LIST();

    ConditionalBlock block =
    {
        .body = NEW_LIST(),
        .scope = (Scope)
        {
            .parent = NULL,
            .symbols = NEW_LIST()
        }
    };

    expression(source, table, &block.condition);
    if (block.condition.tag == EXPRESSION_NONE)
        errorAt(source, "Boolean expression expected.");
    else
    {
        statements(source, table, &block.body);
        appendConditionalBlock(statement->ifs, block);
    }

    while (literal(source, "else"))
    {
        block = (ConditionalBlock)
        {
            .body = NEW_LIST(),
            .scope = (Scope)
            {
                .parent = NULL,
                .symbols = NEW_LIST()
            }
        };

        bool hasCondition = literal(source, "if");
        if (hasCondition)
        {
            expression(source, table, &block.condition);
            if (block.condition.tag == EXPRESSION_NONE)
                errorAt(source, "Boolean expression expected.");
        }
        else
            block.condition.tag == EXPRESSION_NONE;

        statements(source, table, &block.body);
        appendConditionalBlock(statement->ifs, block);

        if (!hasCondition)
            break;
    }

    if (!literal(source, "end"))
        errorAt(source, "end expected.");
}

void statements(Source* source, StringList* table, StatementList* list)
{
    while (true)
    {
        Statement statement;
        if (assignmentStatement(source, table, &statement))
            ;
        else if (literal(source, "if"))
            ifStatement(source, table, &statement);
        else if (literal(source, "return"))
        {
            expression(source, table, &statement.value);
            statement.tag = STATEMENT_RETURN;
        }
        else
        {
            Expression e;
            e.tag = EXPRESSION_NONE;
            expression(source, table, &e);

            if (e.tag == EXPRESSION_NONE)
                break;
            else
            {
                statement.tag = STATEMENT_EXPRESSION;
                statement.value = e;
            }
        }

        appendStatement(list, statement);
    }
}

void functionDeclaration(Source* source, File* file, StringList* table, bool isPublic)
{
    FunctionDeclaration function =
    {
        .isPublic = isPublic,
        .output = (IdentifierList) NEW_LIST(),
        .body = (StatementList) NEW_LIST(),
        .scope = (Scope)
        {
            .parent = NULL,
            .symbols = (SymbolList) NEW_LIST()
        },
        .analyzed = false
    };

    if (!identifier(source, table, &function.name))
        errorAt(source, "Function name expected.");

    if (literal(source, "output"))
    {
        Identifier type;
        if (!identifier(source, table, &type))
            errorAt(source, "Output type expected.");
        else
            appendIdentifier(&function.output, type);

        while (literal(source, ","))
        {
            if (!identifier(source, table, &type))
                errorAt(source, "Output type expected.");
            else
                appendIdentifier(&function.output, type);
        }
    }

    statements(source, table, &function.body);
    if (!literal(source, "end"))
        errorAt(source, "end expected.");

    appendFunctionDeclaration(&file->functions, function);
}

void declaration(Source* source, File* file, StringList* table)
{
    bool isPublic = literal(source, "public");
    if (literal(source, "function"))
        functionDeclaration(source, file, table, isPublic);
    else if (literal(source, "proposition"))
        errorAt(source, "Propositions not yet implemented.");
    else if (literal(source, "structure"))
        errorAt(source, "Structures not yet implemented.");
    else if (literal(source, "union"))
        errorAt(source, "Unions not yet implemented.");
    else if (literal(source, "enumeration"))
        errorAt(source, "Enumeration not yet implemented.");
    else
        errorAt(source, "Expected a function, proposition, structure, union or enumeration.");
}

void directives(Source* source, File* file, StringList* table)
{
    char* qualifiedIdentifier(Source* source, StringList* table)
    {
        StringBuilder builder = NEW_LIST();

        Identifier part;
        if (!identifier(source, table, &part))
            errorAt(source, "Identifier expected.");

        buildFromString(&builder, part.value);
        while (literal(source, "."))
        {
            if (!identifier(source, table, &part))
                errorAt(source, "Identifier expected.");

            buildFromString(&builder, ".");
            buildFromString(&builder, part.value);
        }

        char* qualified = terminateBuilder(&builder);

        return internString(table, qualified);
    }

    if (literal(source, "namespace"))
        file->namespace = qualifiedIdentifier(source, table);
    else
        errorAt(source, "namespace expected.");

    while (literal(source, "use"))
        appendString(&file->uses, qualifiedIdentifier(source, table));
}

void file(Source* source, File* file, StringList* table)
{
    whitespace(source);

    directives(source, file, table);
    while (*source->current)
        declaration(source, file, table);
}

Program parse(SourceList* sources)
{
    Program program =
    {
        .interned = (StringList) NEW_LIST(),
        // @Refactor Thomas said it was probably a size of error that caused the calloc fail.
        .files = (FileList) NEW_LIST()
    };

    for (int32_t i = 0; i < sources->count; i++)
    {
        File f;
        memset(&f, 0, sizeof(File));

        file(&sources->elements[i], &f, &program.interned);
        appendFile(&program.files, f);
    }

    return program;
}
