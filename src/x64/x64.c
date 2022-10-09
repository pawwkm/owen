#include "x64.h"

void write_text_i8(int8_t value)
{
    write_text_u8(value);
}

void write_text_i16(int16_t value)
{
    write_text_u8(value       & 0xFF);
    write_text_u8(value >> 8  & 0xFF);
}

void write_text_i32(int32_t value)
{
    write_text_u8(value       & 0xFF);
    write_text_u8(value >> 8  & 0xFF);
    write_text_u8(value >> 16 & 0xFF);
    write_text_u8(value >> 24 & 0xFF);
}

void write_text_i64(int64_t value)
{
    write_text_u8(value       & 0xFF);
    write_text_u8(value >> 8  & 0xFF);
    write_text_u8(value >> 16 & 0xFF);
    write_text_u8(value >> 24 & 0xFF);
    write_text_u8(value >> 32 & 0xFF);
    write_text_u8(value >> 40 & 0xFF);
    write_text_u8(value >> 48 & 0xFF);
    write_text_u8(value >> 56 & 0xFF);
}

void write_text_u8(uint8_t value)
{
    text_section[text_section_length++] = value;
}

void write_text_u16(uint16_t value)
{
    write_text_u8(value       & 0xFF);
    write_text_u8(value >> 8  & 0xFF);
}

void write_text_u32(uint32_t value)
{
    write_text_u8(value       & 0xFF);
    write_text_u8(value >> 8  & 0xFF);
    write_text_u8(value >> 16 & 0xFF);
    write_text_u8(value >> 24 & 0xFF);
}

void write_text_u64(uint64_t value)
{
    write_text_u8(value       & 0xFF);
    write_text_u8(value >> 8  & 0xFF);
    write_text_u8(value >> 16 & 0xFF);
    write_text_u8(value >> 24 & 0xFF);
    write_text_u8(value >> 32 & 0xFF);
    write_text_u8(value >> 40 & 0xFF);
    write_text_u8(value >> 48 & 0xFF);
    write_text_u8(value >> 56 & 0xFF);
}

void write_number(Number value, Type_Handle type)
{
    if (compare_types(type, u8_handle))
        write_text_u8(value.u8);
    else if (compare_types(type, i8_handle))
        write_text_i8(value.i8);
    else if (compare_types(type, u16_handle))
        write_text_u16(value.u16);
    else if (compare_types(type, i16_handle))
        write_text_i16(value.i16);
    else if (compare_types(type, u32_handle))
        write_text_u32(value.u32);
    else if (compare_types(type, i32_handle))
        write_text_i32(value.i32);
    else if (compare_types(type, u64_handle))
        write_text_u64(value.u64);
    else if (compare_types(type, i64_handle))
        write_text_i64(value.i64);
    else
        assert(false);    
}

uint16_t size_of_x64_ir(const Ir_Instruction* inst)
{
    if (inst->tag == Ir_Tag_x64_mov)
        return size_of_x64_mov(inst);
    else if (inst->tag == Ir_Tag_x64_xor)
        return size_of_x64_xor(inst);
    else if (inst->tag == Ir_Tag_x64_call)
        return size_of_x64_call();
    else if (inst->tag == Ir_Tag_x64_ret)
        return size_of_x64_ret();
    else
        unexpected_ir_instruction(__FILE__, __LINE__, inst->tag);
    
    return 0;
}