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

void assemble_function(Ir_Function* function);

void x64_call(const Ir_X64_Call* call);
uint16_t size_of_x64_call();

void x64_mov(const Ir_Instruction* inst);
uint16_t size_of_x64_mov(const Ir_Instruction* inst);

void x64_xor(const Ir_Instruction* inst);
uint16_t size_of_x64_xor(const Ir_Instruction* inst);

void x64_ret(void);
uint16_t size_of_x64_ret(void);

void write_number(Number value, Type_Handle type);
void write_text_u8(uint8_t value);
void write_text_i8(int8_t value);
void write_text_i16(int16_t value);
void write_text_i32(int32_t value);
void write_text_i64(int64_t value);
void write_text_u8(uint8_t value);
void write_text_u16(uint16_t value);
void write_text_u32(uint32_t value);
void write_text_u64(uint64_t value);
