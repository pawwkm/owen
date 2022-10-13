#include "x64.h"

static void assemble_block(Ir_Basic_Block_Handle block_handle)
{
    Ir_Basic_Block* block = lookup(block_handle);
    for (Array_Size i = 0; i < block->ir.handles_length; i++)
    {
        Ir_Instruction* inst = lookup_in(&block->ir, i);
        if (inst->tag == Ir_Tag_x64_call)
            x64_call(&inst->x64_call);
        else if (inst->tag == Ir_Tag_x64_ret)
            x64_ret();
        else if (inst->tag == Ir_Tag_x64_mov)
            x64_mov(inst);
        else if (inst->tag == Ir_Tag_x64_xor)
            x64_xor(inst);
        else
            unexpected_ir_instruction(__FILE__, __LINE__, inst->tag);
    }
}

void assemble_function(Ir_Function* function)
{
    assert(function->blocks.handles_length == 1);
    assemble_block(handle_at(&function->blocks, 0));
}
