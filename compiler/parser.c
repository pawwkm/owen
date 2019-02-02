#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>

#include "parser.h"

bool whitespace(Source* source)
{
    bool skipped = false;
    while (source->current)
    {
        switch (*source->current)
        {
            case ' ':
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
    char* start = source->current;
    if (*literal == *source->current)
    {
        while (*literal == *source->current)
        {
            if (*literal != *source->current)
            {
                source->current = start;

                return false;
            }
            else
            {
                literal++;
                source->current++;
            }
        }

        whitespace(source);

        return true;
    }
    else
        return false;
}

Slice identifier(Source* source)
{
    char* start = source->current;
    char* end = start;

    if (isalpha(*source->current))
    {
        while (isalpha(*source->current) || isdigit(*source->current))
            source->current++;

        end = source->current;
        whitespace(source);
    }

    return (Slice)
    {
        .start = start,
        .length = end - start
    };
}

QualifiedIdentifier* qualifiedIdentifier(Source* source)
{
    Slice first = identifier(source);
    if (first.length != 0)
    {
        QualifiedIdentifier* list = calloc(1, sizeof(QualifiedIdentifier));
        appendIdentifier(list, first);

        while (literal(source, "."))
            appendIdentifier(list, identifier(source));

        return list;
    }

    return NULL;
}

QualifiedIdentifier* namespaceDirective(Source* source)
{
    return literal(source, "namespace") ? qualifiedIdentifier(source) :
                                          NULL;
}

CompilationUnit compilationUnit(Source* source, DiagnosticList* diagnostics)
{
    whitespace(source);

    return (CompilationUnit)
    {
        .source = source,
        .namespace = namespaceDirective(source)
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
        .diagnostics =
        {
            .elements = NULL,
            .count = 0,
            .capacity = 0
        }
    };

    for (int32_t i = 0; i < length; i++)
    {
        CompilationUnit unit = compilationUnit(&sources[i], &program.diagnostics);
        appendCompilationUnit(&program.compilationUnits, unit);
    }

    return program;
}

void canParseAQualifiedIdentifierWithOneIdentifier()
{
    char* code = "abc";
    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    QualifiedIdentifier* list = qualifiedIdentifier(&source);
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
    char* code = "abc.def";
    Source source =
    {
       .path = "main.owen",
       .code = code,
       .current = code
    };

    QualifiedIdentifier* list = qualifiedIdentifier(&source);
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

void canParseANamespace()
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

void parserTestSuite()
{
    canParseAQualifiedIdentifierWithOneIdentifier();
    canParseAQualifiedIdentifierWithMultipleIdentifiers();
    canParseANamespace();
}
