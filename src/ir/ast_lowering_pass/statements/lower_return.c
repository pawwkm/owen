#include "statements.h"

void lower_return_statement(Ir_Basic_Block* block, const Expression_Handle_Array* values)
{
    Ir_Instruction_Handle handle = add_ir_instruction();
    Ir_Instruction* ret = lookup(handle);
    ret->tag = Ir_Tag_return;
    ret->type = invalid_type_handle;
    
    lower_expressions(block, &ret->sources, values);
    add_to(&block->ir, handle);
}
