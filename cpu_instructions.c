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
        cpu_set_flag(cpu, 'C');
    }
    else {
        cpu_clear_flag(cpu, 'C');
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
        cpu_set_flag(cpu, 'V');
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
    cpu_set_flag(cpu, 'C');
    int r = (cpu->A + (cpu->P & 1) + cpu->op_eval);
    // 9th bit of r is the carry bit
    if (r & ~(1 << 8)) {
        cpu_set_flag(cpu, 'C');
    }
    else {
        cpu_clear_flag(cpu, 'C');
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
        cpu_set_flag(cpu, 'C');
    }
    else {
        cpu_clear_flag(cpu, 'C');
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
        cpu_set_flag(cpu, 'V');
    }
    cpu->A = 0xFF & r;
}

void ins_bit() {
    // Test bits in mem with accumulator
    // this is a weird instruction where the value in A is ANDed with memory
    // if the result (which is not stored anywhere) is zero then Zero flag is set
    // and bit 7 of memory sets N flag while bit 6 sets overflow flag
    if (cpu->A & cpu->op_eval) {
        cpu_set_flag(cpu, 'Z');
    }
    else {
        cpu_clear_flag(cpu, 'Z');
    }
    if ((cpu->op_eval >> 6) & 0x01) {
        // bit 6 is set, meaning overflow will be set
        cpu_set_flag(cpu, 'V');
    }
    else {
        cpu_clear_flag(cpu, 'V');
    }
    if ((cpu->op_eval >> 7) & 0x01) {
        // bit 7 is set, meaning negative will be set
        cpu_set_flag(cpu, 'N');
    }
    else {
        cpu_clear_flag(cpu, 'N');
    }
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
    cpu_set_flag(cpu, 'C');
    int r = (cpu->Y + (cpu->P & 1) + cpu->op_eval);
    // 9th bit of r is the carry bit
    if (r & ~(1 << 8)) {
        cpu_set_flag(cpu, 'C');
    }
    else {
        cpu_clear_flag(cpu, 'C');
    }
}
void ins_cpx() {
    // Compare with X
    // WARNING: THIS IS COMPLETELY UNTESTED AND BASED PURELY OFF OF THEORY
    cpu_set_flag(cpu, 'C');
    int r = (cpu->X + (cpu->P & 1) + cpu->op_eval);
    // 9th bit of r is the carry bit
    if (r & ~(1 << 8)) {
        cpu_set_flag(cpu, 'C');
    }
    else {
        cpu_clear_flag(cpu, 'C');
    }
}

void ins_asl() {
    // shift left by one bit; old bit 7 is stored in carry
    if (cpu->op_eval & 0x80) {
        cpu_set_flag(cpu, 'C');
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
        cpu_set_flag(cpu, 'C');
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

void ins_dex() {
    cpu->X -= 1;
}
void ins_dey() {
    cpu->Y -= 1;
}
void ins_inx() {
    cpu->X += 1;
}
void ins_iny() {
    cpu->Y += 1;
}



void ins_branch() {
    /*
     * All branching opcodes follow the same format:
     * xxy10000
     * where xx is the flag compared with y
     * if they are equal, branch is taken.
     * all branches follow relative addressing where the signed operand value
     * is added to the PC
     * addition happen after PC has already been incremented to point to the next instruction
     * see _getaddrmd and _parse_op
     * 
     * xx:
     * 00 - n
     * 01 - v
     * 10 - c
     * 11 - z
     * 
     */
    char x = (cpu->cur_op >> 5);
    char a = x & 0x01;
    x = x >> 1;
    char b;
    switch(x) {
        case 0:
            b = cpu->P >> 7;
            break;
        case 1:
            b = cpu->P >> 6;
            break;
        case 2:
            b = cpu->P;
            break;
        case 3:
            b = cpu->P >> 1;
            break;
        default:
            break;
    }
    b &= 0x01;
    if (a == b) {
        cpu->PC = cpu->PC + cpu->op_eval;
    }
}

// flags
void ins_clc() {
    // Clear Carry
    cpu_clear_flag(cpu, 'C');
}
void ins_sec() {
    // Set Carry
    cpu_set_flag(cpu, 'C');
}
void ins_cli() {
    // Clear Interrupt
    cpu_clear_flag(cpu, 'I');
}
void ins_sei() {
    // Set Interrupt
    cpu_set_flag(cpu, 'I');
}
void ins_clv() {
    // Clear Overflow
    cpu_clear_flag(cpu, 'V');
}
// decimal flag is unused in the NES, check if there are inconsistencies 
// in register values
void ins_cld() {
    // Clear Decimal
    cpu_clear_flag(cpu, 'D');
}
void ins_sed() {
    // Set Decimal
    cpu_set_flag(cpu, 'D');
}

// transfer value instructions
void ins_tax() {
    // A to X
    cpu->X = cpu->A;
}
void ins_txa() {
    // X to A
    cpu->A = cpu->X;
}
void ins_tay() {
    // A to Y
    cpu->Y = cpu->A;
}
void ins_tya() {
    // Y to A
    cpu->A = cpu->Y;
}
void ins_tsx() {
    // S to X
    cpu->X = cpu->S;
}
void ins_txs() {
    // X to S
    cpu->S = cpu->X;
}

// stack
void ins_pha() {
    // Push Accumulator to stack
    cpu_stack_pushint8(cpu, cpu->A);
}
void ins_pla() {
    // Pull Accumulator from stack
    cpu->A = cpu_stack_popint8(cpu);
}
void ins_php() {
    // Push Status to stack
    cpu_stack_pushint8(cpu, cpu->P);
}
void ins_plp() {
    // Pull Status from stack
    cpu->P = cpu_stack_popint8(cpu);
}

// others
void ins_jsr() {
    // Jump to subroutine
    cpu_stack_pushint16(cpu, cpu->PC - 1);
}
void ins_rts() {
    // Return from subroutine
    cpu->PC = cpu_stack_popint16(cpu) + 1;
}
void ins_nop() {
    // No operation
}
void ins_rti() {
    // Return from Interrupt
    // rti pulls status and program counter (in that order) from stack
    cpu->P = cpu_stack_popint8(cpu);
    cpu->PC = cpu_stack_popint16(cpu);
}

//interrupt functions
void ins_brk() {
    //-------
}