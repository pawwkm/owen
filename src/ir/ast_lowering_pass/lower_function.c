#include "ast_lowering_pass.h"

Ir_Function_Handle lower_function(Function* function)
{
    if (invalid(function->ir))
    {
        function->ir = add_ir_function();
        Function_Type* signature = &lookup(function->signature)->function;
        Ir_Function* ir_function = lookup(function->ir);
     
        add_to(&ir_function->blocks, add_ir_basic_block());
        Ir_Basic_Block* block = lookup_in(&ir_function->blocks, ir_function->blocks.handles_length - 1);   
        
        for (Array_Size i = 0; i < signature->formal_parameters.handles_length; i++)
        {
            Ir_Operand_Handle parameter_handle = add_ir_operand();
            Ir_Parameter* parameter = &lookup(parameter_handle)->parameter;
            parameter->tag = Ir_Operand_Tag_parameter;
            parameter->index = i;
            
            write_definition(block, (Ir_Definition)
            {
                .name = lookup_in(&function->formal_parameters, i)->name, 
                .value = parameter_handle
            });
        }

        lower_statements(block, &function->body);
    }

    return function->ir;
}
