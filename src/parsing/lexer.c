#include "parsing.h"

Lexer lexer;

static bool is_invalid_code_point(uint32_t code_point)
{
    return code_point <= 0x0009 ||
           code_point >= 0x000B && code_point <= 0x001F ||
           code_point == 0x007F ||
           code_point == 0x0085 ||
           code_point == 0x2028 ||
           code_point == 0x2029;
}

static bool lexer_eof(void)
{
    return !lexer.contents.length;
}

static char peek1(void)
{
    if (lexer_eof())
        return 0;

    return lexer.contents.text[0];
}

static char peek2(void)
{
    if (lexer.contents.length < 2)
        return 0;

    return lexer.contents.text[1];
}

void advance_bytes(uint8_t amount)
{
    lexer.contents.text += amount;
    lexer.contents.length -= amount;
}

static char next_character(void)
{
    char c = peek1();
    advance_bytes(1);

    return c;
}

static bool advance_if_char(char c)
{
    if (!lexer_eof() && lexer.contents.text[0] == c)
    {
        next_character();
        return true;
    }

    return false;
}

void next_token(void)
{   
    start_timer(Timer_lexing);

    lexer.token.span.start = lexer.token.span.end;
    while (true)
    {
        if (lexer_eof())
        {
            lexer.token.tag = Token_Tag_eof;
            return;
        }
        else if (advance_if_char(' '))
            lexer.token.span.start.column++;
        else if (advance_if_char('\n'))
        {
            lexer.token.span.start.line++;
            lexer.token.span.start.column = 1;
        }
        else if (peek1() == '/' && peek2() == '/')
        {
            while (peek1() != '\n')
            {
                uint8_t bytes_in_sequence = count_bytes_in_sequence(lexer.contents.text);
                if (!bytes_in_sequence)
                    print_span_error(file, lexer.token.span, "Invalid Unicode code point.");

                uint32_t code_point = sequence_to_code_point(lexer.contents.text, bytes_in_sequence);
                if (is_invalid_code_point(code_point))
                    print_span_error(file, lexer.token.span, "%U not valid in comments.", code_point);
                    
                advance_bytes(bytes_in_sequence);
            }
        }
        else
            break;
    }

    char* start = lexer.contents.text;
    if (is_ascii_digit(peek1()))
    {
        uint8_t base = 10;
        if (peek1() == '0')
        {
            if (peek2() == 'b')
            {
                advance_bytes(2);
                base = 2;
            }
            else if (peek2() == 'x')
            {
                advance_bytes(2);
                base = 16;
            }
        }

        bool has_underscore = false;
        while (true)
        {
            if (base == 2)
            {
                while (is_ascii_binary_digit(peek1()))
                    next_character();
            }
            else if (base == 16)
            {
                while (is_ascii_hex_digit(peek1()))
                    next_character();
            }
            else
            {
                while (is_ascii_digit(peek1()))
                    next_character();
            }

            if (peek1() == '_' && ((base == 2  || is_ascii_binary_digit(peek2()) && 
                                    base == 16 || is_ascii_hex_digit(peek2()) &&
                                    base == 10 || is_ascii_digit(peek2()))))
            {
                next_character();
                has_underscore = true;
            }
            else
                break;
        }

        if (advance_if_char('.'))
        {
            if (base == 2)
                print_span_error(file, lexer.token.span, "Binary literals are not a thing for floats.");
            else if (base == 16)
                print_span_error(file, lexer.token.span, "Hex literals are not a thing for floats.");
            else if (has_underscore)
                print_span_error(file, lexer.token.span, "Floats cannot have underscores.");

            if (is_ascii_digit(peek1()))
            {
                next_character();
                while (is_ascii_digit(peek1()))
                    next_character();
            }
            else
                print_error("expected at least one digit.");

            lexer.token.tag = Token_Tag_float;
        }
        else
            lexer.token.tag = Token_Tag_integer;

        lexer.token.string.text = start;
        lexer.token.string.length = (uint32_t)(lexer.contents.text - start);
    }
    else if (is_ascii_lower(peek1()))
    {
        next_character();
        while (is_ascii_lower(peek1()) || is_ascii_digit(peek1()))
        {
            next_character();
            if (peek1() == '_' && is_ascii_lower(peek2()))
                next_character();
        }

        lexer.token.string.text = start;
        lexer.token.string.length = (uint32_t)(lexer.contents.text - start);
        lexer.token.tag = Token_Tag_lower_case_identifier;

        static struct
        {
            String string;
            uint8_t tag;
        } keywords[] = 
        {
            {{ "if",          2  },  Token_Tag_if          },
            {{ "use",         3  },  Token_Tag_use         },
            {{ "end",         3  },  Token_Tag_end         },
            {{ "for",         3  },  Token_Tag_for         },
            {{ "else",        4  },  Token_Tag_else        },
            {{ "true",        4  },  Token_Tag_true        },
            {{ "null",        4  },  Token_Tag_null        },
            {{ "elif",        4  },  Token_Tag_elif        },
            {{ "union",       5  },  Token_Tag_union       },
            {{ "false",       5  },  Token_Tag_false       },
            {{ "while",       5  },  Token_Tag_while       },
            {{ "break",       5  },  Token_Tag_break       },
            {{ "public",      6  },  Token_Tag_public      },
            {{ "return",      6  },  Token_Tag_return      },
            {{ "sizeof",      6  },  Token_Tag_sizeof      },
            {{ "version",     7  },  Token_Tag_version     },
            {{ "external",    8  },  Token_Tag_external    },
            {{ "function",    8  },  Token_Tag_function    },
            {{ "continue",    8  },  Token_Tag_continue    },
            {{ "structure",   9  },  Token_Tag_structure   },
            {{ "namespace",   9  },  Token_Tag_namespace   },
            {{ "enumeration", 11 },  Token_Tag_enumeration },
        };

        for (uint8_t i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
        {
            if (compare(keywords[i].string, lexer.token.string))
            {
                lexer.token.tag = keywords[i].tag;
                break;
            }
        }

        if (lexer.token.tag == Token_Tag_lower_case_identifier)
            lexer.token.interned = add_interned_string(lexer.token.string);
    }
    else if (is_ascii_upper(peek1()))
    {
        do
        {
            next_character();
            while (is_ascii_lower(peek1()) || is_ascii_digit(peek1()))
                next_character();

            if (peek1() == '_' && is_ascii_upper(peek2()))
                advance_bytes(2);
        } while (is_ascii_upper(peek1()));

        lexer.token.tag = Token_Tag_upper_case_identifier;
        lexer.token.string.text = start;
        lexer.token.string.length = (uint32_t)(lexer.contents.text - start);
        lexer.token.interned = add_interned_string(lexer.token.string);
    }
    else if (advance_if_char('"'))
    {
        lexer.token.string.text = lexer.contents.text;
        while (peek1() != '"')
        {
            uint8_t bytes_in_sequence = count_bytes_in_sequence(lexer.contents.text);
            if (!bytes_in_sequence)
                print_span_error(file, lexer.token.span, "Invalid Unicode code point.");

            uint32_t code_point = sequence_to_code_point(lexer.contents.text, bytes_in_sequence);
            if (is_invalid_code_point(code_point))
                print_span_error(file, lexer.token.span, "%U must be escaped in strings and Unicode code point literal.", code_point);

            advance_bytes(bytes_in_sequence);
        }

        lexer.token.string.length = (uint32_t)(lexer.contents.text - lexer.token.string.text);
        if (!advance_if_char('"'))
            print_error("\" expected at the end of character literal.");

        lexer.token.interned = add_interned_string(lexer.token.string);
        lexer.token.tag = Token_Tag_utf8_string;
    }
    else if (advance_if_char('\''))
    {
        uint8_t bytes_in_sequence = count_bytes_in_sequence(lexer.contents.text);
        if (!bytes_in_sequence)
            print_span_error(file, lexer.token.span, "Invalid Unicode code point.");

        lexer.token.code_point = sequence_to_code_point(lexer.contents.text, bytes_in_sequence);
        if (is_invalid_code_point(lexer.token.code_point))
            print_span_error(file, lexer.token.span, "%U must be escaped in strings and Unicode code point literal.", lexer.token.code_point);

        advance_bytes(bytes_in_sequence);
        if (!advance_if_char('\''))
            print_span_error(file, lexer.token.span, "' expected value in character literal.");

        lexer.token.tag = Token_Tag_unicode_code_point;
    }
    else if (advance_if_char('+'))
        lexer.token.tag = advance_if_char('=') ? Token_Tag_plus_equal : Token_Tag_plus;
    else if (advance_if_char('-'))
    {
        if (advance_if_char('='))
            lexer.token.tag = Token_Tag_minus_equal;
        else if (peek1() == '-' && peek2() == '-')
        {
            advance_bytes(2);
            lexer.token.tag = Token_Tag_uninitialized_literal;
        }
        else
            lexer.token.tag = Token_Tag_minus;
    }
    else if (advance_if_char('*'))
        lexer.token.tag = advance_if_char('=') ? Token_Tag_multiply_equal : Token_Tag_multiply;
    else if (advance_if_char('/'))
        lexer.token.tag = advance_if_char('=') ? Token_Tag_divide_equal : Token_Tag_divide;
    else if (advance_if_char('&'))
    {
        if (advance_if_char('&'))
            lexer.token.tag = Token_Tag_logical_and;
        else if (advance_if_char('='))
            lexer.token.tag = Token_Tag_bitwise_and_equal;
        else
            lexer.token.tag = Token_Tag_bitwise_and;
    }
    else if (advance_if_char('|'))
    {
        if (advance_if_char('|'))
            lexer.token.tag = Token_Tag_logical_or;
        else if (advance_if_char('='))
            lexer.token.tag = Token_Tag_bitwise_or_equal;
        else
            lexer.token.tag = Token_Tag_bitwise_or;
    }
    else if (advance_if_char('^'))
        lexer.token.tag = advance_if_char('=') ? Token_Tag_bitwise_xor_equal : Token_Tag_bitwise_xor;
    else if (advance_if_char('%'))
        lexer.token.tag = advance_if_char('=') ? Token_Tag_modulo_equal : Token_Tag_modulo;
    else if (advance_if_char('<'))
    {
        if (advance_if_char('='))
            lexer.token.tag = Token_Tag_less_than_or_equal;
        else if (advance_if_char('<'))
            lexer.token.tag = advance_if_char('=') ? Token_Tag_left_shift_equal : Token_Tag_left_shift;
        else
            lexer.token.tag = Token_Tag_less_than;
    }
    else if (advance_if_char('>'))
    {
        if (advance_if_char('='))
            lexer.token.tag = Token_Tag_greater_than_or_equal;
        else if (advance_if_char('>'))
            lexer.token.tag = advance_if_char('=') ? Token_Tag_right_shift_equal : Token_Tag_right_shift;
        else
            lexer.token.tag = Token_Tag_greater_than;
    }
    else if (advance_if_char('='))
        lexer.token.tag = advance_if_char('=') ? Token_Tag_equal_equal : Token_Tag_equal;
    else if (advance_if_char('!'))
        lexer.token.tag = advance_if_char('=') ? Token_Tag_not_equal : Token_Tag_exclamation_mark;
    else if (advance_if_char('#'))
        lexer.token.tag = Token_Tag_pound_sign;
    else if (advance_if_char('@'))
        lexer.token.tag = Token_Tag_dereference;
    else if (advance_if_char('.'))
        lexer.token.tag = Token_Tag_dot;
    else if (advance_if_char('('))
        lexer.token.tag = Token_Tag_left_parentheses;
    else if (advance_if_char(')'))
        lexer.token.tag = Token_Tag_right_parentheses;
    else if (advance_if_char('['))
        lexer.token.tag = Token_Tag_left_square_bracket;
    else if (advance_if_char(']'))
        lexer.token.tag = Token_Tag_right_square_bracket;
    else if (advance_if_char('{'))
        lexer.token.tag = Token_Tag_left_curly_bracket;
    else if (advance_if_char('}'))
        lexer.token.tag = Token_Tag_right_curly_bracket;
    else if (advance_if_char(','))
        lexer.token.tag = Token_Tag_comma;
    else if (advance_if_char(';'))
        lexer.token.tag = Token_Tag_semicolon;
    else if (advance_if_char(':'))
        lexer.token.tag = Token_Tag_colon;
    else if (advance_if_char('`'))
        lexer.token.tag = Token_Tag_backtick;
    else if (advance_if_char('?'))
        lexer.token.tag = Token_Tag_question_mark;
    else if (advance_if_char('$'))
        lexer.token.tag = Token_Tag_dollar_sign;
    else if (advance_if_char('_'))
        lexer.token.tag = Token_Tag_blank_identifier;
    else
        print_span_error(file, lexer.token.span, "Invalid token starting with %U.", 
                         sequence_to_code_point(lexer.contents.text, count_bytes_in_sequence(lexer.contents.text)));
    
    lexer.token.span.end = (Position)
    {
        .line   = lexer.token.span.start.line,
        .column = lexer.token.span.start.column + (uint16_t)(lexer.contents.text - start)
    };
    
    start_timer(Timer_parsing);
}

static String read_file(char* path)
{
    start_timer(Timer_file_reading);
    FILE* f = fopen(path, "rb");
    if (f == NULL)
        print_error("Could not open \"%s\".", path);

    fseek(f, 0, SEEK_END);
    uint32_t file_length = ftell(f);
    rewind(f);

    uint32_t new_characters_length = characters_length + file_length;
    if (new_characters_length >= CHARACTER_CAPACITY)
        out_of_memory(__FILE__, __LINE__, "character.");
    else if (fread(&characters[characters_length], 1, file_length, f) != file_length)
        print_error("Could not read \"%s\".", path);

    fclose(f);

    String contents =
    {
        .text = &characters[characters_length],
        .length = new_characters_length - characters_length
    };

    characters_length = new_characters_length;

    return contents;
}

void init_lexer(char* path)
{
    // Skip UTF-8 BOM in case there is one.
    lexer.contents = read_file(path);
    if (lexer.contents.length >= 3 && lexer.contents.text[0] == 0xEF && lexer.contents.text[1] == 0xBB && lexer.contents.text[2] == 0xBF)
        advance_bytes(3);

    lexer.token.span = (Span)
    {
        .start = { .line = 1, .column = 1 },
        .end   = { .line = 1, .column = 1 },
    };

    next_token();
}
