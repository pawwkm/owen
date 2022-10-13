#include "expressions.h"

Ir_Operand_Handle lower_call(Ir_Basic_Block* block, const Call* call)
{
    Ir_Instruction_Handle inst_handle = add_ir_instruction();
    Ir_Call_Instruction* inst = &lookup(inst_handle)->call;
    inst->tag = Ir_Tag_call;

    if (!invalid(call->callee_declaration))
    {
        inst->callee_type = invalid_type_handle;
        inst->callee_declaration = call->callee_declaration;
        lower_function(lookup(call->callee_declaration));
    }
    else
    {
        inst->callee_type = lookup(call->callee)->type;
        inst->callee_declaration = invalid_function_handle;
        lower_expression(block, call->callee); 
    }
       
    lower_expressions(block, &inst->sources, &call->actual_parameters);
    if (compare(call->type, none_handle))
        return invalid_ir_operand_handle;
    
    Ir_Operand_Handle vreg_handle = add_ir_operand();
    Ir_Virtual_Register* vreg = &lookup(vreg_handle)->vreg;

    vreg->tag = Ir_Operand_Tag_vreg;
    vreg->ir = inst_handle;

    add_to(&block->ir, inst_handle);

    return vreg_handle;
}
