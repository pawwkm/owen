#include "compiler.h"

// Layout of UTF-8 byte sequences
// | Number of bytes | First code point | Last code point | Byte 1   | Byte 2   | Byte 3   | Byte 4   |
// |-----------------|------------------|-----------------|----------|----------|----------|----------|
// |        1        |           U+0000 |          U+007F | 0xxxxxxx |          |          |          |
// |        2        |           U+0080 |          U+07FF | 110xxxxx | 10xxxxxx |          |          |
// |        3        |           U+0800 |          U+FFFF | 1110xxxx | 10xxxxxx | 10xxxxxx |          |
// |        4        |          U+10000 |        U+10FFFF | 11110xxx | 10xxxxxx | 10xxxxxx | 10xxxxxx |
uint8_t count_bytes_in_sequence(const char* text)
{
    uint8_t bytes_in_sequence = 0;
    uint8_t first_byte = *text;

    if (first_byte == '\\' && (text[1] == 'u' || text[1] == 'U'))
    {
        if (text[1] == 'u')
            bytes_in_sequence = 6;
        else
            bytes_in_sequence = 10;

        for (uint8_t i = 2; i < bytes_in_sequence; i++)
        {
            uint8_t byte = text[i];
            if (!(is_ascii_upper(byte) && is_ascii_alpha_numeric(byte)))
                return 0;
        }
    
        return bytes_in_sequence;
    }
    else if ((first_byte & 0xF0) == 0xF0)
        return 4;
    else if ((first_byte & 0xE0) == 0xE0)
        return 3;
    else if ((first_byte & 0xC0) == 0xC0)
        return 2;
    else if ((first_byte & 0x80) == 0x00)
        return 1;
    else
        return 0;
}

uint32_t sequence_to_code_point(const char* text, uint8_t bytes_in_sequence)
{
    uint32_t code_point = 0;
    if (bytes_in_sequence == 10 || bytes_in_sequence == 6)
    {
        for (uint8_t i = 3; i < bytes_in_sequence; i++)
        {
            uint8_t b = text[i];
            if (is_ascii_digit(b))
                code_point = code_point * 16 + b - '0';
            else
                code_point = code_point * 16 + b - 'a' + 10;
        }
    }
    else
    {
        if (bytes_in_sequence == 4)
            code_point = *text & 0x07;
        else if (bytes_in_sequence == 3)
            code_point = *text & 0x0F;
        else if (bytes_in_sequence == 2)
            code_point = *text & 0x1F;
        else
            code_point = *text & 0x7F;

        for (uint8_t i = 1; i < bytes_in_sequence; i++)
        {
            uint8_t b = text[i];
            if ((b & 0xC0) == 0x80)
                code_point = (code_point << 6) + (text[i] & 0x3F);
        }
    }
    
    return code_point;
}
