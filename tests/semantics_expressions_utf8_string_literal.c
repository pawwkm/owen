#include "runner.h"

static void literals_are_readonly_qualified(void)
{
    format_test_name("semantics/expressions/utf8_string_literal/literals_are_readonly_qualified");

    format_source_path(0, "semantics/expressions/utf8_string_literal/literals_are_readonly_qualified.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a() : readonly #[]U8\n"
                          "    return \"abc\"\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/expressions/utf8_string_literal/literals_are_readonly_qualified.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/expressions/utf8_string_literal/literals_are_readonly_qualified.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        return_type: readonly #[]U8\n"
                       "        return_statement\n"
                       "            utf8_string_literal\n"
                       "                type: readonly #[]U8\n"
                       "                value: \"abc\"\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

bool semantics_expressions_utf8_string_literal(void)
{
    static uint8_t state;
    switch (state++)
    {
        case 0:
            literals_are_readonly_qualified();
            break;

        default:
            return false;
    }

    return true;
}
