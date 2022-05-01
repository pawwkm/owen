#include "x64.h"

void write_imm(const Ir_Immediate* imm)
{
    if (compare_types(imm->type, u8_handle))
        text_section[text_section_length++] = imm->value.u8        & 0xFF;
    else if (compare_types(imm->type, i8_handle))
        text_section[text_section_length++] = imm->value.i8        & 0xFF;
    else if (compare_types(imm->type, u16_handle))
    {
        text_section[text_section_length++] = imm->value.u16       & 0xFF;
        text_section[text_section_length++] = imm->value.u16 >> 8  & 0xFF;
    }
    else if (compare_types(imm->type, i16_handle))
    {
        text_section[text_section_length++] = imm->value.i16       & 0xFF;
        text_section[text_section_length++] = imm->value.i16 >> 8  & 0xFF;
    }
    else if (compare_types(imm->type, u32_handle))
    {
        text_section[text_section_length++] = imm->value.u32       & 0xFF;
        text_section[text_section_length++] = imm->value.u32 >> 8  & 0xFF;
        text_section[text_section_length++] = imm->value.u32 >> 16 & 0xFF;
        text_section[text_section_length++] = imm->value.u32 >> 24 & 0xFF;
    }
    else if (compare_types(imm->type, i32_handle))
    {
        text_section[text_section_length++] = imm->value.i32       & 0xFF;
        text_section[text_section_length++] = imm->value.i32 >> 8  & 0xFF;
        text_section[text_section_length++] = imm->value.i32 >> 16 & 0xFF;
        text_section[text_section_length++] = imm->value.i32 >> 24 & 0xFF;
    }
    else if (compare_types(imm->type, u64_handle))
    {
        text_section[text_section_length++] = imm->value.u64       & 0xFF;
        text_section[text_section_length++] = imm->value.u64 >> 8  & 0xFF;
        text_section[text_section_length++] = imm->value.u64 >> 16 & 0xFF;
        text_section[text_section_length++] = imm->value.u64 >> 24 & 0xFF;
        text_section[text_section_length++] = imm->value.u64 >> 32 & 0xFF;
        text_section[text_section_length++] = imm->value.u64 >> 40 & 0xFF;
        text_section[text_section_length++] = imm->value.u64 >> 48 & 0xFF;
        text_section[text_section_length++] = imm->value.u64 >> 56 & 0xFF;
    }
    else if (compare_types(imm->type, i64_handle))
    {
        text_section[text_section_length++] = imm->value.u64       & 0xFF;
        text_section[text_section_length++] = imm->value.u64 >> 8  & 0xFF;
        text_section[text_section_length++] = imm->value.u64 >> 16 & 0xFF;
        text_section[text_section_length++] = imm->value.u64 >> 24 & 0xFF;
        text_section[text_section_length++] = imm->value.u64 >> 32 & 0xFF;
        text_section[text_section_length++] = imm->value.u64 >> 40 & 0xFF;
        text_section[text_section_length++] = imm->value.u64 >> 48 & 0xFF;
        text_section[text_section_length++] = imm->value.u64 >> 56 & 0xFF;
    }
    else
        assert(false);    
}

bool imm_is_zero(const Ir_Immediate* imm)
{
    if (compare_types(imm->type, u8_handle))
        return !imm->value.u8;
    else if (compare_types(imm->type, i8_handle))
        return !imm->value.i8;
    else if (compare_types(imm->type, u16_handle))
        return !imm->value.u16;
    else if (compare_types(imm->type, i16_handle))
        return !imm->value.i16;
    else if (compare_types(imm->type, u32_handle))
        return !imm->value.u32;
    else if (compare_types(imm->type, i32_handle))
        return !imm->value.i32;
    else if (compare_types(imm->type, u64_handle))
        return !imm->value.u64;
    else if (compare_types(imm->type, i64_handle))
        return !imm->value.i64;
    else
        assert(false);

    return false;    
}
