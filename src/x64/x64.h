#include "../compiler.h"

// Table 2-4. REX Prefic fields [BITS: 0100WRXB]
//
// W | O = operand size determined by CS.D, 1 = 64 bit Operand size.
// R | Extension of the ModR/M reg field.
// X | Extension of the SIB index field.
// B | Extension of the ModR/M field, SIB base field or Opcode reg field.
#define REX_W 8
#define REX_R 4
#define REX_X 2
#define REX_B 1
#define REX_PREFIX(BITS) (0x40 | (BITS))

#define OPERAND_SIZE_PREFIX 0x66

#define MOD_INDIRECT_REGISTER_ADDRESSING 0
#define MOD_I8_OFFSET_ADDRESSING 1
#define MOD_I32_OFFSET_ADDRESSING 2
#define MOD_REGISTER_ADDRESSING 3
#define MOD_RM(MOD, REG, RM) ((MOD << 6) | (REG << 3) | RM)

void assemble_function(Ir_Function_Handle handle);
void mov(const Ir_Operand* destination, const Ir_Operand* source);
void xor(const Ir_Operand* destination, const Ir_Operand* source);
void write_imm(const Ir_Immediate* imm);
bool imm_is_zero(const Ir_Immediate* imm);
