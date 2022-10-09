#include "../ast_lowering_pass.h"

Ir_Operand_Handle lower_reference(Ir_Basic_Block* block, const Reference* reference);
Ir_Operand_Handle lower_number_literal(const Number_Literal* literal);
Ir_Operand_Handle lower_call(Ir_Basic_Block* block, const Call* call);
Ir_Operand_Handle lower_cast(Ir_Basic_Block* block, const Cast* cast);

