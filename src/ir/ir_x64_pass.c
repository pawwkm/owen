#include "ir.h"

static void lower_instructions(Ir_Function* function)
{   
    for (uint8_t a = 0; a < function->blocks.handles_length; a++)
    {
        Ir_Basic_Block* block = lookup_ir_basic_block(function->blocks.handles[a]);
        for (uint8_t b = 0; b < block->ir.handles_length; b++)
        {
            Ir_Instruction* inst = lookup_ir_instruction(block->ir.handles[b]);
            if (inst->tag == Ir_Tag_call)
            {
                if (is_invalid_function_handle(inst->call.callee_declaration))
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
    for (uint8_t a = 0; a < function->blocks.handles_length; a++)
    {
        Ir_Basic_Block* block = lookup_ir_basic_block(function->blocks.handles[a]);
        for (uint8_t b = 0; b < block->ir.handles_length; b++)
            block->size_of_ir_in_bytes += size_of_x64_ir(lookup_ir_instruction(block->ir.handles[b]));

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
    for (uint8_t a = 0; a < function->blocks.handles_length; a++)
    {
        Ir_Basic_Block* block = lookup_ir_basic_block(function->blocks.handles[a]);
        for (uint8_t b = 0; b < block->ir.handles_length; b++)
        {
            Ir_X64_Call* call = &lookup_ir_instruction(block->ir.handles[b])->x64_call;
            if (call->tag != Ir_Tag_x64_call)
                continue;
            
            call->absolute_address = lookup_ir_function(lookup_function(call->callee_declaration)->ir)->absolute_address;
        }
    }
}

void ir_x64_pass(void)
{
    x64_reg_rax = add_ir_operand();
    lookup_ir_operand(x64_reg_rax)->tag = Ir_Operand_Tag_x64_rax;
    
    x64_reg_rcx = add_ir_operand();
    lookup_ir_operand(x64_reg_rcx)->tag = Ir_Operand_Tag_x64_rcx;
    
    x64_reg_rdx = add_ir_operand();
    lookup_ir_operand(x64_reg_rdx)->tag = Ir_Operand_Tag_x64_rdx;
    
    x64_reg_rbx = add_ir_operand();
    lookup_ir_operand(x64_reg_rbx)->tag = Ir_Operand_Tag_x64_rbx;
    
    x64_reg_sp = add_ir_operand();
    lookup_ir_operand(x64_reg_sp)->tag = Ir_Operand_Tag_x64_sp;
    
    x64_reg_bp = add_ir_operand();
    lookup_ir_operand(x64_reg_bp)->tag = Ir_Operand_Tag_x64_bp;
    
    x64_reg_si = add_ir_operand();
    lookup_ir_operand(x64_reg_si)->tag = Ir_Operand_Tag_x64_si;
    
    x64_reg_r8 = add_ir_operand();
    lookup_ir_operand(x64_reg_r8)->tag = Ir_Operand_Tag_x64_r8;
    
    x64_reg_r9 = add_ir_operand();
    lookup_ir_operand(x64_reg_r9)->tag = Ir_Operand_Tag_x64_r9;
    
    x64_reg_r10 = add_ir_operand();
    lookup_ir_operand(x64_reg_r10)->tag = Ir_Operand_Tag_x64_r10;
    
    x64_reg_r11 = add_ir_operand();
    lookup_ir_operand(x64_reg_r11)->tag = Ir_Operand_Tag_x64_r11;
    
    x64_reg_r12 = add_ir_operand();
    lookup_ir_operand(x64_reg_r12)->tag = Ir_Operand_Tag_x64_r12;
    
    x64_reg_r13 = add_ir_operand();
    lookup_ir_operand(x64_reg_r13)->tag = Ir_Operand_Tag_x64_r13;
    
    x64_reg_r14 = add_ir_operand();
    lookup_ir_operand(x64_reg_r14)->tag = Ir_Operand_Tag_x64_r14;
    
    x64_reg_r15 = add_ir_operand();
    lookup_ir_operand(x64_reg_r15)->tag = Ir_Operand_Tag_x64_r15;
    
    ordered_ir_function_iteration(lower_instructions);

    ir_x64_peep_hole_pass();

    ordered_ir_function_iteration(find_function_size);
    ordered_ir_function_iteration(find_function_offset);
    ordered_ir_function_iteration(set_call_address);
}