#include "ir.h"

static const Function_Type* type_of_callee(Ir_Call_Instruction* call)
{
    if (invalid(call->callee_declaration))
        return &lookup(call->callee_type)->function;
    else
        return &lookup(lookup(call->callee_declaration)->signature)->function;
}

static void lower_parameter(Ir_Operand_Handle* parameter, uint8_t parameter_index, const Function_Type* signature)
{
    if (parameter_index > 3)
        not_implemented(__FILE__, __LINE__, "passing parameters by the stack");
    
    Type* parameter_type = lookup_in(&signature->formal_parameters, parameter_index);
    if (parameter_type->tag > Type_Tag_u64)
        not_implemented(__FILE__, __LINE__, "non-integer parameters");
    
    if (parameter_index == 0)
        *parameter = x64_reg_rcx;
    else if (parameter_index == 1)
        *parameter = x64_reg_rdx;
    else if (parameter_index == 2)
        *parameter = x64_reg_r8;
    else if (parameter_index == 3)
        *parameter = x64_reg_r9;
}

static void lower_return_value(Ir_Operand_Handle* value, uint8_t value_index, const Function_Type* signature)
{
    if (value_index)
        not_implemented(__FILE__, __LINE__, "Subsequent return values");
    else if (lookup_in(&signature->return_types, value_index)->tag > Type_Tag_u64)
        not_implemented(__FILE__, __LINE__, "Non-integer return types");
    
    *value = x64_reg_rax;
}

static void do_block(Ir_Basic_Block* block, const Function_Type* signature)
{
    for (Array_Size a = 0; a < block->ir.handles_length; a++)
    {
        Ir_Instruction* inst = lookup_in(&block->ir, a);
        const Function_Type* callee_signature;
        Type_Handle_Array return_types;

        if (inst->tag == Ir_Tag_call)
        {
            callee_signature = type_of_callee(&inst->call);
            return_types = callee_signature->return_types;
        }
        else if (inst->tag == Ir_Tag_return)
            return_types = signature->return_types;
        else
            continue;

        for (Array_Size b = 0; b < inst->sources.handles_length; b++)
        {
            Ir_Instruction_Handle mov_handle = add_ir_instruction();
            Ir_Instruction* mov = lookup(mov_handle);
            mov->tag = Ir_Tag_x64_mov;
            mov->type = handle_at(&return_types, b);
            
            add_to(&mov->sources, handle_at(&inst->sources, b));
            mov->destination = add_ir_operand();
            
            insert(&block->ir, mov_handle, a++);
            
            if (inst->tag == Ir_Tag_call)
                lower_parameter(&mov->destination, b, callee_signature);
            else
                lower_return_value(&mov->destination, b, signature);
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
        Ir_Instruction* inst = lookup_in(&block->ir, a);
        for (Array_Size b = 0; b < inst->sources.handles_length; b++)
        {
            Ir_Operand* source = lookup_in(&inst->sources, b);
            if (source->tag != Ir_Operand_Tag_vreg)
                continue;

            Ir_Call_Instruction* call = &lookup(source->vreg.ir)->call;
            if (call->tag != Ir_Tag_call)
                continue;
            
            Ir_Operand_Handle handle;
            lower_return_value(&handle, 0, type_of_callee(call));
            replace(&inst->sources, handle, b);
        }
    }
}

static void lower_formal_parameters(Ir_Basic_Block* block, const Function_Type* signature)
{
    for (uint8_t i = 0; i < block->definitions_length; i++)
    {
        Ir_Operand_Handle* handle = &block->definitions[i].value;
        Ir_Operand* operand = lookup(*handle);
        if (operand->tag != Ir_Operand_Tag_parameter)
            continue;

        Ir_Operand_Handle temp;
        lower_parameter(&temp, operand->parameter.index, signature);

        *operand = *lookup(temp);
    }
}

static void do_function(const Ir_Function* function, const Function_Type* function_type)
{
    if (function->blocks.handles_length > 1)
        ice(__FILE__, __LINE__, "It is assumed that there is only 1 basic block and thats the one with the return instruction.");
    else if (!function->blocks.handles_length)
        return;
    
    for (Array_Size i = 0; i < function->blocks.handles_length; i++)
    {
        Ir_Basic_Block* block = lookup_in(&function->blocks, i);
        lower_formal_parameters(block, function_type);
        do_block(block, function_type);
        correct_return_values(block);
    }
}

void ir_abi_pass(void)
{
    x64_reg_rax = add_ir_operand();
    lookup(x64_reg_rax)->tag = Ir_Operand_Tag_x64_rax;

    x64_reg_rcx = add_ir_operand();
    lookup(x64_reg_rcx)->tag = Ir_Operand_Tag_x64_rcx;

    x64_reg_rdx = add_ir_operand();
    lookup(x64_reg_rdx)->tag = Ir_Operand_Tag_x64_rdx;

    x64_reg_rbx = add_ir_operand();
    lookup(x64_reg_rbx)->tag = Ir_Operand_Tag_x64_rbx;

    x64_reg_sp = add_ir_operand();
    lookup(x64_reg_sp)->tag = Ir_Operand_Tag_x64_sp;

    x64_reg_bp = add_ir_operand();
    lookup(x64_reg_bp)->tag = Ir_Operand_Tag_x64_bp;

    x64_reg_si = add_ir_operand();
    lookup(x64_reg_si)->tag = Ir_Operand_Tag_x64_si;

    x64_reg_r8 = add_ir_operand();
    lookup(x64_reg_r8)->tag = Ir_Operand_Tag_x64_r8;

    x64_reg_r9 = add_ir_operand();
    lookup(x64_reg_r9)->tag = Ir_Operand_Tag_x64_r9;

    x64_reg_r10 = add_ir_operand();
    lookup(x64_reg_r10)->tag = Ir_Operand_Tag_x64_r10;

    x64_reg_r11 = add_ir_operand();
    lookup(x64_reg_r11)->tag = Ir_Operand_Tag_x64_r11;

    x64_reg_r12 = add_ir_operand();
    lookup(x64_reg_r12)->tag = Ir_Operand_Tag_x64_r12;

    x64_reg_r13 = add_ir_operand();
    lookup(x64_reg_r13)->tag = Ir_Operand_Tag_x64_r13;

    x64_reg_r14 = add_ir_operand();
    lookup(x64_reg_r14)->tag = Ir_Operand_Tag_x64_r14;

    x64_reg_r15 = add_ir_operand();
    lookup(x64_reg_r15)->tag = Ir_Operand_Tag_x64_r15;

    for (uint16_t i = 0; i < functions_length; i++)
    {
        Function_Type* function_type = &lookup(functions[i].signature)->function;
        Ir_Function* function = lookup(functions[i].ir);
        
        do_function(function, function_type);
    }
}