#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "parser.h"

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

StringList* useDirectives(Source* source, DiagnosticList* diagnostics)
{
    char* next = useDirective(source, diagnostics);
    if (next)
    {
        StringList* list = calloc(1, sizeof(StringList));
        appendString(list, next);

        while (next = useDirective(source, diagnostics))
            appendString(list, next);

        return list;
    }
    else
        return NULL;
}

bool callStatement(Source* source, StatementList* statements)
{
    char* start = source->current;
    Slice function = identifier(source);

    if (function.length == 0)
        return false;

    if (!literal(source, "("))
    {

    }

    if (!literal(source, ")"))
    {

    }

    appendStatement(statements, (Statement)
    {
        .declaredAt = (Position)
        {
            .path = source->path,
            .code = source->code,
            .index = start
        },
        .type = STATEMENT_UNRESOLVED_CALL,
        .identifier = function
    });

    return true;
}

bool statement(Source* source, StatementList* statements)
{
    return callStatement(source, statements);
}

StatementList statements(Source* source)
{
    StatementList list =
    {
        .elements = NULL,
        .count = 0,
        .capacity = 0
    };

    while (*source->current && statement(source, &list))
        ;

    return list;
}

FunctionSignature functionSignature(Access access, Source* source)
{
    char* start = source->current;
    FunctionSignature signature =
    {
        .access = access,
        .identifier = (Slice)
        {
            .start = 0,
            .length = 0
        }
    };

    if (literal(source, "function"))
        signature.identifier = identifier(source);

    return signature;
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
        .signature = functionSignature(access, source),
        .body = statements(source)
    };

    if (!literal(source, "end"))
    {

    }

    return declaration;
}

CompilationUnit compilationUnit(Source* source, DiagnosticList* diagnostics)
{
    whitespace(source);

    FunctionDeclarationList* functions = calloc(1, sizeof(FunctionDeclarationList));
    CompilationUnit unit =
    {
        .source = source,
        .namespace = namespaceDirective(source, diagnostics),
        .uses = useDirectives(source, diagnostics),
        .functions = functions
    };

    while (*source->current)
    {
        Access access = accessibility(source);
        char* start = source->current;

        FunctionDeclaration function = functionDeclaration(access, source, diagnostics);
        if (start == source->current)
        {
            appendDiagnostic(diagnostics, (Diagnostic)
            {
                .description = "Function declaration expected.",
                .occurredAt = positionOf(source)
            });
        }
        else
            appendFunctionDeclaration(functions, function);
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

    assert(unit->uses != NULL);
    assert(unit->uses->count == 1);
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

    assert(unit->uses != NULL);
    assert(unit->uses->count == 2);
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

void canParseAFunctionSignatureWithoutInputOrOutput()
{
    char* code = "function main";
    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    FunctionSignature signature = functionSignature(ACCESS_PRIVATE, &source);
    Slice expected =
    {
        .start = code + 9,
        .length = 4
    };

    assert(compareSlices(&expected, &signature.identifier));
}

void canParseAPublicFunctionSignatureWithoutInputOrOutput()
{
    char* code = "function main";
    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    FunctionSignature signature = functionSignature(ACCESS_PRIVATE, &source);
    Slice expected =
    {
        .start = code + 9,
        .length = 4
    };

    assert(compareSlices(&expected, &signature.identifier));
}

void canParseAFunctionWithoutStatements()
{
    DiagnosticList diagnostics = initDiagnosticList();
    char* code = "function main\n"
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
}

void canParseACompilationUnitWithAFunction()
{
    char* code = "namespace Abc"
                 "function main\n"
                 "end";

    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    Program program = parse(&source, 1);
    assert(program.compilationUnits.elements[0].functions->count == 1);
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

    canParseAFunctionSignatureWithoutInputOrOutput();
    canParseAPublicFunctionSignatureWithoutInputOrOutput();

    canParseAFunctionWithoutStatements();
    canParseACompilationUnitWithAFunction();
}
