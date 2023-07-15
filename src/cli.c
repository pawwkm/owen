#include "compiler.h"
#include <string.h>
#include <stdlib.h>

Options options;
static void parse_arguments(int argc, char* argv[])
{
    if (argc == 1)
        print_error("Usage: owen <files> [options]\n"
                    "\n"
                    "Options:\n"
                    "  -o                  Path to the executable.\n"
                    "  -print-semantics    Print semantic information of the program.\n"
                    "  -print-memory       Print the memory usage.\n"
                    "  -print-timings      Print the timing of each pass.\n"
                    "  -print-version      Print the compiler's version number then exit.", stderr);

    for (int i = 1; i < argc; i++)
    {
        if (!strcmp("-o", argv[i]))
        {
            if (i + 1 == argc)
                print_error("Missing path after -o");

            options.destination = argv[++i];
        }
        else if (!strcmp("-print-semantics", argv[i]))
            options.print_semantics = true;
        else if (!strcmp("-print-timings", argv[i]))
            options.print_timings = true;
        else if (!strcmp("-print-memory", argv[i]))
            options.print_memory = true;
        else if (!strcmp("-print-version", argv[i]))
            options.print_version = true;
        else
        {
            char* extension = strrchr(argv[i], '.');
            if (!extension || strcmp(extension, ".owen"))
                print_error("%s is not an option or .owen file.", argv[i]);
            
            parse_file(argv[i]);
        }
    }
}

static void print_extra_info(bool valid_semantics)
{
    stop_current_timer();
    if (options.print_semantics && valid_semantics)
        print_semantics();

    //print_ir();
    if (options.print_timings)
        print_timings();

    if (options.print_memory)
        print_memory();
}

void exit_with_error_code(void)
{
    // It is much easier to figure out what caused an error
    // message to be printed by setting a break point here
    // since all errors uses exit_with_error_code(void). 

    if (options.print_memory || options.print_timings)
        fputc('\n', stderr);

    print_extra_info(false);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    initialize_analizer();
    parse_arguments(argc, argv);
    if (options.print_version)
    {
        puts(VERSION "\n");
        return 0;
    }
   
    if (files_length)
    {
        analyze_program();
        if (options.destination)
        {
            lower_ast();
            ir_abi_pass();
            ir_x64_pass();

            generate_pe();
        }
    }

    print_extra_info(true);
    
    return 0;
}
