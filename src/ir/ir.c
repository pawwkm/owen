#include "ir.h"

void ordered_ir_function_iteration(void (*func)(Ir_Function*))
{
    Ir_Function_Handle ir_main = lookup(main_function)->ir;
    func(lookup(ir_main));
    
    for (Ir_Function_Handle handle = { 0 }; handle.index < ir_functions_length; handle.index++)
    {
        if (!compare(ir_main, handle))
            func(lookup(handle));
    }
}

bool imm_is_zero(const Ir_Immediate* value, uint8_t tag)
{
    return tag == Type_Tag_i8  && value->value.i8  == 0 ||
           tag == Type_Tag_i16 && value->value.i16 == 0 ||
           tag == Type_Tag_i32 && value->value.i32 == 0 ||
           tag == Type_Tag_i64 && value->value.i64 == 0 ||
           tag == Type_Tag_u8  && value->value.u8  == 0 ||
           tag == Type_Tag_u16 && value->value.u16 == 0 ||
           tag == Type_Tag_u32 && value->value.u32 == 0 ||
           tag == Type_Tag_u64 && value->value.u64 == 0 ||
           tag == Type_Tag_f32 && value->value.f32 == 0 ||
           tag == Type_Tag_f64 && value->value.f64 == 0;
}

