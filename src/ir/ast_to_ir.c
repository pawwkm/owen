#include "ir.h"

static Ir_Operand_Handle lower_expression(Expression_Handle handle)
{
    Expression* expression = lookup_expression(handle);
    if (expression->tag == Expression_Tag_integer)
    {
        Ir_Operand_Handle h = add_ir_operand();
        Ir_Immediate* imm = &lookup_ir_operand(h)->imm;
        
        imm->tag = Ir_Operand_Tag_immediate;
        imm->value = expression->number.value;
        imm->type = expression->type;

        return h;
    }
    else
        unexpected_expression(__FILE__, __LINE__, expression->tag);

    return invalid_ir_operand_handle;
}

static void lower_return_statement(Ir_Basic_Block* block, const Expression_Handle_Array* values)
{
    Ir_Instruction_Handle handle = add_ir_instruction();
    Ir_Return_Instruction* ret = &lookup_ir_instruction(handle)->ret;
    ret->tag = Ir_Instruction_Tag_return;

    add_to_ir_instruction_array(&block->instructions, handle);
    for (uint8_t i = 0; i < values->handles_length; i++)
        add_to_ir_operand_array(&ret->values, lower_expression(values->handles[i]));
}

static void lower_statements(const Statement_Handle_Array* body, Ir_Basic_Block_Handle_Array* blocks)
{
    add_to_ir_basic_block_array(blocks, add_ir_basic_block());
    Ir_Basic_Block* block = lookup_ir_basic_block(blocks->handles[blocks->handles_length - 1]);
    for (uint8_t i = 0; i < body->handles_length; i++)
    {
        Statement* statement = lookup_statement(body->handles[i]);
        if (statement->tag == Statement_Tag_return)
            lower_return_statement(block, &statement->return_statement.values);
        else
            unexpected_statement(__FILE__, __LINE__, statement->tag);
    }
}

static Ir_Function_Handle lower_function(Function_Handle handle)
{
    Function* function = lookup_function(handle);
    if (is_invalid_ir_function_handle(function->ir))
    {
        function->ir = add_ir_function();
        lower_statements(&function->body, &lookup_ir_function(function->ir)->blocks);
    }

    return function->ir;
}

// https://pp.info.uni-karlsruhe.de/uploads/publikationen/braun13cc.pdf
void lower_ast(void)
{
    lower_function(main_function);
}
