#include "cpu.h"
#include <stdio.h>

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

void cpu_stack_pushint8(CPU* cpu, unsigned int val)
{
    cpu->SP--;
}
void cpu_stack_pushint16(CPU* cpu, unsigned int val)
{
    cpu->SP--;
}

unsigned int cpu_stack_popint8(CPU* cpu)
{
    mmap_getint8(cpu->mmap, 0x100 + (++cpu->SP));
}
unsigned int cpu_stack_popint16(CPU* cpu)
{
    mmap_getint16(cpu->mmap, 0x100 + (++cpu->SP));
    cpu->SP++;
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

int _get_addrmd(char ins)
{
    // most opcodes follow the form aaabbbcc
    // notable exceptions to the rule:
    // 0x00  000 000 00 (BRK)
    // 0x20  001 000 00 (JSR) abs
    // 0x40  010 000 00 (RTI)
    // 0x60  011 000 00 (RTS)

    // side note:
    // branching opcodes follow the form xxy10000
    // meaning c = 0, b = 4 and aaa = xxy

    char a = (ins & (7 << 5)) >> 5;
    char b = (ins & (7 << 2)) >> 2;
    char c = ins & 3;

    //printf("%x %x %x ", a, b, c);

    int am;     // addressing mode is stored here

    if (b == 0)
    {
        // 0 is not a valid addressing id as per our templates, so these
        // are remapped; Indirect X (9) if c == 1 and Immediate (8) if
        // c == 0 or 2.
        if (c == 1)
            am = 9;
        else if (c == 2 || c == 0)
            am = 8;
    }
    else if (b == 2 && c == 1)
    {
        // in this case, the mode is actually Immediate (8) instead of
        // Accumulator/Implied (2)
        am = 8;
    }
    else if (b == 6 && c != 1)
    {
        // in this case, the mode is always Implied (2)
        am = 2;
    }
    else
    {
        // for all other values the mode is indicated by b
        am = b;
    }
    return am;
}

void _exec_opc(CPU* cpu)
{
    /*
     * Executes opcode based on opcode type in cur_op
     */


}

void _parse_op(CPU* cpu)
{
    /*
     * get effective address and operand from current cpu state
     * and advance PC to the next instruction
     * should always be executed when PC is at operand address
     * i.e right after the instruction address
     *
     * eff. address  = the address from which opcode will be retrieved
     *                 or to which value will be written
     * operand value = the raw value passed as operand
     * eval. operand = the value retrieved from address
     *
     * TODO:
     * - manage exception cases (JMP)
     * - manage zero page x and y cases ----- DONE
     * - decide on op_eval's position (related to carry flag operations)
     */

    switch (cpu->addrmd)
    {
        case 1:
            // zero page
            //
            cpu->op_val = mmap_getint8(cpu->mmap, cpu->PC);
            cpu->e_addr = cpu->op_val;
            //cpu->op_eval = mmap_getint8(cpu->mmap, cpu->e_addr);
            cpu->PC++;
            break;
        case 2:
            // implied / accumulator
            //
            cpu->PC++;
            break;
        case 3:
            // absolute
            //
            cpu->op_val = mmap_getint16(cpu->mmap, cpu->PC);
            cpu->e_addr = cpu->op_val;
            //cpu->op_eval = mmap_getint8(cpu->mmap, cpu->op_val);
            cpu->PC += 2;
            break;
        case 4:
            // Indirect Y
            //
            cpu->op_val = mmap_getint8(cpu->mmap, cpu->PC);
            cpu->e_addr = mmap_getint16(cpu->mmap, cpu->op_val) + cpu->Y;
            //cpu->op_eval = mmap_getint8(cpu->mmap, cpu->e_addr);
            cpu->PC++;
            break;
        case 5:
            // zero page x/y
            //
            cpu->op_val = mmap_getint8(cpu->mmap, cpu->PC);
            cpu->e_addr = (0xFF & (cpu->op_val + cpu->X));
            cpu->e_addry = (0xFF & (cpu->op_val + cpu->Y));
            cpu->PC++;
            break;
        case 6:
            // absolute Y
            //
            cpu->op_val = mmap_getint16(cpu->mmap, cpu->PC);
            cpu->e_addr = cpu->op_val + cpu->Y;
            cpu->PC += 2;
            break;
        case 7:
            // absolute x
            //
            cpu->op_val = mmap_getint16(cpu->mmap, cpu->PC);
            cpu->e_addr = cpu->op_val + cpu->X;
            cpu->PC += 2;
            break;
        case 8:
            // immediate
            //
            cpu->op_val = mmap_getint8(cpu->mmap, cpu->PC);
            cpu->op_eval = cpu->op_val;
            cpu->PC++;
            break;
        case 9:
            // indirect x
            //
            cpu->op_val = mmap_getint8(cpu->mmap, cpu->PC);
            cpu->e_addr = mmap_getint16(cpu->mmap, 0xFF & (cpu->op_val + cpu->X));
            cpu->PC++;
            break;
        default:
            // invalid addressing mode
            cpu->PC++;
            break;
    }
}

// add function to execute opcode from simplified id

void cpu_cycle(CPU* cpu)
{
    /*
     * NOTE: need to implement a new way to handle exception cases
     * (0x00, 0x20, 0x40, 0x60)
     *
     * code will follow the following structure
     * ---------------------
     * - Get addressing mode
     * - Get address and operand through mode
     * - execute opcode from simplified instruction
     *
     * TODO:
     * - seperate code for addressing mode
     * - not count exception cases as if..else but rather handle them
     *   differently when executing (see step 3)
     */

    char ins = *(char*)mmap_getptr(cpu->mmap, cpu->PC);
    // get addressing mode and opcode
    cpu->addrmd = _get_addrmd(ins);
    cpu->cur_op = 0xE3 & ins;
    cpu->state = mmap_getint8(cpu->mmap, cpu->PC);
    // get operand value and effective address
    cpu->PC++;
    _parse_op(cpu);
    // execute opcode
    _exec_opc(cpu);
}
