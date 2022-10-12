#include "ir.h"

static const Function_Type* type_of_callee(Ir_Call_Instruction* call)
{
    if (is_invalid_function_handle(call->callee_declaration))
        return &lookup_type(call->callee_type)->function;
    else
        return &lookup_type(lookup_function(call->callee_declaration)->signature)->function;
}

static void lower_parameter(Ir_Operand* parameter, uint8_t parameter_index, const Function_Type* signature)
{
    if (parameter_index > 3)
        not_implemented(__FILE__, __LINE__, "passing parameters by the stack");
    
    Type* parameter_type = lookup_type(type_at(&signature->formal_parameters, parameter_index));
    if (parameter_type->tag > Type_Tag_u64)
        not_implemented(__FILE__, __LINE__, "non-integer parameters");
    
    if (parameter_index == 0)
        parameter->tag = Ir_Operand_Tag_x64_rcx;
    else if (parameter_index == 1)
        parameter->tag = Ir_Operand_Tag_x64_rdx;
    else if (parameter_index == 2)
        parameter->tag = Ir_Operand_Tag_x64_r8;
    else if (parameter_index == 3)
        parameter->tag = Ir_Operand_Tag_x64_r9;
}

static void lower_return_value(Ir_Operand* value, uint8_t value_index, const Function_Type* signature)
{
    if (value_index)
        not_implemented(__FILE__, __LINE__, "Subsequent return values");
    else if (lookup_type(type_at(&signature->return_types, value_index))->tag > Type_Tag_u64)
        not_implemented(__FILE__, __LINE__, "Non-integer return types");
    
    value->tag = Ir_Operand_Tag_x64_rax;
}

static void do_block(Ir_Basic_Block* block, const Function_Type* signature)
{
    for (Array_Size a = 0; a < block->ir.handles_length; a++)
    {
        Ir_Instruction* inst = lookup_ir_instruction(ir_instruction_at(&block->ir, a));
        const Function_Type* callee_signature;
        Type_Handle_Array str;

        if (inst->tag == Ir_Tag_call)
        {
            callee_signature = type_of_callee(&inst->call);
            str = callee_signature->return_types;
        }
        else if (inst->tag == Ir_Tag_return)
            str = signature->return_types;
        else
            continue;

        for (Array_Size b = 0; b < inst->sources.handles_length; b++)
        {
            Ir_Instruction_Handle mov_handle = add_ir_instruction();
            Ir_Instruction* mov = lookup_ir_instruction(mov_handle);
            mov->tag = Ir_Tag_x64_mov;
            mov->type = type_at(&str, b);
            
            add_to_ir_operand_array(&mov->sources, ir_operand_at(&inst->sources, b));
            mov->destination = add_ir_operand();
            
            insert_ir_instruction_in_array(&block->ir, mov_handle, a++);
            
            Ir_Operand* destination = lookup_ir_operand(mov->destination);
            if (inst->tag == Ir_Tag_call)
                lower_parameter(destination, b, callee_signature);
            else
                lower_return_value(destination, b, signature);
        }
    }
}

static void correct_return_values(Ir_Basic_Block* block)
{
    for (Array_Size a = 0; a < block->ir.handles_length; a++)
    {
        // Modify the references to the return types.
        // if I can do this here then I don't have to go through every node that could refer to these values...
        
        // I need to do this at the references to the call and not at the call site.
        Ir_Instruction* inst = lookup_ir_instruction(ir_instruction_at(&block->ir, a));
        for (Array_Size b = 0; b < inst->sources.handles_length; b++)
        {
            Ir_Operand* source = lookup_ir_operand(ir_operand_at(&inst->sources, b));
            if (source->tag != Ir_Operand_Tag_vreg)
                continue;

            Ir_Call_Instruction* call = &lookup_ir_instruction(source->vreg.ir)->call;
            if (call->tag != Ir_Tag_call)
                continue;
    
            lower_return_value(source, 0, type_of_callee(call));
        }
    }
}

static void lower_formal_parameters(Ir_Basic_Block* block, const Function_Type* signature)
{
    for (uint8_t i = 0; i < block->definitions_length; i++)
    {
        Ir_Operand* operand = lookup_ir_operand(block->definitions[i].value);
        if (operand->tag != Ir_Operand_Tag_parameter)
            continue;
        
        lower_parameter(operand, operand->parameter.index, signature);
    }
}

static void do_function(const Ir_Function* function, const Function_Type* function_type)
{
    if (function->blocks.handles_length > 1)
        ice(__FILE__, __LINE__, "It is assumed that there is only 1 basic block and thats the one with the return instruction.");
    else if (!function->blocks.handles_length)
        return;
    
    lower_formal_parameters(lookup_ir_basic_block(ir_basic_block_at(&function->blocks, 0)), function_type);
    for (Array_Size i = 0; i < function->blocks.handles_length; i++)
    {
        Ir_Basic_Block* block = lookup_ir_basic_block(ir_basic_block_at(&function->blocks, i));
        do_block(block, function_type);
        correct_return_values(block);
    }
}

void ir_abi_pass(void)
{
    for (uint16_t i = 0; i < functions_length; i++)
    {
        Function_Type* function_type = &lookup_type(functions[i].signature)->function;
        Ir_Function* function = lookup_ir_function(functions[i].ir);
        
        do_function(function, function_type);
    }
}