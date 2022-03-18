
#include "cpu.h"
#include "cpubus.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

struct cpu CPU;

static struct opc_record opc_table[256] = {
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
    [0x98] = { OPC_TYA, ADDR_IMPLIED, 1, 2, 0, 0, ins_tya }
    // TODO: add illegal opcodes
};

static char* str_mnemonics[] = {
	[OPC_ADC] = "ADC",
	[OPC_AND] = "AND",
	[OPC_ASL] = "ASL",
	[OPC_BCC] = "BCC",
	[OPC_BCS] = "BCS",
	[OPC_BEQ] = "BEQ",
	[OPC_BIT] = "BIT",
	[OPC_BMI] = "BMI",
	[OPC_BNE] = "BNE",
	[OPC_BPL] = "BPL",
	[OPC_BRK] = "BRK",
	[OPC_BVC] = "BVC",
	[OPC_BVS] = "BVS",
	[OPC_CLC] = "CLC",
	[OPC_CLD] = "CLD",
	[OPC_CLI] = "CLI",
	[OPC_CLV] = "CLV",
	[OPC_CMP] = "CMP",
	[OPC_CPX] = "CPX",
	[OPC_CPY] = "CPY",
	[OPC_DEC] = "DEC",
	[OPC_DEX] = "DEX",
	[OPC_DEY] = "DEY",
	[OPC_EOR] = "EOR",
	[OPC_INC] = "INC",
	[OPC_INX] = "INX",
	[OPC_INY] = "INY",
	[OPC_JMP] = "JMP",
	[OPC_JSR] = "JSR",
	[OPC_LDA] = "LDA",
	[OPC_LDX] = "LDX",
	[OPC_LDY] = "LDY",
	[OPC_LSR] = "LSR",
	[OPC_NOP] = "NOP",
	[OPC_ORA] = "ORA",
	[OPC_PHA] = "PHA",
	[OPC_PHP] = "PHP",
	[OPC_PLA] = "PLA",
	[OPC_PLP] = "PLP",
	[OPC_ROL] = "ROL",
	[OPC_ROR] = "ROR",
	[OPC_RTI] = "RTI",
	[OPC_RTS] = "RTS",
	[OPC_SBC] = "SBC",
	[OPC_SEC] = "SEC",
	[OPC_SED] = "SED",
	[OPC_SEI] = "SEI",
	[OPC_STA] = "STA",
	[OPC_STX] = "STX",
	[OPC_STY] = "STY",
	[OPC_TAX] = "TAX",
	[OPC_TAY] = "TAY",
	[OPC_TSX] = "TSX",
	[OPC_TXA] = "TXA",
	[OPC_TXS] = "TXS",
    [OPC_TYA] = "TYA",
    [OPC_UNK] = "???"
};

static char* str_addr_modes[] = {
	[ADDR_IMMEDIATE] = "#$%.2x",
	[ADDR_IMPLIED] = "",
	[ADDR_ACCUMULATOR] = "A",
	[ADDR_ABSOLUTE] = "$%.4x",
	[ADDR_ABSOLUTE_X] = "$%.4x,X",
	[ADDR_ABSOLUTE_Y] = "$%.4x,Y",
	[ADDR_ZEROPAGE] = "$%.2x",
	[ADDR_ZEROPAGE_X] = "$%.2x,X",
	[ADDR_ZEROPAGE_Y] = "$%.2x,Y",
	[ADDR_INDIRECT] = "($%.4x)",
	[ADDR_INDIRECT_X] = "($%.2x,X)",
	[ADDR_INDIRECT_Y] = "($%.2x),Y",
	[ADDR_RELATIVE] = "$%.2x"
};


void
print_opc(uint16_t addr)
{
    uint8_t bytes[4];

    char opcode_full[64] = "";
    char opcode_mnemonic[4] = "";
    char opcode_operands[16] = "";
    char opcode_bytes[16] = "";

    for (int i = 0; i < 4; i++)
    {
        bytes[i] = read8(addr + i);
    }

    int len = opc_table[bytes[0]].bytes;

    for (int i = 0; i < len; i++)
    {
        char tmp[4];
        sprintf(tmp, "%.2X ", bytes[i]);
        strcat(opcode_bytes, tmp);
    }

    enum addr_modes am = opc_table[bytes[0]].addr_mode;

    switch (am)
    {
        case ADDR_ABSOLUTE:
        case ADDR_ABSOLUTE_X:
        case ADDR_ABSOLUTE_Y:
        case ADDR_INDIRECT:
            sprintf(opcode_operands, str_addr_modes[am], *(uint16_t*)(&bytes[1]));
            break;
        case ADDR_IMMEDIATE:
        case ADDR_INDIRECT_X:
        case ADDR_INDIRECT_Y:
        case ADDR_ZEROPAGE:
        case ADDR_ZEROPAGE_X:
        case ADDR_ZEROPAGE_Y:
        case ADDR_RELATIVE:
            sprintf(opcode_operands, str_addr_modes[am], *(uint8_t*)(&bytes[1]));
            break;
        case ADDR_IMPLIED:
        case ADDR_ACCUMULATOR:
            sprintf(opcode_operands, str_addr_modes[am]);
            break;
        default:
            sprintf(opcode_operands, "");
            break;
    }

    sprintf(opcode_mnemonic, "%s ", str_mnemonics[opc_table[bytes[0]].mnemonic]);
    strcat(opcode_full, opcode_mnemonic);
    strcat(opcode_full, opcode_operands);

    const char fmtstr[] = 
        "%.4X: %-10s %-20s "
        "A: %.2X X: %.2X Y: %.2X S: %.2X "
        "flags C: %d Z: %d V: %d N: %d I: %d\n";

    printf(
        fmtstr, 
        addr, opcode_bytes, opcode_full, 
        CPU.A, CPU.X, CPU.Y, CPU.S, 
        CPU.flags.C, CPU.flags.Z, CPU.flags.V, CPU.flags.N, CPU.flags.I
    );
    fflush(stdout);
}

void
print_stack()
{
    for (int i = 0x0100; i < 0x0200; i++)
    {
        printf("%.4X: %.2X", i, read8(i));
        if ((i & 0xFF) == CPU.S)
            printf("  <-- Stack Pointer");
        printf("\n");
    }
}

void
get_operands()
{
    uint16_t operand_addr = CPU.PC + 1;
    switch (CPU.addr_mode)
    {
    case ADDR_ABSOLUTE:
        CPU.e_addr = read16(operand_addr);
        CPU.data = read8(CPU.e_addr);
        break;
    case ADDR_ABSOLUTE_X:
        CPU.e_addr = read16(operand_addr);
        CPU.e_addr += CPU.X;
        CPU.data = read8(CPU.e_addr);
        break;
    case ADDR_ABSOLUTE_Y:
        CPU.e_addr = read16(operand_addr);
        CPU.e_addr += CPU.Y;
        CPU.data = read8(CPU.e_addr);
        break;
    case ADDR_ACCUMULATOR:
        CPU.data = CPU.A;
        break;
    case ADDR_IMPLIED:
        break;
    case ADDR_IMMEDIATE:
        CPU.data = read8(operand_addr);
        break;
    case ADDR_INDIRECT:
        CPU.addr = read16(operand_addr);
        CPU.e_addr = read16(CPU.addr);
        CPU.data = read8(CPU.e_addr);
        break;
    case ADDR_INDIRECT_X:
        CPU.e_addr = (read8(operand_addr) + CPU.X);
        CPU.e_addr &= 0x00FF;
        CPU.e_addr = read16(CPU.e_addr);
        CPU.data = read8(CPU.e_addr);
        break;
    case ADDR_INDIRECT_Y:
        CPU.e_addr = read8(operand_addr);
        CPU.e_addr = read16(CPU.e_addr);
        CPU.e_addr += CPU.Y;
        CPU.data = read8(CPU.e_addr);
        break;
    case ADDR_RELATIVE:
        CPU.data = read8(operand_addr);
        break;
    case ADDR_ZEROPAGE:
        CPU.e_addr = read8(operand_addr);
        CPU.data = read8(CPU.e_addr);
        break;
    case ADDR_ZEROPAGE_X:
        CPU.e_addr = read8(operand_addr);
        CPU.e_addr += CPU.X;
        CPU.e_addr &= 0x00FF;
        CPU.data = read8(CPU.e_addr);
        break;
    case ADDR_ZEROPAGE_Y:
        CPU.e_addr = read8(operand_addr);
        CPU.e_addr += CPU.Y;
        CPU.e_addr &= 0x00FF;
        CPU.data = read8(CPU.e_addr);
        break;            
    default:
        assert(0);
    }
}

struct cpu_flags 
flags_unpack(uint8_t flags)
{
    struct cpu_flags r;

    r.N = (flags >> 7) & 1;
    r.V = (flags >> 6) & 1;
    r.unused = 1;               // always 1
    // r.B = (flags >> 4) & 1;
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
    r |= 1 << 5;        // always 1
    r |= flags.B << 4;
    r |= flags.D << 3;
    r |= flags.I << 2;
    r |= flags.Z << 1;
    r |= flags.C;

    return r;
}

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
    /*
     * states:
     * 0: opcode not read, read opcode
     * 1: opcode byte read, execute opcode
     * 2: waste time until cycle quota full
     */

    // TODO: make cycle accurate

    switch(CPU.state)
    {
    case 0:
        // print_opc(CPU.PC);
        CPU.IR = read8(CPU.PC);
        CPU.addr_mode = opc_table[CPU.IR].addr_mode;
        CPU.rem_bytes = opc_table[CPU.IR].bytes;
        CPU.rem_cycles = opc_table[CPU.IR].clock - 1;
        get_operands();
        CPU.state = 1;
        break;
    case 1:
        opc_table[CPU.IR].instruction();
        CPU.rem_cycles--;
        CPU.state = (CPU.rem_cycles > 0) ? 2 : 0;
        break;
    case 2:
        if (CPU.rem_cycles > 0)
            CPU.rem_cycles--;
        else
            CPU.state = 0;
        break;
    default:
        assert(0);
        break;
    }
}

void
stack_push(uint8_t x)
{
    uint16_t stack_addr = 0x0100 | CPU.S;
    write8(stack_addr, x);
    CPU.S--;
}

uint8_t
stack_pull()
{
    CPU.S++;
    uint16_t stack_addr = 0x0100 | CPU.S;
    uint8_t r = read8(stack_addr);
    return r;
}


/* opcodes */

// set Z and N flags based on 8-bit signed value
void setnz(int8_t val)
{
    CPU.flags.Z = (val == 0);
    CPU.flags.N = (val < 0);
    return;
}

/* Arithmetic Operations */

uint8_t adcsbc(uint8_t a, uint8_t b, uint8_t c)
{
    uint16_t r = a + b + c;
    
    // vflag1 = 1 if both operands have same sign
    int vflag1 = !(((a ^ b) >> 7) & 1);
    // vflag2 = 1 if result and accumulator have different sign
    int vflag2 = (((a ^ r) >> 7) & 1);

    CPU.flags.C = (r >> 8) & 1;
    CPU.flags.V = (vflag1 && vflag2);

    return (r & 0xFF);
}

void ins_adc()
{
    CPU.A = adcsbc(CPU.A, CPU.data, CPU.flags.C);
    setnz(CPU.A);
    CPU.PC += CPU.rem_bytes;
}

void ins_sbc()
{
    CPU.A = adcsbc(CPU.A, ~CPU.data, CPU.flags.C);
    setnz(CPU.A);
    CPU.PC += CPU.rem_bytes;
}

/* Bitwise operations */

void ins_and() 
{
    CPU.A &= CPU.data;
    setnz(CPU.A);
    CPU.PC += CPU.rem_bytes;
}

void ins_ora()
{
    CPU.A |= CPU.data;
    setnz(CPU.A);
    CPU.PC += CPU.rem_bytes;
}

void ins_eor()
{
    CPU.A ^= CPU.data;
    setnz(CPU.A);
    CPU.PC += CPU.rem_bytes;
}

void ins_asl() 
{
    CPU.flags.C = (CPU.data >> 7) & 1;
    CPU.data <<= 1;

    if (CPU.addr_mode == ADDR_ACCUMULATOR)
        CPU.A = CPU.data;
    else
        write8(CPU.e_addr, CPU.data);

    setnz(CPU.data);
    CPU.PC += CPU.rem_bytes;
}

void ins_lsr()
{
    CPU.flags.C = CPU.data & 1;
    CPU.data >>= 1;

    if (CPU.addr_mode == ADDR_ACCUMULATOR)
        CPU.A = CPU.data;
    else
        write8(CPU.e_addr, CPU.data);

    setnz(CPU.data);
    CPU.PC += CPU.rem_bytes;
}

void ins_rol()
{
    uint16_t r = CPU.data;
    r <<= 1;
    r |= CPU.flags.C;
    CPU.flags.C = (r >> 8) & 1;
    CPU.data = r & 0xFF;

    if (CPU.addr_mode == ADDR_ACCUMULATOR)
        CPU.A = CPU.data;
    else
        write8(CPU.e_addr, CPU.data);

    setnz(CPU.data);
    CPU.PC += CPU.rem_bytes;
}

void ins_ror()
{
    uint16_t r = CPU.data;
    r |= CPU.flags.C << 8;
    CPU.flags.C = r & 1;
    r >>= 1;
    CPU.data = r & 0xFF;

    if (CPU.addr_mode == ADDR_ACCUMULATOR)
        CPU.A = CPU.data;
    else
        write8(CPU.e_addr, CPU.data);

    setnz(CPU.data);
    CPU.PC += CPU.rem_bytes;
}


/* Branching instructions */

void branch()
{
    int8_t offset = CPU.data;   // convert to signed
    CPU.PC += offset + 2;
}

void ins_bcc() 
{
    if (CPU.flags.C == 0)
        branch();
    else
        CPU.PC += CPU.rem_bytes;
}

void ins_bcs() 
{
    if (CPU.flags.C == 1)
        branch();
    else
        CPU.PC += CPU.rem_bytes;
}

void ins_beq() 
{
    if (CPU.flags.Z == 1)
        branch();
    else
        CPU.PC += CPU.rem_bytes;
}

void ins_bmi() 
{
    if (CPU.flags.N == 1)
        branch();
    else
        CPU.PC += CPU.rem_bytes;
}

void ins_bne() 
{
    if (CPU.flags.Z == 0)
        branch();
    else
        CPU.PC += CPU.rem_bytes;
}

void ins_bpl() 
{
    if (CPU.flags.N == 0)
        branch();
    else
        CPU.PC += CPU.rem_bytes;
}

void ins_bvc() 
{
    if (CPU.flags.V == 0)
        branch();
    else
        CPU.PC += CPU.rem_bytes;
}

void ins_bvs() 
{
    if (CPU.flags.V == 1)
        branch();
    else
        CPU.PC += CPU.rem_bytes;
}

/* Flag set/clear instructions */

void ins_clc() 
{
    CPU.flags.C = 0;
    CPU.PC += CPU.rem_bytes;
}

void ins_cld() 
{
    CPU.flags.D = 0;
    CPU.PC += CPU.rem_bytes;
}

void ins_cli() 
{
    CPU.flags.I = 0;
    CPU.PC += CPU.rem_bytes;
}

void ins_clv() 
{
    CPU.flags.V = 0;
    CPU.PC += CPU.rem_bytes;
}

void ins_sec()
{
    CPU.flags.C = 1;
    CPU.PC += CPU.rem_bytes;
}

void ins_sed()
{
    CPU.flags.D = 1;
    CPU.PC += CPU.rem_bytes;
}

void ins_sei()
{
    CPU.flags.I = 1;
    CPU.PC += CPU.rem_bytes;
}


/* Comparison operations */

void compare(uint8_t a, uint8_t b)
{
    uint8_t r = a - b;
    setnz(r);
    CPU.flags.C = (a >= b);
}

void ins_cmp()
{
    compare(CPU.A, CPU.data);
    CPU.PC += CPU.rem_bytes;
}

void ins_cpx()
{
    compare(CPU.X, CPU.data);
    CPU.PC += CPU.rem_bytes;
}

void ins_cpy()
{
    compare(CPU.Y, CPU.data);
    CPU.PC += CPU.rem_bytes;
}


/* Increment/Decrement operations */

void ins_dec() 
{
    CPU.data--;
    write8(CPU.e_addr, CPU.data);
    setnz(CPU.data);
    CPU.PC += CPU.rem_bytes;
}

void ins_dex() 
{
    CPU.X--;
    setnz(CPU.X);
    CPU.PC += CPU.rem_bytes;
}

void ins_dey()
{
    CPU.Y--;
    setnz(CPU.Y);
    CPU.PC += CPU.rem_bytes;
}

void ins_inc()
{
    CPU.data++;
    write8(CPU.e_addr, CPU.data);
    setnz(CPU.data);
    CPU.PC += CPU.rem_bytes;
}

void ins_inx()
{
    CPU.X++;
    setnz(CPU.X);
    CPU.PC += CPU.rem_bytes;
}

void ins_iny()
{
    CPU.Y++;
    setnz(CPU.Y);
    CPU.PC += CPU.rem_bytes;
}

/* Jumps and Calls */

void ins_jmp()
{
    uint16_t jump_addr = CPU.e_addr;

    // indirect jump page wraparound
    if (CPU.addr_mode == ADDR_INDIRECT)
    {
        jump_addr = read8(CPU.addr);

        if (CPU.addr & 0xFF == 0x00FF)
            CPU.addr &= 0xFF00;
        else
            CPU.addr++;

        jump_addr |= read8(CPU.addr) << 8;
    }

    CPU.PC = jump_addr;
}

void ins_jsr()
{
    uint16_t return_addr = CPU.PC + 2;      // one less than actual address
    stack_push((return_addr >> 8) & 0xFF);
    stack_push(return_addr & 0xFF);
    CPU.PC = CPU.e_addr;
}

void ins_rts()
{
    CPU.PC = stack_pull();
    CPU.PC |= stack_pull() << 8;
    CPU.PC++;
}


/* Load/Store operations */

void ins_lda() 
{
    CPU.A = CPU.data;
    setnz(CPU.A);
    CPU.PC += CPU.rem_bytes;
}

void ins_ldx()
{
    CPU.X = CPU.data;
    setnz(CPU.X);
    CPU.PC += CPU.rem_bytes;
}

void ins_ldy()
{
    CPU.Y = CPU.data;
    setnz(CPU.Y);
    CPU.PC += CPU.rem_bytes;
}

void ins_sta()
{
    write8(CPU.e_addr, CPU.A);
    CPU.PC += CPU.rem_bytes;
}

void ins_stx()
{
    write8(CPU.e_addr, CPU.X);
    CPU.PC += CPU.rem_bytes;
}

void ins_sty()
{
    write8(CPU.e_addr, CPU.Y);
    CPU.PC += CPU.rem_bytes;
}


/* Stack instructions */

void ins_pha()
{
    stack_push(CPU.A);
    CPU.PC += CPU.rem_bytes;
}

void ins_php()
{
    stack_push(flags_pack(CPU.flags));
    CPU.PC += CPU.rem_bytes;
}

void ins_pla()
{
    CPU.A = stack_pull();
    setnz(CPU.A);
    CPU.PC += CPU.rem_bytes;
}

void ins_plp()
{
    CPU.flags = flags_unpack(stack_pull());
    CPU.PC += CPU.rem_bytes;
}

/* Register operations */

void ins_tax()
{
    CPU.X = CPU.A;
    setnz(CPU.X);
    CPU.PC += CPU.rem_bytes;
}

void ins_tay() 
{
    CPU.Y = CPU.A;
    setnz(CPU.Y);
    CPU.PC += CPU.rem_bytes;
}

void ins_tsx()
{
    CPU.X = CPU.S;
    setnz(CPU.X);
    CPU.PC += CPU.rem_bytes;
}

void ins_txa()
{
    CPU.A = CPU.X;
    setnz(CPU.A);
    CPU.PC += CPU.rem_bytes;
}

void ins_txs()
{
    CPU.S = CPU.X;
    CPU.PC += CPU.rem_bytes;
}

void ins_tya()
{
    CPU.A = CPU.Y;
    setnz(CPU.A);
    CPU.PC += CPU.rem_bytes;
}

/* Other Instructions */

void ins_nop()
{
    CPU.PC += CPU.rem_bytes;
}

void ins_bit() 
{
    CPU.flags.N = (CPU.data >> 7) & 1;
    CPU.flags.V = (CPU.data >> 6) & 1;
    CPU.flags.Z = ((CPU.data & CPU.A) == 0);
    CPU.PC += CPU.rem_bytes;
}

void ins_brk()
{
    if (CPU.flags.I)
        return;
    CPU.flags.B = 1;
    uint16_t return_addr = CPU.PC + 2;
    stack_push((return_addr >> 8) & 0xFF);
    stack_push(return_addr & 0xFF);
    CPU.PC = read8(0xFFFE) | (read8(0xFFFF) << 8);
    stack_push(flags_pack(CPU.flags));
    CPU.flags.B = 0;
}

void ins_rti()
{
    CPU.flags = flags_unpack(stack_pull());
    CPU.PC = stack_pull();
    CPU.PC |= stack_pull() << 8;
}

/* Non-maskable Interrupt */
void cpu_nmi()
{
}
