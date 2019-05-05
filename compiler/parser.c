#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include "parser.h"

typedef enum
{
    TokenType_Comma,
    TokenType_Namespace,
    TokenType_Use,
    TokenType_Public,
    TokenType_Function,
    TokenType_Input,
    TokenType_Output,
    TokenType_End,
    TokenType_If,
    TokenType_Else,
    TokenType_For,
    TokenType_Each,
    TokenType_In,
    TokenType_While,
    TokenType_Break,
    TokenType_Structure,
    TokenType_Proposition,
    TokenType_Enumeration,
    TokenType_Of,
    TokenType_Size,
    TokenType_Union,
    TokenType_Identifier,
    TokenType_LeftParenthesis,
    TokenType_RightParenthesis,
    TokenType_AssignmentOperator
} TokenType;

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
    0
};

Slice identifier(Source* source);

bool whitespace(Source* source)
{
    bool skipped = false;
    while (source->current)
    {
        switch (*source->current)
        {
            case ' ':
            case '\n':
            case '\t':
                skipped = true;
                source->current++;
            break;
            default:
                return skipped;
        }
    }

    return skipped;
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

bool contains(TokenType* list, int32_t listLength, TokenType type)
{
    for (int32_t i = 0; i < listLength; i++)
    {
        if (list[i] == type)
            return true;
    }

    return false;
}

void skip(Source* source)
{
    const char* delimiters = " \n\t";
    while (source->current && strchr(delimiters, source->current[0]) == NULL)
        source->current++;

    whitespace(source);
}

TokenType peek(Source* source)
{
    char* start = source->current;
    TokenType type = TokenType_Of;

    if (identifier(source).length != 0)
        type = TokenType_Identifier;
    else if (literal(source, "function"))
        type = TokenType_Function;
    else if (literal(source, "proposition"))
        type = TokenType_Proposition;
    else if (literal(source, "structure"))
        type = TokenType_Structure;
    else if (literal(source, "union"))
        type = TokenType_Union;
    else if (literal(source, "enumeration"))
        type = TokenType_Enumeration;
    else if (literal(source, "end"))
        type = TokenType_End;
    else if (literal(source, "("))
        type = TokenType_LeftParenthesis;
    else if (literal(source, ")"))
        type = TokenType_RightParenthesis;
    else if (literal(source, "="))
        type = TokenType_AssignmentOperator;

    source->current = start;

    return type;
}

void panicIfUnsynchronized(Source* source, TokenType* admissible, int32_t admissibleLength, TokenType* stops, int32_t stopsLength, DiagnosticList* diagnostics, char* description)
{
    TokenType next = peek(source);
    if (!contains(admissible, admissibleLength, next))
    {
        appendDiagnostic(diagnostics, (Diagnostic)
        {
            .description = description,
            .occurredAt = positionOf(source)
        });

        while (*source->current && (!contains(admissible, admissibleLength, next) || !contains(stops, stopsLength, next)))
        {
            skip(source);
            next = peek(source);
        }
    }
}

Access accessibility(Source* source)
{
    if (literal(source, "public"))
        return ACCESS_PUBLIC;
    else
        return ACCESS_PRIVATE;
}

Slice identifier(Source* source)
{
    bool isKeyword(char* start, size_t length)
    {
        int32_t i = 0;
        while (true)
        {
            char* keyword = keywords[i];
            if (!keyword)
                break;

            if (!strncmp(start, keyword, length) && length == strlen(keyword))
                return true;

            i++;
        }

        return false;
    }

    char* start = source->current;
    char* end = start;

    if (isalpha(*source->current))
    {
        while (isalpha(*source->current) || isdigit(*source->current))
            source->current++;

        end = source->current;
        if (isKeyword(start, end - start))
        {
            end = start;
            source->current = start;
        }
        else
            whitespace(source);
    }

    return (Slice)
    {
        .start = start,
        .length = end - start
    };
}

char* qualifiedIdentifierToString(const SliceList* list)
{
    int32_t length = list->count - 1;
    for (int32_t i = 0; i < list->count; i++)
        length += list->elements[i].length;

    char* string = malloc(length + 1);

    int32_t index = 0;
    for (int32_t i = 0; i < list->count; i++)
    {
        Slice slice = list->elements[i];
        strncpy(string + index, slice.start, slice.length);

        index += slice.length;
        string[index] = '.';
        index++;
    }

    string[length] = 0;

    return string;
}

char* qualifiedIdentifier(Source* source, DiagnosticList* diagnostics)
{
    Slice first = identifier(source);
    if (first.length != 0)
    {
        SliceList* list = calloc(1, sizeof(SliceList));
        appendSlice(list, first);

        while (literal(source, "."))
        {
            Slice next = identifier(source);
            if (next.length == 0)
            {
                appendDiagnostic(diagnostics, (Diagnostic)
                {
                    .description = "Identifier expected.",
                    .occurredAt = positionOf(source)
                });

                break;
            }
            else
                appendSlice(list, next);
        }

        return qualifiedIdentifierToString(list);
    }

    return NULL;
}

char* namespaceDirective(Source* source, DiagnosticList* diagnostics)
{
    if (!literal(source, "namespace"))
    {
        appendDiagnostic(diagnostics, (Diagnostic)
        {
            .description = "Namespace expected.",
            .occurredAt = positionOf(source)
        });

        return NULL;
    }

    char* namespace =  qualifiedIdentifier(source, diagnostics);
    if (!namespace)
    {
        appendDiagnostic(diagnostics, (Diagnostic)
        {
            .description = "Qualified identifier expected.",
            .occurredAt = positionOf(source)
        });
    }

    return namespace;
}

char* useDirective(Source* source, DiagnosticList* diagnostics)
{
    if (!literal(source, "use"))
        return NULL;

    char* namespace =  qualifiedIdentifier(source, diagnostics);
    if (!namespace)
    {
        appendDiagnostic(diagnostics, (Diagnostic)
        {
            .description = "Qualified identifier expected.",
            .occurredAt = positionOf(source)
        });
    }

    return namespace;
}

StringList useDirectives(Source* source, DiagnosticList* diagnostics)
{
    StringList directives =
    {
        .elements = NULL,
        .count = 0,
        .capacity = 0
    };

    char* next = useDirective(source, diagnostics);
    if (next)
    {
        appendString(&directives, next);
        while (next = useDirective(source, diagnostics))
            appendString(&directives, next);
    }

    return directives;
}

Number number(Source* source)
{
    Number number =
    {
        .tag = NUMBER_INTEGER_TO_BE_INFERRED,
        .value = (Slice)
        {
            .start = NULL,
            .length = 0
        }
    };

    if (isdigit(*source->current))
    {
        number.value.start = source->current;
        while (isdigit(*source->current))
            source->current++;

        if (literal(source, "."))
        {
            if (isdigit(*source->current))
            {
                while (isdigit(*source->current))
                    source->current++;

                number.value.length = source->current - number.value.start;
                if (literal(source, "f32"))
                    number.tag = NUMBER_F32;
                else if (literal(source, "f64"))
                    number.tag = NUMBER_F64;
                else
                    number.tag = NUMBER_FLOAT_TO_BE_INFERRED;
            }
            else
            {
                // decimal part expected. PANIC!
            }
        }
        else
        {
            number.value.length = source->current - number.value.start;
            if (literal(source, "i8"))
                number.tag = NUMBER_I8;
            else if (literal(source, "i16"))
                number.tag = NUMBER_I16;
            else if (literal(source, "i32"))
                number.tag = NUMBER_I32;
            else if (literal(source, "i64"))
                number.tag = NUMBER_I64;
            else if (literal(source, "u8"))
                number.tag = NUMBER_U8;
            else if (literal(source, "u16"))
                number.tag = NUMBER_U16;
            else if (literal(source, "u32"))
                number.tag = NUMBER_U32;
            else if (literal(source, "u64"))
                number.tag = NUMBER_U64;
        }
    }

    return number;
}

Expression expression(Source* source)
{
    Expression value;
    if ((value.identifier = identifier(source)).length != 0)
        value.tag = EXPRESSION_IDENTIFIER;
    else if ((value.number = number(source)).value.length != 0)
        value.tag = EXPRESSION_NUMBER;
    else
        value.tag = EXPRESSION_NULL;

    return value;
}

ExpressionList expressionList(Source* source)
{
    ExpressionList list =
    {
        .elements = NULL,
        .count = 0,
        .capacity = 0
    };

    Expression next = expression(source);
    if (next.tag != EXPRESSION_NULL)
    {
        appendExpression(&list, next);
    }

    return list;
}

bool assignmentStatement(Source* source, StatementList* statements, DiagnosticList* diagnostics)
{
    char* start = source->current;

    ExpressionList variables = expressionList(source);
    if (variables.count && literal(source, "="))
    {
        appendStatement(statements, (Statement)
        {
            .tag = STATEMENT_ASSIGNMENT,
            .assignment.variables = variables,
            .assignment.values = expressionList(source)
        });

        return true;
    }
    else
    {
        source->current = start;

        return false;
    }
}

bool callStatement(Source* source, StatementList* statements, DiagnosticList* diagnostics)
{
    char* start = source->current;
    Slice function = identifier(source);

    if (function.length == 0)
        return false;

    panicIfUnsynchronized(source, (TokenType[]){ TokenType_LeftParenthesis }, 1, (TokenType[]){ TokenType_RightParenthesis, TokenType_If, TokenType_For, TokenType_While, TokenType_Identifier, TokenType_End }, 6, diagnostics, "( expected.");
    literal(source, "(");

    panicIfUnsynchronized(source, (TokenType[]){ TokenType_RightParenthesis }, 1, (TokenType[]){ TokenType_If, TokenType_For, TokenType_While, TokenType_Identifier, TokenType_End }, 5, diagnostics, ") expected.");
    literal(source, ")");

    appendStatement(statements, (Statement)
    {
        .tag = STATEMENT_CALL,
        .call = (Call)
        {
            .declaredAt = (Position)
            {
                .path = source->path,
                .code = source->code,
                .index = start
            },
            .identifier = function,
            .input = (ExpressionList)
            {
                .elements = NULL,
                .count = 0,
                .capacity = 0
            }
        }
    });

    return true;
}

bool statement(Source* source, StatementList* statements, DiagnosticList* diagnostics)
{
    return assignmentStatement(source, statements, diagnostics) ||
           callStatement(source, statements, diagnostics);
}

StatementList statements(Source* source, DiagnosticList* diagnostics)
{
    StatementList list =
    {
        .elements = NULL,
        .count = 0,
        .capacity = 0
    };

    while (*source->current && statement(source, &list, diagnostics))
        ;

    return list;
}

FunctionDeclaration functionDeclaration(Access access, Source* source, DiagnosticList* diagnostics)
{
    FunctionDeclaration declaration =
    {
        .declaredAt = (Position)
        {
            .path = source->path,
            .code = source->code,
            .index = source->current
        },
        .signature = (FunctionSignature)
        {
            .access = access,
            .identifier = identifier(source)
        },
        .body = statements(source, diagnostics)
    };

    panicIfUnsynchronized(source, (TokenType[]){ TokenType_End }, 1, (TokenType[]){TokenType_Function, TokenType_Proposition, TokenType_Structure, TokenType_Union, TokenType_Enumeration }, 5, diagnostics, "end expected.");
    literal(source, "end");

    return declaration;
}

CompilationUnit compilationUnit(Source* source, DiagnosticList* diagnostics)
{
    whitespace(source);

    CompilationUnit unit =
    {
        .source = source,
        .namespace = namespaceDirective(source, diagnostics),
        .uses = useDirectives(source, diagnostics),
        .functions = (FunctionDeclarationList)
        {
            .elements = NULL,
            .count = 0,
            .capacity = 0
        }
    };

    while (*source->current)
    {
        Access access = accessibility(source);
        if (literal(source, "function"))
            appendFunctionDeclaration(&unit.functions, functionDeclaration(access, source, diagnostics));
        else
        {
            appendDiagnostic(diagnostics, (Diagnostic)
            {
                .description = "Function declaration expected.",
                .occurredAt = positionOf(source)
            });
        }
    }

    return unit;
}

Program parse(Source* sources, int length)
{
    Program program =
    {
        .compilationUnits =
        {
            .elements = NULL,
            .count = 0,
            .capacity = 0
        },
        .diagnostics = initDiagnosticList()
    };

    for (int32_t i = 0; i < length; i++)
    {
        CompilationUnit unit = compilationUnit(&sources[i], &program.diagnostics);
        appendCompilationUnit(&program.compilationUnits, unit);
    }

    return program;
}

void identifiersCantBeKeywords()
{
    char* code = "if";
    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    Slice keyword = identifier(&source);

    assert(code == source.current);
    assert(keyword.length == 0);
}

void identifiersCanBePartialKeywords()
{
    char* code = "name";
    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    Slice partial = identifier(&source);
    Slice expected =
    {
        .start = code,
        .length = 4
    };

    assert(compareSlices(&partial, &expected));
}

void identifierCanStartWithAKeyword()
{
    char* code = "ifn";
    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    Slice super = identifier(&source);
    Slice expected =
    {
        .start = code,
        .length = 3
    };

    assert(compareSlices(&super, &expected));
}

void canParseAQualifiedIdentifierWithOneIdentifier()
{
    DiagnosticList diagnostics = initDiagnosticList();

    char* code = "abc";
    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    char* abc = qualifiedIdentifier(&source, &diagnostics);

    assert(abc != NULL);
    assert(!strcmp(code, abc));
}

void canParseAQualifiedIdentifierWithMultipleIdentifiers()
{
    DiagnosticList diagnostics = initDiagnosticList();

    char* code = "abc . def";
    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    char* abcdef = qualifiedIdentifier(&source, &diagnostics);

    assert(abcdef != NULL);
    assert(!strcmp("abc.def", abcdef));
}

void missingIdentifierAfterDotInQualifiedIdentifierIssuesDiagnostic()
{
    DiagnosticList diagnostics = initDiagnosticList();

    char* code = "abc.";
    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    qualifiedIdentifier(&source, &diagnostics);

    assert(diagnostics.count == 1);
    assert(!strcmp(diagnostics.elements[0].description, "Identifier expected."));
}

void missingQualifiedIdentifierAfterNamespaceKeyword()
{
    DiagnosticList diagnostics = initDiagnosticList();

    char* code = "namespace";
    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    char* namespace = namespaceDirective(&source, &diagnostics);

    assert(namespace == NULL);
    assert(diagnostics.count == 1);
    assert(!strcmp(diagnostics.elements[0].description, "Qualified identifier expected."));
}

void canParseANamespaceDirective()
{
    char* code = "namespace abc";
    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    Program program = parse(&source, 1);

    assert(program.compilationUnits.elements != NULL);
    assert(program.compilationUnits.count == 1);
    assert(program.diagnostics.count == 0);

    CompilationUnit* unit = &program.compilationUnits.elements[0];

    assert(unit->source == &source);
    assert(!strcmp("abc", unit->namespace));
}

void canParseAnUseDirective()
{
    char* code = "namespace abc\n"
                 "use abc";

    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    Program program = parse(&source, 1);

    assert(program.compilationUnits.elements != NULL);
    assert(program.compilationUnits.count == 1);
    assert(program.diagnostics.count == 0);

    CompilationUnit* unit = &program.compilationUnits.elements[0];

    assert(unit->uses.count == 1);
}

void canParseUseDirectives()
{
    char* code = "namespace Abc\n"
                 "use abc\n"
                 "use def";

    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    Program program = parse(&source, 1);

    assert(program.compilationUnits.elements != NULL);
    assert(program.compilationUnits.count == 1);
    assert(program.diagnostics.count == 0);

    CompilationUnit* unit = &program.compilationUnits.elements[0];

    assert(unit->uses.count == 2);
}

void missingQualifiedIdentifierAfterUseKeywordIssuesDiagnostic()
{
    DiagnosticList diagnostics = initDiagnosticList();

    char* code = "use";
    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    char* namespace = useDirective(&source, &diagnostics);

    assert(namespace == NULL);
    assert(diagnostics.count == 1);
    assert(!strcmp(diagnostics.elements[0].description, "Qualified identifier expected."));
}

void canParseAnEmptyFunction()
{
    DiagnosticList diagnostics = initDiagnosticList();
    char* code = "main\n"
                 "end";

    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    FunctionDeclaration declaration = functionDeclaration(ACCESS_PRIVATE, &source, &diagnostics);

    assert(declaration.body.count == 0);
    assert(diagnostics.count == 0);
    assert(*source.current == 0);

    assert(declaration.signature.access == ACCESS_PRIVATE);
    assert(declaration.body.count == 0);
}

void canParseACompilationUnitWithAFunction()
{
    char* code = "namespace Abc\n"
                 "function main\n"
                 "end";

    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    Program program = parse(&source, 1);
    assert(program.compilationUnits.elements[0].functions.count == 1);
}

void canParseACompilationUnitWithAnEmptyPublicFunction()
{
    char* code = "namespace Abc\n"
                 "public function main\n"
                 "end";

    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    Program program = parse(&source, 1);
    assert(*source.current == 0);
    assert(program.diagnostics.count == 0);
    assert(program.compilationUnits.elements[0].functions.count == 1);

    FunctionDeclaration declaration = program.compilationUnits.elements[0].functions.elements[0];
    assert(declaration.signature.access == ACCESS_PUBLIC);
    assert(declaration.body.count == 0);
}

void canParseASingularValidVariableDeclaration()
{
    char* code = "namespace Abc\n"
                 "function main\n"
                 "    a = 0i32\n"
                 "end";

    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    Program program = parse(&source, 1);
    assert(*source.current == 0);
    assert(program.diagnostics.count == 0);
    assert(program.compilationUnits.elements[0].functions.count == 1);

    FunctionDeclaration declaration = program.compilationUnits.elements[0].functions.elements[0];
}

void parserTestSuite()
{
    identifiersCantBeKeywords();
    identifiersCanBePartialKeywords();
    identifierCanStartWithAKeyword();

    canParseAQualifiedIdentifierWithOneIdentifier();
    canParseAQualifiedIdentifierWithMultipleIdentifiers();
    missingIdentifierAfterDotInQualifiedIdentifierIssuesDiagnostic();

    canParseANamespaceDirective();
    missingQualifiedIdentifierAfterNamespaceKeyword();

    canParseAnUseDirective();
    canParseUseDirectives();
    missingQualifiedIdentifierAfterUseKeywordIssuesDiagnostic();

    canParseAnEmptyFunction();
    canParseACompilationUnitWithAFunction();
    canParseACompilationUnitWithAnEmptyPublicFunction();

    canParseASingularValidVariableDeclaration();

    canParseASingularValidVariableDeclaration();
}
