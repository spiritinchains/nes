#include "cpu.h"
#include <stdio.h>

// private functions
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

// flag functions
char cpu_get_flag(CPU* cpu, char flag)
{
    unsigned int bit = _get_flag_bit(flag);
    return (cpu->P >> bit) & 0x01;
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
void cpu_clear_flag(CPU* cpu, char flag)
{
    unsigned int bit = _get_flag_bit(flag);
    if (bit < 8)
    {
        cpu->P &= ~(1 << bit);
    }

}

// stack functions
void cpu_stack_pushint8(CPU* cpu, unsigned int val)
{
    cpu->SP--;
}
void cpu_stack_pushint16(CPU* cpu, unsigned int val)
{
    cpu->SP--;
}

char cpu_stack_popint8(CPU* cpu)
{
    return mmap_getint8(cpu->mmap, 0x100 + (++cpu->SP));
}
short cpu_stack_popint16(CPU* cpu)
{
    short x = mmap_getint16(cpu->mmap, 0x100 + (++cpu->SP));
    cpu->SP++;
    return x;
}

// others
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

// private functions 2: electric boogaloo
int _get_addrmd(char ins)
{
    /*
     * returns the addressing mode
     * ins = instruction byte
     */

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
    else if (b == 4 && c == 0)
    {
        // this is relative addressing, which is only used in branches
        // so we can harmlessly (for now) treat it as immediate
        am = 8;
    }
    else if (c == 0 && (b == 2 || b == 6))
    {
        // These are guaranteed single-byte operations, therefore addressing
        // mode is implied
        am = 2;
    }
    else if
    (
        ins == 0x8A ||
        ins == 0x9A ||
        ins == 0xAA ||
        ins == 0xBA ||
        ins == 0xCA ||
        ins == 0xEA ||
        ins == 0x00 ||
        ins == 0x20 ||
        ins == 0x40 ||
        ins == 0x60
    )
    {
        // these are single byte instructions that don't clearly fall into
        // the above categories. all implied.
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
     * e_addr  = effective address; the address from which opcode will be 
     *           retrieved or in which value will be modified
     * op_val  = operand value; the raw value passed as operand
     * op_eval = evaluated operand; the value retrieved from address this 
     *           is the value that's used by the opcodes
     *
     * TODO:
     * - manage non-standard cases (JMP, branches, etc)
     * - manage zero page x and y cases ----- DONE
     * - decide on op_eval's position (related to carry flag operations)
     */

    switch (cpu->addrmd)
    {
        /*
         * a few notes:
         * - the operand value is ALWAYS the raw value accompanying the instruction
         *   in raw machine code
         * - effective address is the address from where the code would get its value
         *   from; this exists because some opcodes need to modify the memory in some
         *   addressing modes
         * - evaluated operand is the value retrieved from the effective address
         * - PC should be at the address AFTER the instruction's address
         */
        case 1:
            // zero page
            // operand is zeropage address XX
            // effective address is $00XX
            cpu->op_val = mmap_getint8(cpu->mmap, cpu->PC);
            cpu->e_addr = cpu->op_val;
            cpu->op_eval = mmap_getint8(cpu->mmap, cpu->e_addr);
            cpu->PC++;
            break;
        case 2:
            // implied / accumulator
            // there is no operand
            cpu->op_eval = cpu->A;
            cpu->PC++;
            break;
        case 3:
            // absolute
            // operand is address $XXXX
            // effective address is $XXXX
            cpu->op_val = mmap_getint16(cpu->mmap, cpu->PC);
            cpu->e_addr = cpu->op_val;
            cpu->op_eval = mmap_getint8(cpu->mmap, cpu->e_addr);
            cpu->PC += 2;
            break;
        case 4:
            // Indirect Y
            // operand is zeropage address $XX
            // effective address is 16-bit address stored in $00XX + value in Y
            cpu->op_val = mmap_getint8(cpu->mmap, cpu->PC);
            cpu->e_addr = mmap_getint16(cpu->mmap, cpu->op_val) + cpu->Y;
            cpu->op_eval = mmap_getint8(cpu->mmap, cpu->e_addr);
            cpu->PC++;
            break;
        case 5:
            // zero page x/y
            //
            cpu->op_val = mmap_getint8(cpu->mmap, cpu->PC);
            cpu->e_addr = (0xFF & (cpu->op_val + cpu->X));
            // e_addry only exists because LDX uses Y to index and not X
            cpu->e_addry = (0xFF & (cpu->op_val + cpu->Y));
            // NOTE: op_eval does NOT account for Y indexing because it is only
            // used for one instruction, LDX
            cpu->op_eval = mmap_getint8(cpu->mmap, cpu->e_addr);
            cpu->PC++;
            break;
        case 6:
            // absolute Y
            //
            cpu->op_val = mmap_getint16(cpu->mmap, cpu->PC);
            cpu->e_addr = cpu->op_val + cpu->Y;
            cpu->op_eval = mmap_getint8(cpu->mmap, cpu->e_addr);
            cpu->PC += 2;
            break;
        case 7:
            // absolute x
            //
            cpu->op_val = mmap_getint16(cpu->mmap, cpu->PC);
            cpu->e_addr = cpu->op_val + cpu->X;
            cpu->op_eval = mmap_getint8(cpu->mmap, cpu->e_addr);
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
            cpu->op_eval = mmap_getint8(cpu->mmap, cpu->e_addr);
            cpu->PC++;
            break;
        default:
            // invalid addressing mode
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
    cpu->cur_op = ins;
    cpu->state = mmap_getint8(cpu->mmap, cpu->PC);
    // get operand value and effective address
    cpu->PC++;
    _parse_op(cpu);
    // execute opcode
    // NOTE: at this point the PC will point to the NEXT instruction, not the one being executed
    _exec_opc(cpu);
}
