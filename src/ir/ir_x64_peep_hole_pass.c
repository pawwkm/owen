#include "ir.h"

void ir_x64_peep_hole_pass(void)
{
    for (uint16_t a = 0; a < ir_functions_length; a++)
    {
        Ir_Function* function = &ir_functions[a];
        for (uint8_t b = 0; b < function->blocks.handles_length; b++)
        {
            Ir_Basic_Block* block = lookup_ir_basic_block(function->blocks.handles[b]);
            for (uint8_t c = 0; c < block->ir.handles_length; c++)
            {
                Ir_Instruction* inst = lookup_ir_instruction(block->ir.handles[c]);
                if (inst->tag == Ir_Tag_x64_mov)
                {
                    Ir_Operand* destination = lookup_ir_operand(inst->destination);
                    Ir_Operand* source = lookup_ir_operand(inst->sources.handles[0]);
                    if (source->tag == destination->tag && IS_X64_REG(source->tag))
                        remove_ir_instruction_from_array(&block->ir, c--);
                    else if (IS_X64_REG(destination->tag) && source->tag == Ir_Operand_Tag_immediate && imm_is_zero(&source->imm, lookup_type(inst->type)->tag))
                    {
                        inst->tag = Ir_Tag_x64_xor;
                        inst->sources.handles[0] = inst->destination;
                    }
                }
            }
        }
    }
}