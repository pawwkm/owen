#include "x64.h"

static void ret(void)
{
    text_section[text_section_length++] = 0xC3;
}

static void assemble_return(Ir_Return_Instruction* instruction)
{
    Ir_Operand destination = { .x64_reg.tag = Ir_Operand_Tag_x64_register, .x64_reg.reg = X64_Register_Tag_rax };
    Ir_Operand* source = lookup_ir_operand(instruction->values.handles[0]);

    mov(&destination, source);
    ret();
}

static void assemble_block(Ir_Basic_Block_Handle block_handle)
{
    Ir_Basic_Block* block = lookup_ir_basic_block(block_handle);
    for (uint8_t i = 0; i < block->instructions.handles_length; i++)
    {
        Ir_Instruction* instruction = lookup_ir_instruction(block->instructions.handles[i]);
        if (instruction->tag == Ir_Instruction_Tag_return)
            assemble_return(&instruction->ret);
        else
            unexpected_instruction(__FILE__, __LINE__, instruction->tag);
    }
}

void assemble_function(Ir_Function_Handle handle)
{
    Ir_Function* function = lookup_ir_function(handle);
    assert(function->blocks.handles_length == 1);
    // check that there are no formal parameters
    // because I do not emit any prologue or epilogue.

    assemble_block(function->blocks.handles[0]);
}
