#include "cpu.h"

/*
c = 01
000	ORA
001	AND
010	EOR
011	ADC
100	STA 
101	LDA
110	CMP
111	SBC

c = 00
001	BIT
010	JMP 
011	JMP (abs)
100	STY
101	LDY
110	CPY
111	CPX

c = 10
000	ASL
001	ROL
010	LSR
011	ROR
100	STX
101	LDX
110	DEC
111	INC
*/

CPU* cpu;

void ins_nop() {
    // No operation
}

/*
 * TODO:
 * - fix all instructions
 * - implement flags
 * - implement non-standard instructions (branching, flag clr)
 */

void ins_ora() {
    // OR with accumulator
    cpu->A = cpu->A | cpu->op_eval;
}

void ins_and() {
    // AND with accumulator
    cpu->A = cpu->A & cpu->op_eval;
}

void ins_eor() {
    // XOR with accumulator
    cpu->A = cpu->A ^ cpu->op_eval;
}

void ins_adc() {
    // Add with Carry
}

void ins_sta() {
    // Store value of accumulator in memory
    
} 

void ins_lda() {
    // Load in accumulator
}

void ins_cmp() {
    // Compare memory with accumulator
}

void ins_sbc() {}

void ins_bit() {
    // Test bits in mem with accumulator
}

void ins_jmp() {}
void ins_jmpabs() {}
void ins_sty() {}
void ins_ldy() {}
void ins_cpy() {}
void ins_cpx() {}

void ins_asl() {}
void ins_rol() {}
void ins_lsr() {}
void ins_ror() {}
void ins_stx() {}
void ins_ldx() {}
void ins_dec() {}
void ins_inc() {}

