#include "mmap.h"

typedef struct _cpu {
    unsigned char A;
    unsigned char X;
    unsigned char Y;
    unsigned char S;
    unsigned short PC;
    unsigned char P;        // N V - B D I Z C
    unsigned char SP;
    MMAP* mmap;
    /*!
     * state:
     * bit 16-23 - opcode
     * bit 24-27 - addressing mode
     *      1 - zero page
     *      2 - Accumulator / Implied
     *      3 - Absolute
     *      4 - Indirect Y
     *      5 - zero page x/y
     *      6 - absolute Y
     *      7 - absolute X
     *      8 - Immediate
     *      9 - Indirect X
     *      (Indirect and relative are covered in special cases)
     * bit 28-31 - branching (?)
     * bit 32 - always 1
     *
     */
    int state;
} CPU;

void cpu_init(CPU* cpu, MMAP* mem);
void cpu_cycle(CPU* cpu);

void set_flag(CPU* cpu, char bit);
void clr_flag(CPU* cpu, char bit);

char* get_arg_ptr(CPU* cpu);

void _opc_adc(CPU* cpu);