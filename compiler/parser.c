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
    "continue",
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

Slice identifier(Source* source)
{
    bool isKeyword(char* start, size_t length)
    {
        int32_t i = 0;
        while (true)
        {
            char* keyword = keywords[i];
            if (keyword == 0)
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

QualifiedIdentifier* qualifiedIdentifier(Source* source, DiagnosticList* diagnostics)
{
    Slice first = identifier(source);
    if (first.length != 0)
    {
        QualifiedIdentifier* list = calloc(1, sizeof(QualifiedIdentifier));
        appendIdentifier(list, first);

        while (literal(source, "."))
        {
            Slice next = identifier(source);
            if (next.length == 0)
            {
                appendDiagnostic(diagnostics, (Diagnostic)
                {
                    .description = "Identifier expected.",
                    .code = source->code,
                    .index = source->current
                });

                break;
            }
            else
                appendIdentifier(list, next);
        }

        return list;
    }

    return NULL;
}

QualifiedIdentifier* namespaceDirective(Source* source, DiagnosticList* diagnostics)
{
    if (!literal(source, "namespace"))
        return NULL;

    QualifiedIdentifier* list =  qualifiedIdentifier(source, diagnostics);
    if (!list)
    {
        appendDiagnostic(diagnostics, (Diagnostic)
        {
            .description = "Qualified identifier expected.",
            .code = source->code,
            .index = source->current
        });
    }

    return list;
}

QualifiedIdentifier* useDirective(Source* source, DiagnosticList* diagnostics)
{
    if (!literal(source, "use"))
        return NULL;

    QualifiedIdentifier* list =  qualifiedIdentifier(source, diagnostics);
    if (!list)
    {
        appendDiagnostic(diagnostics, (Diagnostic)
        {
            .description = "Qualified identifier expected.",
            .code = source->code,
            .index = source->current
        });
    }

    return list;
}

QualifiedIdentifierList* useDirectives(Source* source, DiagnosticList* diagnostics)
{
    QualifiedIdentifier* next = useDirective(source, diagnostics);
    if (next)
    {
        QualifiedIdentifierList* list = calloc(1, sizeof(QualifiedIdentifierList));
        appendQualifiedIdentifier(list, *next);

        while (next = useDirective(source, diagnostics))
            appendQualifiedIdentifier(list, *next);

        return list;
    }
    else
        return NULL;
}

FunctionSignature functionSignature(Source* source)
{
    char* start = source->current;
    FunctionSignature signature =
    {
        .isPublic =  literal(source, "public"),
        .identifier = (Slice)
        {
            .start = 0,
            .length = 0
        }
    };

    if (literal(source, "function"))
    {
        signature.identifier = identifier(source);
    }

    return signature;
}

CompilationUnit compilationUnit(Source* source, DiagnosticList* diagnostics)
{
    whitespace(source);

    return (CompilationUnit)
    {
        .source = source,
        .namespace = namespaceDirective(source, diagnostics),
        .uses = useDirectives(source, diagnostics)
    };
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

    QualifiedIdentifier* list = qualifiedIdentifier(&source, &diagnostics);
    Slice expected =
    {
        .start = code,
        .length = 3
    };

    assert(list != NULL);
    assert(list->count == 1);
    assert(compareSlices(&list->elements[0], &expected));
}

void canParseAQualifiedIdentifierWithMultipleIdentifiers()
{
    DiagnosticList diagnostics = initDiagnosticList();

    char* code = "abc.def";
    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    QualifiedIdentifier* list = qualifiedIdentifier(&source, &diagnostics);
    Slice first =
    {
        .start = code,
        .length = 3
    };

    Slice second =
    {
        .start = &code[4],
        .length = 3
    };

    assert(list != NULL);
    assert(list->count == 2);
    assert(compareSlices(&list->elements[0], &first));
    assert(compareSlices(&list->elements[1], &second));
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

    QualifiedIdentifier* list = qualifiedIdentifier(&source, &diagnostics);
    Slice expected =
    {
        .start = code,
        .length = 3
    };

    assert(list != NULL);
    assert(list->count == 1);
    assert(compareSlices(&list->elements[0], &expected));

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

    QualifiedIdentifier* list = namespaceDirective(&source, &diagnostics);

    assert(list == NULL);
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
    assert(unit->namespace != NULL);
}

void canParseAUseDirective()
{
    char* code = "use abc";
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
    char* code = "use abc\n"
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

    QualifiedIdentifier* list = useDirective(&source, &diagnostics);

    assert(list == NULL);
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

    FunctionSignature signature = functionSignature(&source);
    Slice expected =
    {
        .start = code + 9,
        .length = 4
    };

    assert(compareSlices(&expected, &signature.identifier));
}

void canParseAPublicFunctionSignatureWithoutInputOrOutput()
{
    char* code = "public function main";
    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    FunctionSignature signature = functionSignature(&source);
    Slice expected =
    {
        .start = code + 16,
        .length = 4
    };

    assert(compareSlices(&expected, &signature.identifier));
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

    canParseAUseDirective();
    canParseUseDirectives();
    missingQualifiedIdentifierAfterUseKeywordIssuesDiagnostic();

    canParseAFunctionSignatureWithoutInputOrOutput();
    canParseAPublicFunctionSignatureWithoutInputOrOutput();
}
