#include "ast_lowering_pass.h"

Ir_Function_Handle lower_function(Function* function)
{
    if (is_invalid_ir_function_handle(function->ir))
    {
        function->ir = add_ir_function();
        Function_Type* signature = &lookup_type(function->signature)->function;
        Ir_Function* ir_function = lookup_ir_function(function->ir);
     
        add_to_ir_basic_block_array(&ir_function->blocks, add_ir_basic_block());
        Ir_Basic_Block* block = lookup_ir_basic_block(ir_function->blocks.handles[ir_function->blocks.handles_length - 1]);   
        
        for (uint8_t i = 0; i < signature->formal_parameters.handles_length; i++)
        {
            Ir_Operand_Handle parameter_handle = add_ir_operand();
            Ir_Parameter* parameter = &lookup_ir_operand(parameter_handle)->parameter;
            parameter->tag = Ir_Operand_Tag_parameter;
            parameter->index = i;
            
            write_definition(block, (Ir_Definition)
            {
                .name = lookup_formal_parameter(function->formal_parameters.handles[i])->name, 
                .value = parameter_handle
            });
        }

        lower_statements(block, &function->body);
    }

    return function->ir;
}
