#include "ir.h"

void ir_x64_peep_hole_pass(void)
{
    for (uint16_t a = 0; a < ir_functions_length; a++)
    {
        Ir_Function* function = &ir_functions[a];
        for (Array_Size b = 0; b < function->blocks.handles_length; b++)
        {
            Ir_Basic_Block* block = lookup_in(&function->blocks, b);
            for (Array_Size c = 0; c < block->ir.handles_length; c++)
            {
                Ir_Instruction* inst = lookup_in(&block->ir, c);
                if (inst->tag == Ir_Tag_x64_mov)
                {
                    Ir_Operand* destination = lookup(inst->destination);
                    Ir_Operand* source = lookup_in(&inst->sources, 0);
                    if (source->tag == destination->tag && IS_X64_REG(source->tag))
                        remove(&block->ir, c--);
                    else if (IS_X64_REG(destination->tag) && source->tag == Ir_Operand_Tag_immediate && imm_is_zero(&source->imm, lookup(inst->type)->tag))
                    {
                        inst->tag = Ir_Tag_x64_xor;
                        replace(&inst->sources, inst->destination, 0);
                    }
                }
            }
        }
    }
}