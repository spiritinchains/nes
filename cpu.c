#include "cpu.h"
#include <stdio.h>

void _set_addrmd(CPU* cpu, int mode)
{
    cpu->state &= ~(0xF << 4);
    cpu->state |= (mode << 4);
}

char _get_addrmd(CPU* cpu)
{
    return ((cpu->state >> 4) & 0xF);
}

char _get_flag_bit(char flag)
{
    /*
     * Flag register:
     * N V - B D I Z C
     * 7 6 5 4 3 2 1 0
     */
    char bit = 8;
    switch (flag)
    {
        case 'C':
            bit = 0;
            break;
        case 'Z':
            bit = 1;
            break;
        case 'I':
            bit = 2;
            break;
        case 'D':
            bit = 3;
            break;
        case 'B':
            bit = 4;
            break;
        case 'V':
            bit = 6;
            break;
        case 'N':
            bit = 7;
            break;
        default:
            break;
    }
    return bit;
}

void cpu_set_flag(CPU* cpu, char flag)
{
    unsigned int bit = _get_flag_bit(flag);
    if (bit < 8)
    {
        cpu->P |= (1 << bit);
        return;
    }
}

void cpu_clr_flag(CPU* cpu, char flag)
{
    unsigned int bit = _get_flag_bit(flag);
    if (bit < 8)
    {
        cpu->P &= ~(1 << bit);
    }

}

void cpu_init(CPU* cpu, MMAP* mm)
{
    cpu->A = 0;
    cpu->X = 0;
    cpu->Y = 0;
    cpu->P = 0;
    cpu->S = 0;
    cpu->PC = 0;
    cpu->SP = 0xFF;
    cpu->mmap = mm;
    cpu->state = 1;
}

void cpu_cycle(CPU* cpu)
{
    char ins = *(char*)mmap_getptr(cpu->mmap, cpu->PC);
    if (ins == 0x00)
    {
        //BRK
        cpu->state = 0;
    }
    else if (ins == 0x20)
    {
        //JSR abs
        //push PC + 2
        //jump arg
    }
    else if (ins == 0x40)
    {
        //RTI
    }
    else if (ins == 0x60)
    {
        //RTS
    }
    else
    {
        // all other opcodes follow the form aaabbbcc
        // branching opcodes follow the form xxy10000
        // meaning c = 0, b = 4 and aaa = xxy

        char a = (ins & (7 << 5)) >> 5;
        char b = (ins & (7 << 2)) >> 2;
        char c = ins & 3;

        //char opc_id = ins & (!28);

        // get addressing mode
        if (b == 0)
        {
            // 0 is not a valid addressing id as per our templates, so these
            // are remapped; Indirect X (9) if c == 1 and Immediate (8) if
            // c == 0. Or, c + 8
            _set_addrmd(cpu, c + 8);
        }
        else if (b == 2 && c == 1)
        {
            // in this case, the mode is actually Immediate (8) instead of
            // Accumulator/Implied (2)
            _set_addrmd(cpu, 8);
        }
        else if (b == 6 && c != 1)
        {
            // in this case, the mode is always Implied (2)
            _set_addrmd(cpu, 2);
        }
        else
        {
            // for all other values the mode is indicated by b
            _set_addrmd(cpu, b);
        }
    }
    cpu->PC++;
}
