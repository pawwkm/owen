#include "ir.h"

static void lower_instructions(Ir_Function* function)
{   
    for (Array_Size a = 0; a < function->blocks.handles_length; a++)
    {
        Ir_Basic_Block* block = lookup_in(&function->blocks, a);
        for (Array_Size b = 0; b < block->ir.handles_length; b++)
        {
            Ir_Instruction* inst = lookup_in(&block->ir, b);
            if (inst->tag == Ir_Tag_call)
            {
                if (invalid(inst->call.callee_declaration))
                    not_implemented(__FILE__, __LINE__, "calling function expressions");

                inst->x64_call = (Ir_X64_Call)
                {
                    .tag = Ir_Tag_x64_call,
                    .callee_declaration = inst->call.callee_declaration,
                    .destination = invalid_ir_operand_handle,
                    .sources.handles_length = 0
                };
            }
            else if (inst->tag == Ir_Tag_return)
            {
                inst->tag = Ir_Tag_x64_ret;
                inst->destination = invalid_ir_operand_handle;
                inst->sources.handles_length = 0;
            }
        }
    }
}

static void find_function_size(Ir_Function* function)
{
    for (Array_Size a = 0; a < function->blocks.handles_length; a++)
    {
        Ir_Basic_Block* block = lookup_in(&function->blocks, a);
        for (Array_Size b = 0; b < block->ir.handles_length; b++)
            block->size_of_ir_in_bytes += size_of_x64_ir(lookup_in(&block->ir, b));

        function->size_of_blocks_in_bytes += block->size_of_ir_in_bytes;
    }
}

static void find_function_offset(Ir_Function* function)
{
    static uint32_t offset;
    
    function->absolute_address = offset;
    offset += function->size_of_blocks_in_bytes;
}

static void set_call_address(Ir_Function* function)
{
    for (Array_Size a = 0; a < function->blocks.handles_length; a++)
    {
        Ir_Basic_Block* block = lookup_in(&function->blocks, a);
        for (Array_Size b = 0; b < block->ir.handles_length; b++)
        {
            Ir_X64_Call* call = &lookup_in(&block->ir, b)->x64_call;
            if (call->tag != Ir_Tag_x64_call)
                continue;
            
            call->absolute_address = lookup(lookup(call->callee_declaration)->ir)->absolute_address;
        }
    }
}

void ir_x64_pass(void)
{   
    ordered_ir_function_iteration(lower_instructions);

    ir_x64_peep_hole_pass();

    ordered_ir_function_iteration(find_function_size);
    ordered_ir_function_iteration(find_function_offset);
    ordered_ir_function_iteration(set_call_address);
}