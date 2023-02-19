
#ifndef CPU_H
#define CPU_H

#include <inttypes.h>

enum addr_modes 
{
	ADDR_IMMEDIATE = 1,
	ADDR_ABSOLUTE,
	ADDR_ZEROPAGE,
	ADDR_ACCUMULATOR,
	ADDR_ZEROPAGE_X,
	ADDR_ZEROPAGE_Y,
	ADDR_ABSOLUTE_X,
	ADDR_ABSOLUTE_Y,
	ADDR_INDIRECT_X,
	ADDR_INDIRECT_Y,
	ADDR_IMPLIED,
	ADDR_RELATIVE,
	ADDR_INDIRECT
};

enum opc_mnemonics
{
	OPC_UNK = 0,
	OPC_ADC,
	OPC_AND,
	OPC_ASL,
	OPC_BCC,
	OPC_BCS,
	OPC_BEQ,
	OPC_BIT,
	OPC_BMI,
	OPC_BNE,
	OPC_BPL,
	OPC_BRK,
	OPC_BVC,
	OPC_BVS,
	OPC_CLC,
	OPC_CLD,
	OPC_CLI,
	OPC_CLV,
	OPC_CMP,
	OPC_CPX,
	OPC_CPY,
	OPC_DEC,
	OPC_DEX,
	OPC_DEY,
	OPC_EOR,
	OPC_INC,
	OPC_INX,
	OPC_INY,
	OPC_JMP,
	OPC_JSR,
	OPC_LDA,
	OPC_LDX,
	OPC_LDY,
	OPC_LSR,
	OPC_NOP,
	OPC_ORA,
	OPC_PHA,
	OPC_PHP,
	OPC_PLA,
	OPC_PLP,
	OPC_ROL,
	OPC_ROR,
	OPC_RTI,
	OPC_RTS,
	OPC_SBC,
	OPC_SEC,
	OPC_SED,
	OPC_SEI,
	OPC_STA,
	OPC_STX,
	OPC_STY,
	OPC_TAX,
	OPC_TAY,
	OPC_TSX,
	OPC_TXA,
	OPC_TXS,
	OPC_TYA
};

struct cpu_flags
{
	unsigned C : 1;
	unsigned Z : 1;
	unsigned I : 1;
	unsigned D : 1;
	unsigned B : 1;
	unsigned unused : 1;
	unsigned V : 1;
	unsigned N : 1;
};

struct cpu
{
	/* registers */

	uint8_t A;
	uint8_t X;
	uint8_t Y;
	uint8_t S;						// Stack Pointer

	struct cpu_flags flags;

	uint16_t PC;					// Program Counter

	/* internal */

	uint8_t IR;						// Current Opcode
	enum addr_modes addr_mode;		// Addressing Mode
	int state;
	int rem_cycles;
	int rem_bytes;

	uint16_t e_addr;
	uint16_t addr;
	uint8_t data;

} extern CPU;

struct opc_record
{
	enum opc_mnemonics mnemonic;
	enum addr_modes addr_mode;
	int bytes;
	int clock;
	int page_transition;			// takes an extra cycle on page transition
	int branch;
	void (* instruction)(void);
};

void cpu_init();
void cpu_cycle();


/* Opcodes */


void ins_adc();
void ins_and();
void ins_asl();
void ins_bcc();
void ins_bcs();
void ins_beq();
void ins_bit();
void ins_bmi();
void ins_bne();
void ins_bpl();
void ins_brk();
void ins_bvc();
void ins_bvs();
void ins_clc();
void ins_cld();
void ins_cli();
void ins_clv();
void ins_cmp();
void ins_cpx();
void ins_cpy();
void ins_dec();
void ins_dex();
void ins_dey();
void ins_eor();
void ins_inc();
void ins_inx();
void ins_iny();
void ins_jmp();
void ins_jsr();
void ins_lda();
void ins_ldx();
void ins_ldy();
void ins_lsr();
void ins_nop();
void ins_ora();
void ins_pha();
void ins_php();
void ins_pla();
void ins_plp();
void ins_rol();
void ins_ror();
void ins_rti();
void ins_rts();
void ins_sbc();
void ins_sec();
void ins_sed();
void ins_sei();
void ins_sta();
void ins_stx();
void ins_sty();
void ins_tax();
void ins_tay();
void ins_tsx();
void ins_txa();
void ins_txs();
void ins_tya();


#endif
