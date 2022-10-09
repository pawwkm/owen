#include "expressions.h"

Ir_Operand_Handle lower_cast(Ir_Basic_Block* block, const Cast* cast)
{
    Type_Handle source_type = lookup_expression(cast->source)->type;
    Ir_Operand_Handle source = lower_expression(block, cast->source);
    if (compare_types(cast->type, source_type))
        return source;

    not_implemented(__FILE__, __LINE__, "Casting distinct types");
    return invalid_ir_operand_handle;
}
