#include "../compiler.h"

Ir_Operand_Handle x64_reg_rax;
Ir_Operand_Handle x64_reg_rcx;
Ir_Operand_Handle x64_reg_rdx;
Ir_Operand_Handle x64_reg_rbx;
Ir_Operand_Handle x64_reg_sp;
Ir_Operand_Handle x64_reg_bp;
Ir_Operand_Handle x64_reg_si;
Ir_Operand_Handle x64_reg_r8;
Ir_Operand_Handle x64_reg_r9;
Ir_Operand_Handle x64_reg_r10;
Ir_Operand_Handle x64_reg_r11;
Ir_Operand_Handle x64_reg_r12;
Ir_Operand_Handle x64_reg_r13;
Ir_Operand_Handle x64_reg_r14;
Ir_Operand_Handle x64_reg_r15;

void write_definition(Ir_Basic_Block* block, Ir_Definition definition);
Ir_Operand_Handle read_definition(const Ir_Basic_Block* block, Interned_String_Handle name);
void ir_x64_peep_hole_pass(void);
