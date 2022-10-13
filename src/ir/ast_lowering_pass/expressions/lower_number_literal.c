#include "expressions.h"

Ir_Operand_Handle lower_number_literal(const Number_Literal* literal)
{
    Ir_Operand_Handle h = add_ir_operand();
    Ir_Immediate* imm = &lookup(h)->imm;

    imm->tag = Ir_Operand_Tag_immediate;
    imm->value = literal->value;

    return h;
}
