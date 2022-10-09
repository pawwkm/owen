#include "x64.h"

static void mov_reg_imm(uint8_t destination, Number source, Type_Handle type)
{
    if (compare_types(type, u8_handle) || compare_types(type, i8_handle))
    {
        // MOV r8, imm8.
        if (destination >= Ir_Operand_Tag_x64_r8)
        {
            write_text_u8(REX_PREFIX(REX_B));
            write_text_u8(0xB0 + destination - Ir_Operand_Tag_x64_r8);
        }
        else if (destination >= Ir_Operand_Tag_x64_sp)
        {
            write_text_u8(REX_PREFIX(0));
            write_text_u8(0xB0 + destination);
        }
    }
    else if (compare_types(type, u16_handle) || compare_types(type, i16_handle))
    {
        // MOV r16, imm16.
        write_text_u8(OPERAND_SIZE_PREFIX);
        if (destination >= Ir_Operand_Tag_x64_r8)
        {
            write_text_u8(REX_PREFIX(REX_B));
            write_text_u8(0xB8 + destination - Ir_Operand_Tag_x64_r8);
        }
        else
            write_text_u8(0xB8 + destination);
    }
    else if (compare_types(type, u32_handle) || compare_types(type, i32_handle))
    {
        // MOV r32, imm32.
        if (destination >= Ir_Operand_Tag_x64_r8)
        {
            write_text_u8(REX_PREFIX(REX_B));
            write_text_u8(0xB8 + destination - Ir_Operand_Tag_x64_r8);
        }
        else
            write_text_u8(0xB8 + destination);
    }
    else if (compare_types(type, u64_handle) || compare_types(type, i64_handle))
    {
        // MOV r64, imm64.
        if (destination >= Ir_Operand_Tag_x64_r8)
        {
            write_text_u8(REX_PREFIX(REX_W | REX_B));
            write_text_u8(0xB8 + destination - Ir_Operand_Tag_x64_r8);
        }
        else
        {
            write_text_u8(REX_PREFIX(REX_W));
            write_text_u8(0xB8 + destination);
        }
    }
    else
        unexpected_type(__FILE__, __LINE__, lookup_type(type)->tag);
    
    write_number(source, type);
}

static void mov_reg_reg(uint8_t destination, uint8_t source, Type_Handle type)
{
    if (compare_types(type, u8_handle) || compare_types(type, i8_handle))
    {
        // MOV r8, r8.
        not_implemented(__FILE__, __LINE__, "MOV r8, r8");
    }
    else if (compare_types(type, u16_handle) || compare_types(type, i16_handle))
    {
        // MOV r16, r16.
        not_implemented(__FILE__, __LINE__, "MOV r16, r16");
    }
    else if (compare_types(type, u32_handle) || compare_types(type, i32_handle))
    {
        // MOV r32, r32.
        if (destination >= Ir_Operand_Tag_x64_r8 && source >= Ir_Operand_Tag_x64_r8)
        {
            write_text_u8(REX_PREFIX(REX_W | REX_R | REX_B));
            destination -= Ir_Operand_Tag_x64_r8;
            source -= Ir_Operand_Tag_x64_r8;
        }
        else if (destination >= Ir_Operand_Tag_x64_r8 || source >= Ir_Operand_Tag_x64_r8)
            ice(__FILE__, __LINE__, "Invalid mov register operands.");

        write_text_u8(0x89);
        write_text_u8(MOD_RM(MOD_REGISTER_ADDRESSING, source, destination));
    }
    else if (compare_types(type, u64_handle) || compare_types(type, i64_handle))
    {
        // MOV r64, r64.
        not_implemented(__FILE__, __LINE__, "MOV r64, r64");
    }
    else
        unexpected_type(__FILE__, __LINE__, lookup_type(type)->tag);
}

void x64_mov(const Ir_Instruction* mov)
{
    Ir_Operand* source = lookup_ir_operand(mov->sources.handles[0]);
    Ir_Operand* destination = lookup_ir_operand(mov->destination);
    
    if (IS_X64_REG(destination->tag) && source->tag == Ir_Operand_Tag_immediate)
        mov_reg_imm(destination->tag, source->imm.value, mov->type);
    else if (IS_X64_REG(destination->tag) && IS_X64_REG(source->tag))
        mov_reg_reg(destination->tag, source->tag, mov->type);
    else
        ice(__FILE__, __LINE__, "Unexpected mov operands.");
}

uint16_t size_of_x64_mov(const Ir_Instruction* mov)
{
    Ir_Operand* source = lookup_ir_operand(mov->sources.handles[0]);
    Ir_Operand* destination = lookup_ir_operand(mov->destination);
        
    if (IS_X64_REG(destination->tag) && source->tag == Ir_Operand_Tag_immediate)
    {
        if (compare_types(mov->type, u8_handle) || compare_types(mov->type, i8_handle))
            // MOV r8, imm8.
            return 3;
        else if (compare_types(mov->type, u16_handle) || compare_types(mov->type, i16_handle))
            // MOV r16, imm16.
            return destination->tag >= Ir_Operand_Tag_x64_r8 ? 5 : 4;
        else if (compare_types(mov->type, u32_handle) || compare_types(mov->type, i32_handle))
            // MOV r32, imm32.
            return destination->tag >= Ir_Operand_Tag_x64_r8 ? 6 : 5;
        else if (compare_types(mov->type, u64_handle) || compare_types(mov->type, i64_handle))
            // MOV r64, imm64.
            return 10;
        else
            unexpected_type(__FILE__, __LINE__, lookup_type(mov->type)->tag);
    }
    else if (IS_X64_REG(destination->tag) && IS_X64_REG(source->tag))
    {
        if (compare_types(mov->type, u32_handle) || compare_types(mov->type, i32_handle))
        {
            // MOV r32, r32.
            if (destination->tag >= Ir_Operand_Tag_x64_r8 && source->tag >= Ir_Operand_Tag_x64_r8)
                return 3;
            else if (destination->tag >= Ir_Operand_Tag_x64_r8 || source->tag >= Ir_Operand_Tag_x64_r8)
                ice(__FILE__, __LINE__, "Invalid mov register operands.");
            else
                return 2;
        }
        else
            unexpected_type(__FILE__, __LINE__, lookup_type(mov->type)->tag);
    }
    else
        ice(__FILE__, __LINE__, "Unexpected mov operands.");
    
    return 0;
}
