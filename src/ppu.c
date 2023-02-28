
#include "ppu.h"
#include "rom.h"

struct ppu PPU;

// NES Color Palette
SDL_Color const colors[64] = {
    [0x0]  = { .r = 82,     .g = 82,    .b = 82,    .a = 0 },
    [0x1]  = { .r = 1,      .g = 26,    .b = 81,    .a = 0 },
    [0x2]  = { .r = 15,     .g = 15,    .b = 101,   .a = 0 },
    [0x3]  = { .r = 35,     .g = 6,     .b = 99,    .a = 0 },
    [0x4]  = { .r = 54,     .g = 3,     .b = 75,    .a = 0 },
    [0x5]  = { .r = 64,     .g = 4,     .b = 38,    .a = 0 },
    [0x6]  = { .r = 63,     .g = 9,     .b = 4,     .a = 0 },
    [0x7]  = { .r = 50,     .g = 19,    .b = 0,     .a = 0 },
    [0x8]  = { .r = 31,     .g = 32,    .b = 0,     .a = 0 },
    [0x9]  = { .r = 11,     .g = 42,    .b = 0,     .a = 0 },
    [0xa]  = { .r = 0,      .g = 47,    .b = 0,     .a = 0 },
    [0xb]  = { .r = 0,      .g = 46,    .b = 10,    .a = 0 },
    [0xc]  = { .r = 0,      .g = 38,    .b = 45,    .a = 0 },
    [0xd]  = { .r = 0,      .g = 0,     .b = 0,     .a = 0 },
    [0xe]  = { .r = 0,      .g = 0,     .b = 0,     .a = 0 },
    [0xf]  = { .r = 0,      .g = 0,     .b = 0,     .a = 0 },
    [0x10] = { .r = 160,    .g = 160,   .b = 160,   .a = 0 },
    [0x11] = { .r = 30,     .g = 74,    .b = 157,   .a = 0 },
    [0x12] = { .r = 56,     .g = 55,    .b = 188,   .a = 0 },
    [0x13] = { .r = 88,     .g = 40,    .b = 184,   .a = 0 },
    [0x14] = { .r = 117,    .g = 33,    .b = 148,   .a = 0 },
    [0x15] = { .r = 132,    .g = 35,    .b = 92,    .a = 0 },
    [0x16] = { .r = 130,    .g = 46,    .b = 36,    .a = 0 },
    [0x17] = { .r = 111,    .g = 63,    .b = 0,     .a = 0 },
    [0x18] = { .r = 81,     .g = 82,    .b = 0,     .a = 0 },
    [0x19] = { .r = 49,     .g = 99,    .b = 0,     .a = 0 },
    [0x1a] = { .r = 26,     .g = 107,   .b = 5,     .a = 0 },
    [0x1b] = { .r = 14,     .g = 105,   .b = 46,    .a = 0 },
    [0x1c] = { .r = 16,     .g = 92,    .b = 104,   .a = 0 },
    [0x1d] = { .r = 0,      .g = 0,     .b = 0,     .a = 0 },
    [0x1e] = { .r = 0,      .g = 0,     .b = 0,     .a = 0 },
    [0x1f] = { .r = 0,      .g = 0,     .b = 0,     .a = 0 },
    [0x20] = { .r = 254,    .g = 255,   .b = 255,   .a = 0 },
    [0x21] = { .r = 105,    .g = 158,   .b = 252,   .a = 0 },
    [0x22] = { .r = 137,    .g = 135,   .b = 255,   .a = 0 },
    [0x23] = { .r = 174,    .g = 118,   .b = 255,   .a = 0 },
    [0x24] = { .r = 206,    .g = 109,   .b = 241,   .a = 0 },
    [0x25] = { .r = 224,    .g = 112,   .b = 178,   .a = 0 },
    [0x26] = { .r = 222,    .g = 124,   .b = 112,   .a = 0 },
    [0x27] = { .r = 200,    .g = 145,   .b = 62,    .a = 0 },
    [0x28] = { .r = 166,    .g = 167,   .b = 37,    .a = 0 },
    [0x29] = { .r = 129,    .g = 186,   .b = 40,    .a = 0 },
    [0x2a] = { .r = 99,     .g = 196,   .b = 70,    .a = 0 },
    [0x2b] = { .r = 84,     .g = 193,   .b = 125,   .a = 0 },
    [0x2c] = { .r = 86,     .g = 179,   .b = 192,   .a = 0 },
    [0x2d] = { .r = 60,     .g = 60,    .b = 60,    .a = 0 },
    [0x2e] = { .r = 0,      .g = 0,     .b = 0,     .a = 0 },
    [0x2f] = { .r = 0,      .g = 0,     .b = 0,     .a = 0 },
    [0x30] = { .r = 254,    .g = 255,   .b = 255,   .a = 0 },
    [0x31] = { .r = 190,    .g = 214,   .b = 253,   .a = 0 },
    [0x32] = { .r = 204,    .g = 204,   .b = 255,   .a = 0 },
    [0x33] = { .r = 221,    .g = 196,   .b = 255,   .a = 0 },
    [0x34] = { .r = 234,    .g = 192,   .b = 249,   .a = 0 },
    [0x35] = { .r = 242,    .g = 193,   .b = 223,   .a = 0 },
    [0x36] = { .r = 241,    .g = 199,   .b = 194,   .a = 0 },
    [0x37] = { .r = 232,    .g = 208,   .b = 170,   .a = 0 },
    [0x38] = { .r = 217,    .g = 218,   .b = 157,   .a = 0 },
    [0x39] = { .r = 201,    .g = 226,   .b = 158,   .a = 0 },
    [0x3a] = { .r = 188,    .g = 230,   .b = 174,   .a = 0 },
    [0x3b] = { .r = 180,    .g = 229,   .b = 199,   .a = 0 },
    [0x3c] = { .r = 181,    .g = 223,   .b = 228,   .a = 0 },
    [0x3d] = { .r = 169,    .g = 169,   .b = 169,   .a = 0 },
    [0x3e] = { .r = 0,      .g = 0,     .b = 0,     .a = 0 },
    [0x3f] = { .r = 0,      .g = 0,     .b = 0,     .a = 0 }
};

uint8_t vram[2048];         // Video RAM - nametables
uint8_t pram[32];           // Palette RAM
uint8_t oam[256];           // Object Attribute Memory - sprites

void
ppu_init()
{
    PPU.dot = 0;
    PPU.scanline = 0;
}

uint8_t
ppu_read8(uint16_t addr)
{
    if (addr >= 0 && addr < 0x2000)
    {
        return ROM.chr[addr];
    }
    if (addr >= 0x2000 && addr < 0x3EFF)
    {
        return vram[addr & 0x1FF];
    }
    if (addr >= 0x3F00 && addr < 0x4000)
    {
        return pram[addr & 0x1F];
    }
}

void
ppu_write8(uint16_t addr, uint8_t data)
{
    if (addr >= 0x2000 && addr < 0x3EFF)
    {
        vram[addr & 0x1FF] = data;
    }
    if (addr >= 0x3F00 && addr < 0x4000)
    {
        pram[addr & 0x1F] = data;
    }
}

/* register read/write */

void
ppu_reg_write(enum ppu_reg reg, uint8_t data)
{
    switch (reg)
    {
        case REG_PPUCTRL:
            printf("PPU Write PPUCTRL\n");
            /*
             * bit 0-1: nametable base
             *      0 = 2000, 
             *      1 = 2400, 
             *      2 = 2800, 
             *      3 = 2C00
             * bit 2: VRAM address increment mode
             * bit 3: sprite pattern table base
             *      0 = 0000, 
             *      1 = 1000
             * bit 4: background pattern table base
             *      0 = 0000, 
             *      1 = 1000
             * bit 5: sprite size
             * bit 6: PPU master/slave select
             * bit 7: Generate NMI on vblank
             */

            PPU.nt_base_addr = ((data & 0x3) << 10) | 0x2000;
            PPU.increment_mode = (data >> 2) & 1;
            PPU.pt_addr = ((data >> 3) & 1) << 12;
            PPU.bg_pt_addr = ((data >> 4) & 1) << 12;
            PPU.spr_size = (data >> 5) & 1;
            PPU.master_slave = (data >> 6) & 1;
            PPU.vblank_nmi = (data >> 7) & 1;
            break;

        case REG_PPUMASK:
            printf("PPU Write PPUMASK\n");
            /*
             * bit 0: greyscale
             * bit 1: show background in leftmost 8 pixels
             * bit 2: show sprites in leftmost 8 pixels
             * bit 3: show background
             * bit 4: show sprites
             * bit 5: emphasize red
             * bit 6: emphasize green
             * bit 7: emphasize blue
             */

            PPU.grayscale = data & 1;
            PPU.show_left8_bg = (data >> 1) & 1;
            PPU.show_left8_sprite = (data >> 2) & 1;
            PPU.show_bg = (data >> 3) & 1;
            PPU.show_sprite = (data >> 4) & 1;
            PPU.emph_red = (data >> 5) & 1;
            PPU.emph_blue = (data >> 6) & 1;
            PPU.emph_red = (data >> 7) & 1;
            break;

        case REG_OAMADDR:
            printf("PPU Write OAMADDR\n");
            break;
        case REG_OAMDATA:
            printf("PPU Write OAMDATA\n");
            break;
        case REG_PPUSCROLL:
            printf("PPU Write PPUSCROLL\n");
            break;
        case REG_PPUADDR:
            printf("PPU Write PPUADDR\n");
            PPU.addr <<= 8;
            PPU.addr |= data;
            break;
        case REG_PPUDATA:
            printf("PPU Write PPUDATA\n");
            PPU.data = data;
            ppu_write8(PPU.addr, data);
            if (PPU.increment_mode == 0)
                PPU.addr++;
            else
                PPU.addr += 32;
            break;
        case REG_OAMDMA:
            printf("PPU Write OAMDMA\n");
            break;
    }
}

uint8_t
ppu_reg_read(enum ppu_reg reg)
{
    switch (reg)
    {
        case REG_PPUSTATUS:
            printf("PPU Read PPUSTATUS\n");
            // clear address latch
            PPU.addr = 0;
            uint8_t ppustatus;
            ppustatus |= PPU.data & 0x1F;
            ppustatus |= PPU.in_vblank;
            break;
        case REG_PPUDATA:
            printf("PPU Read PPUDATA\n");
            break;
        case REG_OAMDATA:
            printf("PPU Read OAMDATA\n");
            break;
        default:
            break;
    }
}


void
ppu_cycle()
{
    PPU.dot = (PPU.dot + 1) % 341;
    if (PPU.dot == 0)
        PPU.scanline = (PPU.scanline + 1) % 262;

    printf("PPU dot: %d scan: %d\n", PPU.dot, PPU.scanline);

    // vblank
    if (PPU.dot == 1 && PPU.scanline == 241)
    {
        PPU.in_vblank = true;
    }
    if (PPU.dot == 1 && PPU.scanline == 261)
    {
        PPU.in_vblank = false;
    }
}
