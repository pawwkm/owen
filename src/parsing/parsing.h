#include "../compiler.h"

typedef struct
{
    Span span;
    Token_Tag tag;
    union
    {
        Interned_String_Handle interned;
        String string;
        uint32_t code_point;
    };
} Token;

typedef struct
{
    String contents;
    Token token;
} Lexer;

Lexer lexer;
File_Handle file_handle;
File* file;

void init_lexer(char* path);
void next_token(void);
void advance_bytes(uint8_t amount);