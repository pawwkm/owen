#include "x64.h"

static void xor_reg_reg(uint8_t destination, uint8_t source)
{
    if (destination == source)
    {
        // XOR r32, r32.
        text_section[text_section_length++] = 0x31;
        text_section[text_section_length++] = MOD_RM(MOD_REGISTER_ADDRESSING, source, source);
    }
    else
        not_implemented(__FILE__, __LINE__, "Differing xor registers");
}

void xor(const Ir_Operand* destination, const Ir_Operand* source)
{
    if (destination->tag == Ir_Operand_Tag_x64_register && source->tag == Ir_Operand_Tag_x64_register)
        xor_reg_reg(destination->x64_reg.reg, source->x64_reg.reg);
    else
        ice(__FILE__, __LINE__, "Unexpected xor operands.");
}
