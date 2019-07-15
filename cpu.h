#include "mmap.h"

typedef struct _cpu {
    unsigned int A;
    unsigned int X;
    unsigned int Y;
    unsigned int S;
    unsigned int PC;
    unsigned int P;         // flags: N V - B D I Z C
    unsigned int SP;        // stack pointer

    unsigned int op_value;  // operand value
    unsigned int e_addr;    // effective address
    unsigned int addrmd;    // addressing mode
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

void cpu_set_flag(CPU* cpu, char flag);
void cpu_clr_flag(CPU* cpu, char flag);
