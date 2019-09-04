#include "mmap.h"

typedef struct _cpu {
    char A;
    char X;
    char Y;
    char S;
    short PC;
    char P;         // flags: N V - B D I Z C
    char SP;        // stack pointer

    char op_val;    // raw operand value - the value actually passed to the code
    char op_eval;   // evaluated operand value - the value that the instruction uses
    short e_addr;    // effective address
    short e_addry;   // effective address, y indexed (special use case)
    char addrmd;    // addressing mode
    char cur_op;    // current opcode
    
    /*
     * 0001 1 - zero page
     * 0010 2 - Accumulator / Implied
     * 0011 3 - Absolute
     * 0100 4 - Indirect Y
     * 0101 5 - zero page x/y
     * 0110 6 - absolute Y
     * 0111 7 - absolute X
     * 1000 8 - Immediate
     * 1001 9 - Indirect X
     */
    unsigned int state;     // state - boolean

    MMAP* mmap;
} CPU;

void cpu_init(CPU* cpu, MMAP* mem);
void cpu_cycle(CPU* cpu);

char cpu_get_flag(CPU* cpu, char flag);
void cpu_set_flag(CPU* cpu, char flag);
void cpu_clear_flag(CPU* cpu, char flag);

void cpu_stack_pushint8(CPU* cpu, unsigned int);
void cpu_stack_pushint16(CPU* cpu, unsigned int);

char cpu_stack_popint8(CPU* cpu);
short cpu_stack_popint16(CPU* cpu);
