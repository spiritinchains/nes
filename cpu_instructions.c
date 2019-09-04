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
 * - refactor code (VERY VERY IMPORTANT - SLAP AUTHOR IN THE FACE IF NOT DONE YET)
 * - link to cpu structure
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
    /*
     * This instruction adds the evaluated operand to the accumulator, and the
     * carry bit to it. If the resulting addition produces a carry as well,
     * then the carry bit is set again, otherwise reset.
     * The overflow bit is set when the result of addition is outside the 2's
     * complement range in signed operations; i.e. considering if all opeerations were
     * signed
     */
    int r = (cpu->A + (cpu->P & 1) + cpu->op_eval);
    // 9th bit of r is the carry bit
    if (r & ~(1 << 8)) {
        cpu_set_flag(cpu, "C");
    }
    else {
        cpu_clear_flag(cpu, "C");
    }
    // overflow bit is set if
    // - sign bit of both initial operands are same to each other
    // - and different from result (after adding carry)
    // NOTE: this may cause problems in corner cases! check when in doubt
    int bit = ((~(cpu->A ^ cpu->op_eval) >> 7) & 1);    // bit 7 of A and op_eval are same or not
    bit &= ((r >> 7) & 1);  // whether sign bit of result is the same as sign bit of operands
    if (bit) {
        // no overflow
    }
    else {
        cpu_set_flag(cpu, "V");
    }
    cpu->A = 0xFF & r;
}

void ins_sta() {
    // Store value of accumulator in memory
    mmap_setint8(cpu->mmap, cpu->e_addr, cpu->A);
} 

void ins_lda() {
    // Load in accumulator
    cpu->A = cpu->op_eval;
}

void ins_cmp() {
    // Compare memory with accumulator
    // WARNING: THIS IS COMPLETELY UNTESTED AND BASED PURELY OFF OF THEORY
    cpu_set_flag(cpu, "C");
    int r = (cpu->A + (cpu->P & 1) + cpu->op_eval);
    // 9th bit of r is the carry bit
    if (r & ~(1 << 8)) {
        cpu_set_flag(cpu, "C");
    }
    else {
        cpu_clear_flag(cpu, "C");
    }

}

void ins_sbc() {
    // subtraction
    /*
     * NOTE:
     * This is almost EXACTLY the same as ADC, with the only difference being
     * the evaluated operand is complemented. If behaviour is not fully
     * identical to the original then refer to this function and check
     * whether the flags are set properly or something idk
     */
    int r = (cpu->A + (cpu->P & 1) + ~(cpu->op_eval));
    // 9th bit of r is the carry bit
    if (r & ~(1 << 8)) {
        cpu_set_flag(cpu, "C");
    }
    else {
        cpu_clear_flag(cpu, "C");
    }
    // overflow bit is set if
    // - sign bit of both initial operands are same to each other
    // - and different from result (after adding carry)
    // NOTE: this may cause problems in corner cases! check when in doubt
    int bit = ((~(cpu->A ^ cpu->op_eval) >> 7) & 1);    // bit 7 of A and op_eval are same or not
    bit &= ((r >> 7) & 1);  // whether sign bit of result is the same as sign bit of operands
    if (bit) {
        // no overflow
    }
    else {
        cpu_set_flag(cpu, "V");
    }
    cpu->A = 0xFF & r;
}

void ins_bit() {
    // Test bits in mem with accumulator
}

void ins_jmp() {
    // Regular Jump (indirect)
    // NOTE: Indirect addressing has not been implemented yet
    // This is only theoretical as JMP is the only instruction to use it
    char lbs = cpu->e_addr & 0xFF;
    char hbs = (cpu->e_addr >> 8) & 0xFF;
    char lbd = mmap_getint8(cpu->mmap, (hbs << 8) + lbs);
    lbs++;
    char hbd = mmap_getint8(cpu->mmap, (hbs << 8) + lbs);
    cpu->PC = (hbs << 8) + lbs;
}
void ins_jmpabs() {
    // Absolute address jump
    cpu->PC = cpu->e_addr;
}
void ins_sty() {
    // Store Y in memory
    mmap_setint8(cpu->mmap, cpu->e_addr, cpu->X);
}
void ins_ldy() {
    // Load in Y
    cpu->Y = cpu->op_eval;
}
void ins_cpy() {
    // Compare with Y
    // WARNING: THIS IS COMPLETELY UNTESTED AND BASED PURELY OFF OF THEORY
    cpu_set_flag(cpu, "C");
    int r = (cpu->Y + (cpu->P & 1) + cpu->op_eval);
    // 9th bit of r is the carry bit
    if (r & ~(1 << 8)) {
        cpu_set_flag(cpu, "C");
    }
    else {
        cpu_clear_flag(cpu, "C");
    }
}
void ins_cpx() {
    // Compare with X
    // WARNING: THIS IS COMPLETELY UNTESTED AND BASED PURELY OFF OF THEORY
    cpu_set_flag(cpu, "C");
    int r = (cpu->X + (cpu->P & 1) + cpu->op_eval);
    // 9th bit of r is the carry bit
    if (r & ~(1 << 8)) {
        cpu_set_flag(cpu, "C");
    }
    else {
        cpu_clear_flag(cpu, "C");
    }
}

void ins_asl() {
    // shift left by one bit; old bit 7 is stored in carry
    if (cpu->op_eval & 0x80) {
        cpu_set_flag(cpu, "C");
    }
    if (cpu->addrmd == 2) {
        cpu->A = cpu->A << 1;
    }
    else {
        mmap_setint8(cpu->mmap, cpu->e_addr, cpu->op_eval << 1);
    }
}
void ins_rol() {
    // same as asl, except old carry value is new bit 0
    char carry = cpu->P & 0x01;
    ins_asl();
    if (cpu->addrmd == 2) {
        cpu->A += carry;
    }
    else {
        mmap_setint8(cpu->mmap, cpu->e_addr, cpu->op_eval + carry);
    }
}
void ins_lsr() {
    // shift right one bit, old bit 0 is stored in carry
    if (cpu->op_eval & 0x01) {
        cpu_set_flag(cpu, "C");
    }
    if (cpu->addrmd == 2) {
        cpu->A = cpu->A >> 1;
    }
    else {
        mmap_setint8(cpu->mmap, cpu->e_addr, cpu->op_eval >> 1);
    }
}
void ins_ror() {
    // same as lsr, except old carry value is new bit 7
    char carry = cpu->P & 0x01;
    ins_lsr();
    if (cpu->addrmd == 2) {
        cpu->A += (carry << 7);
    }
    else {
        mmap_setint8(cpu->mmap, cpu->e_addr, cpu->op_eval + (carry << 7));
    }
}
void ins_stx() {
    // store x in memory
    mmap_setint8(cpu->mmap, cpu->e_addr, cpu->X);
}
void ins_ldx() {
    // load value into x
    cpu->X = cpu->op_eval;
}
void ins_dec() {
    mmap_setint8(cpu->mmap, cpu->e_addr, cpu->op_eval - 1);
}
void ins_inc() {
    mmap_setint8(cpu->mmap, cpu->e_addr, cpu->op_eval + 1);
}

