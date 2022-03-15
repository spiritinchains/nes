
#include "cpu.h"
#include "cpubus.h"

#include <assert.h>

static struct opc_record opc_table[] = {
    // Add with Carry
    [0x69] = { OPC_ADC, ADDR_IMMEDIATE, 2, 2, 0, 0, ins_adc },
    [0x65] = { OPC_ADC, ADDR_ZEROPAGE, 2, 3, 0, 0, ins_adc },
    [0x75] = { OPC_ADC, ADDR_ZEROPAGE_X, 2, 4, 0, 0, ins_adc },
    [0x6D] = { OPC_ADC, ADDR_ABSOLUTE, 3, 4, 0, 0, ins_adc },
    [0x7D] = { OPC_ADC, ADDR_ABSOLUTE_X, 3, 4, 1, 0, ins_adc },
    [0x79] = { OPC_ADC, ADDR_ABSOLUTE_Y, 3, 4, 1, 0, ins_adc },
    [0x61] = { OPC_ADC, ADDR_INDIRECT_X, 2, 6, 0, 0, ins_adc },
    [0x71] = { OPC_ADC, ADDR_INDIRECT_Y, 2, 5, 1, 0, ins_adc },
    // Bitwise AND
    [0x29] = { OPC_AND, ADDR_IMMEDIATE, 2, 2, 0, 0, ins_and },
    [0x25] = { OPC_AND, ADDR_ZEROPAGE, 2, 3, 0, 0, ins_and },
    [0x35] = { OPC_AND, ADDR_ZEROPAGE_X, 2, 4, 0, 0, ins_and },
    [0x2D] = { OPC_AND, ADDR_ABSOLUTE, 3, 4, 0, 0, ins_and },
    [0x3D] = { OPC_AND, ADDR_ABSOLUTE_X, 3, 4, 1, 0, ins_and },
    [0x39] = { OPC_AND, ADDR_ABSOLUTE_Y, 3, 4, 1, 0, ins_and },
    [0x21] = { OPC_AND, ADDR_INDIRECT_X, 2, 6, 0, 0, ins_and },
    [0x31] = { OPC_AND, ADDR_INDIRECT_Y, 2, 5, 1, 0, ins_and },
    // Arithmetic Shift Left
    [0x0A] = { OPC_ASL, ADDR_ACCUMULATOR, 1, 2, 0, 0, ins_asl },
    [0x06] = { OPC_ASL, ADDR_ZEROPAGE, 2, 5, 0, 0, ins_asl },
    [0x16] = { OPC_ASL, ADDR_ZEROPAGE_X, 2, 6, 0, 0, ins_asl },
    [0x0E] = { OPC_ASL, ADDR_ABSOLUTE, 3, 6, 0, 0, ins_asl },
    [0x1E] = { OPC_ASL, ADDR_ABSOLUTE_X, 3, 7, 0, 0, ins_asl },
    // Branching
    [0x90] = { OPC_BCC, ADDR_RELATIVE, 2, 2, 0, 1, ins_bcc },
    [0xB0] = { OPC_BCS, ADDR_RELATIVE, 2, 2, 0, 1, ins_bcs },
    [0xF0] = { OPC_BEQ, ADDR_RELATIVE, 2, 2, 0, 1, ins_beq },
    [0x30] = { OPC_BMI, ADDR_RELATIVE, 2, 2, 0, 1, ins_bmi },
    [0xD0] = { OPC_BNE, ADDR_RELATIVE, 2, 2, 0, 1, ins_bne },
    [0x10] = { OPC_BPL, ADDR_RELATIVE, 2, 2, 0, 1, ins_bpl },
    [0x50] = { OPC_BVC, ADDR_RELATIVE, 2, 2, 0, 1, ins_bvc },
    [0x70] = { OPC_BVS, ADDR_RELATIVE, 2, 2, 0, 1, ins_bvs },
    // Bit Test
    [0x24] = { OPC_BIT, ADDR_ZEROPAGE, 2, 3, 0, 0, ins_bit },
    [0x2C] = { OPC_BIT, ADDR_ABSOLUTE, 3, 4, 0, 0, ins_bit },
    // Break / Interrupt
    [0x00] = { OPC_BRK, ADDR_IMPLIED, 1, 7, 0, 0, ins_brk },
    // Clear Flags
    [0x18] = { OPC_CLC, ADDR_IMPLIED, 1, 2, 0, 0, ins_clc },
    [0xD8] = { OPC_CLD, ADDR_IMPLIED, 1, 2, 0, 0, ins_cld },
    [0x58] = { OPC_CLI, ADDR_IMPLIED, 1, 2, 0, 0, ins_cli },
    [0xB8] = { OPC_CLV, ADDR_IMPLIED, 1, 2, 0, 0, ins_clv },
    // Compare
    [0xC9] = { OPC_CMP, ADDR_IMMEDIATE, 2, 2, 0, 0, ins_cmp },
    [0xC5] = { OPC_CMP, ADDR_ZEROPAGE, 2, 3, 0, 0, ins_cmp },
    [0xD5] = { OPC_CMP, ADDR_ZEROPAGE_X, 2, 4, 0, 0, ins_cmp },
    [0xCD] = { OPC_CMP, ADDR_ABSOLUTE, 3, 4, 0, 0, ins_cmp },
    [0xDD] = { OPC_CMP, ADDR_ABSOLUTE_X, 3, 4, 1, 0, ins_cmp },
    [0xD9] = { OPC_CMP, ADDR_ABSOLUTE_Y, 3, 4, 1, 0, ins_cmp },
    [0xC1] = { OPC_CMP, ADDR_INDIRECT_X, 2, 6, 0, 0, ins_cmp },
    [0xD1] = { OPC_CMP, ADDR_INDIRECT_Y, 2, 5, 1, 0, ins_cmp },
    // Compare with X
    [0xE0] = { OPC_CPX, ADDR_IMMEDIATE, 2, 2, 0, 0, ins_cpx },
    [0xE4] = { OPC_CPX, ADDR_ZEROPAGE, 2, 3, 0, 0, ins_cpx },
    [0xEC] = { OPC_CPX, ADDR_ABSOLUTE, 3, 4, 0, 0, ins_cpx },
    // Compare with Y
    [0xC0] = { OPC_CPY, ADDR_IMMEDIATE, 2, 2, 0, 0, ins_cpy },
    [0xC4] = { OPC_CPY, ADDR_ZEROPAGE, 2, 3, 0, 0, ins_cpy },
    [0xCC] = { OPC_CPY, ADDR_ABSOLUTE, 3, 4, 0, 0, ins_cpy },
    // Decrement
    [0xC6] = { OPC_DEC, ADDR_ZEROPAGE, 2, 5, 0, 0, ins_dec },
    [0xD6] = { OPC_DEC, ADDR_ZEROPAGE_X, 2, 6, 0, 0, ins_dec },
    [0xCE] = { OPC_DEC, ADDR_ABSOLUTE, 3, 6, 0, 0, ins_dec },
    [0xDE] = { OPC_DEC, ADDR_ABSOLUTE_X, 3, 7, 0, 0, ins_dec },
    [0xCA] = { OPC_DEX, ADDR_IMPLIED, 1, 2, 0, 0, ins_dex },
    [0x88] = { OPC_DEY, ADDR_IMPLIED, 1, 2, 0, 0, ins_dey },
    // Exclusive OR
    [0x49] = { OPC_EOR, ADDR_IMMEDIATE, 2, 2, 0, 0, ins_eor },
    [0x45] = { OPC_EOR, ADDR_ZEROPAGE, 2, 3, 0, 0, ins_eor },
    [0x55] = { OPC_EOR, ADDR_ZEROPAGE_X, 2, 4, 0, 0, ins_eor },
    [0x4D] = { OPC_EOR, ADDR_ABSOLUTE, 3, 4, 0, 0, ins_eor },
    [0x5D] = { OPC_EOR, ADDR_ABSOLUTE_X, 3, 4, 1, 0, ins_eor },
    [0x59] = { OPC_EOR, ADDR_ABSOLUTE_Y, 3, 4, 1, 0, ins_eor },
    [0x41] = { OPC_EOR, ADDR_INDIRECT_X, 2, 6, 0, 0, ins_eor },
    [0x51] = { OPC_EOR, ADDR_INDIRECT_Y, 2, 5, 1, 0, ins_eor },
    // Increment
    [0xE6] = { OPC_INC, ADDR_ZEROPAGE, 2, 5, 0, 0, ins_inc },
    [0xF6] = { OPC_INC, ADDR_ZEROPAGE_X, 2, 6, 0, 0, ins_inc },
    [0xEE] = { OPC_INC, ADDR_ABSOLUTE, 3, 6, 0, 0, ins_inc },
    [0xFE] = { OPC_INC, ADDR_ABSOLUTE_X, 3, 7, 0, 0, ins_inc },
    [0xE8] = { OPC_INX, ADDR_IMPLIED, 1, 2, 0, 0, ins_inx },
    [0xC8] = { OPC_INY, ADDR_IMPLIED, 1, 2, 0, 0, ins_iny },
    // Unconditional Jump
    [0x4C] = { OPC_JMP, ADDR_ABSOLUTE, 3, 3, 0, 0, ins_jmp },
    [0x6C] = { OPC_JMP, ADDR_INDIRECT, 3, 5, 0, 0, ins_jmp },
    // Jump to subroutine
    [0x20] = { OPC_JSR, ADDR_ABSOLUTE, 3, 6, 0, 0, ins_jsr },
    // Load Accumulator
    [0xA9] = { OPC_LDA, ADDR_IMMEDIATE, 2, 2, 0, 0, ins_lda },
    [0xA5] = { OPC_LDA, ADDR_ZEROPAGE, 2, 3, 0, 0, ins_lda },
    [0xB5] = { OPC_LDA, ADDR_ZEROPAGE_X, 2, 4, 0, 0, ins_lda },
    [0xAD] = { OPC_LDA, ADDR_ABSOLUTE, 3, 4, 0, 0, ins_lda },
    [0xBD] = { OPC_LDA, ADDR_ABSOLUTE_X, 3, 4, 1, 0, ins_lda },
    [0xB9] = { OPC_LDA, ADDR_ABSOLUTE_Y, 3, 4, 1, 0, ins_lda },
    [0xA1] = { OPC_LDA, ADDR_INDIRECT_X, 2, 6, 0, 0, ins_lda },
    [0xB1] = { OPC_LDA, ADDR_INDIRECT_Y, 2, 5, 1, 0, ins_lda },
    // Load X
    [0xA2] = { OPC_LDX, ADDR_IMMEDIATE, 2, 2, 0, 0, ins_ldx },
    [0xA6] = { OPC_LDX, ADDR_ZEROPAGE, 2, 3, 0, 0, ins_ldx },
    [0xB6] = { OPC_LDX, ADDR_ZEROPAGE_Y, 2, 4, 0, 0, ins_ldx },
    [0xAE] = { OPC_LDX, ADDR_ABSOLUTE, 3, 4, 0, 0, ins_ldx },
    [0xBE] = { OPC_LDX, ADDR_ABSOLUTE_Y, 3, 4, 1, 0, ins_ldx },
    // Load Y
    [0xA0] = { OPC_LDY, ADDR_IMMEDIATE, 2, 2, 0, 0, ins_ldy },
    [0xA4] = { OPC_LDY, ADDR_ZEROPAGE, 2, 3, 0, 0, ins_ldy },
    [0xB4] = { OPC_LDY, ADDR_ZEROPAGE_X, 2, 4, 0, 0, ins_ldy },
    [0xAC] = { OPC_LDY, ADDR_ABSOLUTE, 3, 4, 0, 0, ins_ldy },
    [0xBC] = { OPC_LDY, ADDR_ABSOLUTE_X, 3, 4, 1, 0, ins_ldy },
    // Logical Shift Right
    [0x4A] = { OPC_LSR, ADDR_ACCUMULATOR, 1, 2, 0, 0, ins_lsr },
    [0x46] = { OPC_LSR, ADDR_ZEROPAGE, 2, 5, 0, 0, ins_lsr },
    [0x56] = { OPC_LSR, ADDR_ZEROPAGE_X, 2, 6, 0, 0, ins_lsr },
    [0x4E] = { OPC_LSR, ADDR_ABSOLUTE, 3, 6, 0, 0, ins_lsr },
    [0x5E] = { OPC_LSR, ADDR_ABSOLUTE_X, 3, 7, 0, 0, ins_lsr },
    // No Operation
    [0xEA] = { OPC_NOP, ADDR_IMPLIED, 1, 2, 0, 0, ins_nop },
    // Bitwise OR
    [0x09] = { OPC_ORA, ADDR_IMMEDIATE, 2, 2, 0, 0, ins_ora },
    [0x05] = { OPC_ORA, ADDR_ZEROPAGE, 2, 3, 0, 0, ins_ora },
    [0x15] = { OPC_ORA, ADDR_ZEROPAGE_X, 2, 4, 0, 0, ins_ora },
    [0x0D] = { OPC_ORA, ADDR_ABSOLUTE, 3, 4, 0, 0, ins_ora },
    [0x1D] = { OPC_ORA, ADDR_ABSOLUTE_X, 3, 4, 1, 0, ins_ora },
    [0x19] = { OPC_ORA, ADDR_ABSOLUTE_Y, 3, 4, 1, 0, ins_ora },
    [0x01] = { OPC_ORA, ADDR_INDIRECT_X, 2, 6, 0, 0, ins_ora },
    [0x11] = { OPC_ORA, ADDR_INDIRECT_Y, 2, 5, 1, 0, ins_ora },
    // Stack Push/Pop
    [0x48] = { OPC_PHA, ADDR_IMPLIED, 1, 3, 0, 0, ins_pha },
    [0x08] = { OPC_PHP, ADDR_IMPLIED, 1, 3, 0, 0, ins_php },
    [0x68] = { OPC_PLA, ADDR_IMPLIED, 1, 4, 0, 0, ins_pla },
    [0x28] = { OPC_PLP, ADDR_IMPLIED, 1, 4, 0, 0, ins_plp },
    // Rotate Left
    [0x2A] = { OPC_ROL, ADDR_ACCUMULATOR, 1, 2, 0, 0, ins_rol },
    [0x26] = { OPC_ROL, ADDR_ZEROPAGE, 2, 5, 0, 0, ins_rol },
    [0x36] = { OPC_ROL, ADDR_ZEROPAGE_X, 2, 6, 0, 0, ins_rol },
    [0x2E] = { OPC_ROL, ADDR_ABSOLUTE, 3, 6, 0, 0, ins_rol },
    [0x3E] = { OPC_ROL, ADDR_ABSOLUTE_X, 3, 7, 0, 0, ins_rol },
    // Rotate Right
    [0x6A] = { OPC_ROR, ADDR_ACCUMULATOR, 1, 2, 0, 0, ins_ror },
    [0x66] = { OPC_ROR, ADDR_ZEROPAGE, 2, 5, 0, 0, ins_ror },
    [0x76] = { OPC_ROR, ADDR_ZEROPAGE_X, 2, 6, 0, 0, ins_ror },
    [0x6E] = { OPC_ROR, ADDR_ABSOLUTE, 3, 6, 0, 0, ins_ror },
    [0x7E] = { OPC_ROR, ADDR_ABSOLUTE_X, 3, 7, 0, 0, ins_ror },
    // Return from Interrupt
    [0x40] = { OPC_RTI, ADDR_IMPLIED, 1, 6, 0, 0, ins_rti },
    // Return from Subroutine
    [0x60] = { OPC_RTS, ADDR_IMPLIED, 1, 6, 0, 0, ins_rts },
    // Subtract with Carry
    [0xE9] = { OPC_SBC, ADDR_IMMEDIATE, 2, 2, 0, 0, ins_sbc },
    [0xE5] = { OPC_SBC, ADDR_ZEROPAGE, 2, 3, 0, 0, ins_sbc },
    [0xF5] = { OPC_SBC, ADDR_ZEROPAGE_X, 2, 4, 0, 0, ins_sbc },
    [0xED] = { OPC_SBC, ADDR_ABSOLUTE, 3, 4, 0, 0, ins_sbc },
    [0xFD] = { OPC_SBC, ADDR_ABSOLUTE_X, 3, 4, 1, 0, ins_sbc },
    [0xF9] = { OPC_SBC, ADDR_ABSOLUTE_Y, 3, 4, 1, 0, ins_sbc },
    [0xE1] = { OPC_SBC, ADDR_INDIRECT_X, 2, 6, 0, 0, ins_sbc },
    [0xF1] = { OPC_SBC, ADDR_INDIRECT_Y, 2, 5, 1, 0, ins_sbc },
    // Set Flags
    [0x38] = { OPC_SEC, ADDR_IMPLIED, 1, 2, 0, 0, ins_sec },
    [0xF8] = { OPC_SED, ADDR_IMPLIED, 1, 2, 0, 0, ins_sed },
    [0x78] = { OPC_SEI, ADDR_IMPLIED, 1, 2, 0, 0, ins_sei },
    // Store Accumulator
    [0x85] = { OPC_STA, ADDR_ZEROPAGE, 2, 3, 0, 0, ins_sta },
    [0x95] = { OPC_STA, ADDR_ZEROPAGE_X, 2, 4, 0, 0, ins_sta },
    [0x8D] = { OPC_STA, ADDR_ABSOLUTE, 3, 4, 0, 0, ins_sta },
    [0x9D] = { OPC_STA, ADDR_ABSOLUTE_X, 3, 5, 0, 0, ins_sta },
    [0x99] = { OPC_STA, ADDR_ABSOLUTE_Y, 3, 5, 0, 0, ins_sta },
    [0x81] = { OPC_STA, ADDR_INDIRECT_X, 2, 6, 0, 0, ins_sta },
    [0x91] = { OPC_STA, ADDR_INDIRECT_Y, 2, 6, 0, 0, ins_sta },
    // Store X
    [0x86] = { OPC_STX, ADDR_ZEROPAGE, 2, 3, 0, 0, ins_stx },
    [0x96] = { OPC_STX, ADDR_ZEROPAGE_Y, 2, 4, 0, 0, ins_stx },
    [0x8E] = { OPC_STX, ADDR_ABSOLUTE, 3, 4, 0, 0, ins_stx },
    // Store Y
    [0x84] = { OPC_STY, ADDR_ZEROPAGE, 2, 3, 0, 0, ins_sty },
    [0x94] = { OPC_STY, ADDR_ZEROPAGE_X, 2, 4, 0, 0, ins_sty },
    [0x8C] = { OPC_STY, ADDR_ABSOLUTE, 3, 4, 0, 0, ins_sty },
    // Transfer between Registers
    [0xAA] = { OPC_TAX, ADDR_IMPLIED, 1, 2, 0, 0, ins_tax },
    [0xA8] = { OPC_TAY, ADDR_IMPLIED, 1, 2, 0, 0, ins_tay },
    [0xBA] = { OPC_TSX, ADDR_IMPLIED, 1, 2, 0, 0, ins_tsx },
    [0x8A] = { OPC_TXA, ADDR_IMPLIED, 1, 2, 0, 0, ins_txa },
    [0x9A] = { OPC_TXS, ADDR_IMPLIED, 1, 2, 0, 0, ins_txs },
    [0x98] = { OPC_TYA, ADDR_IMPLIED, 1, 2, 0, 0, ins_tya },
    // TODO: add illegal opcodes
};

struct cpu_flags 
flags_unpack(uint8_t flags)
{
    struct cpu_flags r;

    r.N = (flags >> 7) & 1;
    r.V = (flags >> 6) & 1;
    r.B = (flags >> 4) & 1;
    r.D = (flags >> 3) & 1;
    r.I = (flags >> 2) & 1;
    r.Z = (flags >> 1) & 1;
    r.C = flags & 1;

    return r;
}

uint8_t 
flags_pack(struct cpu_flags flags)
{
    uint8_t r = 0;

    r |= flags.N << 7;
    r |= flags.V << 6;
    r |= flags.B << 4;
    r |= flags.D << 3;
    r |= flags.I << 2;
    r |= flags.Z << 1;
    r |= flags.C;

    return r;
}

struct cpu CPU;

void 
cpu_init()
{
    CPU.A = 0;
    CPU.X = 0;
    CPU.Y = 0;
    CPU.S = 0xFF;

    CPU.PC = 0xC000;

    CPU.state = 0;
    CPU.rem_bytes = 0;
    CPU.rem_cycles = 0;

    CPU.IR = 0;
    CPU.addr_mode = 0;

    CPU.addr = 0;
    CPU.data = 0;

    CPU.e_addr = 0;
}

void
cpu_cycle()
{
}


/* opcodes */


void ins_adc() { }
void ins_and() { }
void ins_asl() { }
void ins_bcc() { }
void ins_bcs() { }
void ins_beq() { }
void ins_bit() { }
void ins_bmi() { }
void ins_bne() { }
void ins_bpl() { }
void ins_brk() { }
void ins_bvc() { }
void ins_bvs() { }
void ins_clc() { }
void ins_cld() { }
void ins_cli() { }
void ins_clv() { }
void ins_cmp() { }
void ins_cpx() { }
void ins_cpy() { }
void ins_dec() { }
void ins_dex() { }
void ins_dey() { }
void ins_eor() { }
void ins_inc() { }
void ins_inx() { }
void ins_iny() { }
void ins_jmp() { }
void ins_jsr() { }
void ins_lda() { }
void ins_ldx() { }
void ins_ldy() { }
void ins_lsr() { }
void ins_nop() { }
void ins_ora() { }
void ins_pha() { }
void ins_php() { }
void ins_pla() { }
void ins_plp() { }
void ins_rol() { }
void ins_ror() { }
void ins_rti() { }
void ins_rts() { }
void ins_sbc() { }
void ins_sec() { }
void ins_sed() { }
void ins_sei() { }
void ins_sta() { }
void ins_stx() { }
void ins_sty() { }
void ins_tax() { }
void ins_tay() { }
void ins_tsx() { }
void ins_txa() { }
void ins_txs() { }
void ins_tya() { }

