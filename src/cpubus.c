
#include "common.h"
#include "cpubus.h"
#include "rom.h"
#include "ppu.h"

static uint8_t RAM[2048];
static uint8_t blank;

static uint8_t* map[0x10000];

void
bus_init()
{
    // RAM
    for (int i = 0; i < 0x2000; i++)
    {
        map[i] = &RAM[i & 0x7FF];
    }

    for(int i = 0x2000; i < 0x8000; i++)
    {
        map[i] = &blank;
    }

    // TODO: do mapper stuff
    for (int i = 0x8000; i < 0x10000; i++)
    {
        uint16_t idx = i - 0x8000;
        idx &= (ROM.prg_banks > 1) ? 0xFFFF : 0x3FFF;
        map[i] = &ROM.prg[idx];
    }
}

uint8_t 
read8(uint16_t addr)
{
    if (addr >= 0x2000 && addr < 0x4000)
    {
        return ppu_reg_read((addr % 8));
    }
    return *map[addr];
}

void
write8(uint16_t addr, uint8_t data)
{
    // PPU registers
    if (addr >= 0x2000 && addr < 0x4000)
    {
        ppu_reg_write((addr % 8), data);
    }
    if (addr == 0x4014)
    {
        ppu_reg_write(8, data);
    }

    // TODO: protection for read only locations
    *map[addr] = data;
    return;
}

uint16_t
read16(uint16_t addr)
{
    uint16_t r = 0;

    r = read8((addr == 0x00FF) ? 0 : (addr + 1));
    r <<= 8;
    r |= read8(addr);

    return r;
}
