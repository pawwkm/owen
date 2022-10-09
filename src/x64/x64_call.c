#include "x64.h"

void x64_call(const Ir_X64_Call* call)
{
    write_text_u8(0xE8);
    write_text_i32((int32_t)((int64_t)call->absolute_address - ((int64_t)text_section_length + 4)));
}

uint16_t size_of_x64_call(void)
{
    return 5;
}