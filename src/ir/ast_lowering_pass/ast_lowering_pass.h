#include "../ir.h"

void lower_expressions(Ir_Basic_Block* block, Ir_Operand_Handle_Array* destination, const Expression_Handle_Array* source);
Ir_Operand_Handle lower_expression(Ir_Basic_Block* block, Expression_Handle handle);
void lower_statements(Ir_Basic_Block* block, const Statement_Handle_Array* body);
Ir_Function_Handle lower_function(Function* handle);
