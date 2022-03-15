
#include "cpubus.h"
#include "rom.h"

static uint8_t RAM[2048];

uint8_t 
read8(uint16_t addr)
{
    // TODO: use pointers when refactoring
    uint8_t result;
    if (addr < 0x2000)
    {
        result = RAM[addr & 0x7FF];
    }
    else if (addr > 0x8000 && addr < 0x10000)
    {
        result = ROM.prg[addr - 0x8000];
    }
    
    return result;
}

void
write8(uint16_t addr, uint8_t data)
{
    if (addr < 0x2000)
    {
        RAM[addr & 0x7FF] = data;
    }
    return;
}

