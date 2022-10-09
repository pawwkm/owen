#include "expressions.h"

Ir_Operand_Handle lower_reference(Ir_Basic_Block* block, const Reference* reference)
{
    return read_definition(block, reference->name);
}
