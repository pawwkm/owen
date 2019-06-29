#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "parser.h"
#include "semantics.h"
#include "generate.h"
#include "errors.h"

DECLARE_LIST_APPEND(Source)

typedef struct
{
    char* output;
    char* target;
    bool isRelease;
    bool analyzeOnly;
    bool runPropositions;
    SourceList sources;
} Options;

Options parseCommandLine(int argc, char **argv)
{
    bool isSourceFile(char *string)
    {
        string = strrchr(string, '.');
        return string != NULL &&
               !strcmp(string, ".owen");
    }

    Options options =
    {
        .output = NULL,
        .target = NULL,
        .isRelease = false,
        .analyzeOnly = false,
        .sources = (SourceList) NEW_LIST()
    };

    for (int32_t i = 0; i < argc; i++)
    {
        if (isSourceFile(argv[i]))
        {
            FILE* file = fopen(argv[i], "r");
            if (!file)
                error("Could not open %s", argv[i]);

            fseek(file, 0, SEEK_END);
            int32_t length = ftell(file);

            Source source;
            source.path = argv[i],
            source.contents = malloc(length + 1),
            source.current = source.contents;

            fseek(file, 0, SEEK_SET);
            int32_t bytesRead = fread(source.contents, 1, length, file);
            fclose(file);

            source.contents[bytesRead] = 0;
            appendSource(&options.sources, source);
        }
        else if (!strcmp(argv[i], "-o"))
        {
            options.output = argv[i + 1];
            i++;
        }
        else if (!strcmp(argv[i], "-t"))
        {
            options.target = argv[i + 1];
            i++;
        }
        else if (!strcmp(argv[i], "--analyzeOnly"))
            options.analyzeOnly = true;
        else if (!strcmp(argv[i], "--release"))
            options.isRelease = true;
        else if (!strcmp(argv[i], "--propositions"))
            options.runPropositions = true;
        else if (!strcmp(argv[i], "--version"))
        {
            printf("Compiler Version:  0.1.0\n"
                   "Language Version:  0.1.0\n");

            exit(EXIT_SUCCESS);
        }
        else if (!strcmp(argv[i], "--help"))
        {
            printf("owen [options] <inputs> \n"
                   "  --analyzeOnly          Analyzes the program but no code is generated\n"
                   "  -o                     The name of the output file\n"
                   "  --propositions         Includes propositions in the build\n"
                   "  --release              Turns optimizations on\n"
                   "  -t                     Sets the target for a build. Defaults to this machine\n");

            exit(EXIT_SUCCESS);
        }
        else
            error("Invalid command line argument: %s", argv[i]);
    }

    if (!options.output && !options.analyzeOnly)
        error("No output file specified");

    return options;
}

int main(int argc, char **argv)
{
    Options options = parseCommandLine(argc - 1, argv + 1);

    Program program = parse(&options.sources);
    analyze(&program);

    if (!options.analyzeOnly)
        generate(&program, options.output);

    return 0;
}
