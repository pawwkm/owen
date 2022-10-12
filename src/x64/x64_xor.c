#include "x64.h"

static void xor_reg_reg(uint8_t destination, uint8_t source, Type_Handle type)
{
    if ((compare_types(type, u32_handle) || compare_types(type, i32_handle)) && destination == source)
    {
        // XOR r32, r32.
        write_text_u8(0x31);
        write_text_u8(MOD_RM(MOD_REGISTER_ADDRESSING, source, source));
    }
    else
        not_implemented(__FILE__, __LINE__, "Differing xor registers");
}

void x64_xor(const Ir_Instruction* xor)
{
    Ir_Operand* source = lookup_ir_operand(ir_operand_at(&xor->sources, 0));
    Ir_Operand* destination = lookup_ir_operand(xor->destination);

    if (IS_X64_REG(destination->tag) && IS_X64_REG(source->tag))
        xor_reg_reg(destination->tag, source->tag, xor->type);
    else
        ice(__FILE__, __LINE__, "Unexpected xor operands.");
}

uint16_t size_of_x64_xor(const Ir_Instruction* xor)
{
    Ir_Operand* source = lookup_ir_operand(ir_operand_at(&xor->sources, 0));
    Ir_Operand* destination = lookup_ir_operand(xor->destination);
    
    if (IS_X64_REG(destination->tag) && IS_X64_REG(source->tag))
    {
        if ((compare_types(xor->type, u32_handle) || compare_types(xor->type, i32_handle)) && destination->tag == source->tag)
            // XOR r32, r32.
            return 2;
        else
            not_implemented(__FILE__, __LINE__, "Differing xor registers");
    }
    else
        ice(__FILE__, __LINE__, "Unexpected xor operands.");
    
    return 0;
}