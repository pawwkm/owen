#include "x64.h"

static void mov_reg_imm(const Ir_X64_Register* destination, const Ir_Immediate* source)
{
    if (imm_is_zero(source))
    {
        xor((Ir_Operand*)destination, (Ir_Operand*)destination);
        return;
    }
    
    if (compare_types(source->type, u8_handle) || compare_types(source->type, i8_handle))
    {
        // MOV r8, imm8.
        if (destination->reg >= X64_Register_Tag_r8)
        {
            text_section[text_section_length++] = REX_PREFIX(REX_B);
            text_section[text_section_length++] = 0xB0 + destination->reg - X64_Register_Tag_r8;
        }
        else if (destination->reg >= X64_Register_Tag_sp)
        {
            text_section[text_section_length++] = REX_PREFIX(0);
            text_section[text_section_length++] = 0xB0 + destination->reg;
        }
    }
    else if (compare_types(source->type, u16_handle) || compare_types(source->type, i16_handle))
    {
        // MOV r16, imm16.
        text_section[text_section_length++] = OPERAND_SIZE_PREFIX;
        if (destination->reg >= X64_Register_Tag_r8)
        {
            text_section[text_section_length++] = REX_PREFIX(REX_B);
            text_section[text_section_length++] = 0xB8 + destination->reg - X64_Register_Tag_r8;
        }
        else
            text_section[text_section_length++] = 0xB8 + destination->reg;
    }
    else if (compare_types(source->type, u32_handle) || compare_types(source->type, i32_handle))
    {
        // MOV r32, imm32.
        if (destination->reg >= X64_Register_Tag_r8)
        {
            text_section[text_section_length++] = REX_PREFIX(REX_B);
            text_section[text_section_length++] = 0xB8 + destination->reg - X64_Register_Tag_r8;
        }
        else
            text_section[text_section_length++] = 0xB8 + destination->reg;
    }
    else if (compare_types(source->type, u64_handle) || compare_types(source->type, i64_handle))
    {
        // MOV r64, imm64.
        if (destination->reg >= X64_Register_Tag_r8)
        {
            text_section[text_section_length++] = REX_PREFIX(REX_W | REX_B);
            text_section[text_section_length++] = 0xB8 + destination->reg - X64_Register_Tag_r8;
        }
        else
        {
            text_section[text_section_length++] = REX_PREFIX(REX_W);
            text_section[text_section_length++] = 0xB8 + destination->reg;
        }
    }
    else
        unexpected_type(__FILE__, __LINE__, lookup_type(source->type)->tag);
    
    write_imm(source);
}

void mov(const Ir_Operand* destination, const Ir_Operand* source)
{
    if (destination->tag == Ir_Operand_Tag_x64_register && source->tag == Ir_Operand_Tag_immediate)
        mov_reg_imm(&destination->x64_reg, &source->imm);
    else
        ice(__FILE__, __LINE__, "Unexpected mov operands.");
}

