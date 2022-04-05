#include "compiler.h"

bool is_ascii_upper(uint8_t c)
{
    return c >= 'A' && c <= 'Z';
} 

bool is_ascii_lower(uint8_t c)
{
    return c >= 'a' && c <= 'z';
}

bool is_ascii_digit(uint8_t c)
{
    return c >= '0' && c <= '9';
}

bool is_ascii_alpha_numeric(uint8_t c)
{
    return is_ascii_upper(c) ||
           is_ascii_lower(c) ||
           is_ascii_digit(c);
}

bool is_ascii_hex_digit(uint8_t c)
{
    return is_ascii_digit(c) ||
           c >= 'A' && c <= 'F';
}

bool is_ascii_binary_digit(uint8_t c)
{
    return c == '0' || 
           c == '1';
}

