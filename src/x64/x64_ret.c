#include "x64.h"

void x64_ret(void)
{
    write_text_u8(0xC3);
}

uint16_t size_of_x64_ret(void)
{
    return 1;
}
