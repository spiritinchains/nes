
#include "common.h"
#include "ppu.h"

struct ppu PPU;

// color palette array generated using python (0: R, 1: G, 2: B)
uint8_t const colors[64][3] = {
    [0x0]  = {82, 82, 82},
    [0x1]  = {1, 26, 81},
    [0x2]  = {15, 15, 101},
    [0x3]  = {35, 6, 99},
    [0x4]  = {54, 3, 75},
    [0x5]  = {64, 4, 38},
    [0x6]  = {63, 9, 4},
    [0x7]  = {50, 19, 0},
    [0x8]  = {31, 32, 0},
    [0x9]  = {11, 42, 0},
    [0xa]  = {0, 47, 0},
    [0xb]  = {0, 46, 10},
    [0xc]  = {0, 38, 45},
    [0xd]  = {0, 0, 0},
    [0xe]  = {0, 0, 0},
    [0xf]  = {0, 0, 0},
    [0x10] = {160, 160, 160},
    [0x11] = {30, 74, 157},
    [0x12] = {56, 55, 188},
    [0x13] = {88, 40, 184},
    [0x14] = {117, 33, 148},
    [0x15] = {132, 35, 92},
    [0x16] = {130, 46, 36},
    [0x17] = {111, 63, 0},
    [0x18] = {81, 82, 0},
    [0x19] = {49, 99, 0},
    [0x1a] = {26, 107, 5},
    [0x1b] = {14, 105, 46},
    [0x1c] = {16, 92, 104},
    [0x1d] = {0, 0, 0},
    [0x1e] = {0, 0, 0},
    [0x1f] = {0, 0, 0},
    [0x20] = {254, 255, 255},
    [0x21] = {105, 158, 252},
    [0x22] = {137, 135, 255},
    [0x23] = {174, 118, 255},
    [0x24] = {206, 109, 241},
    [0x25] = {224, 112, 178},
    [0x26] = {222, 124, 112},
    [0x27] = {200, 145, 62},
    [0x28] = {166, 167, 37},
    [0x29] = {129, 186, 40},
    [0x2a] = {99, 196, 70},
    [0x2b] = {84, 193, 125},
    [0x2c] = {86, 179, 192},
    [0x2d] = {60, 60, 60},
    [0x2e] = {0, 0, 0},
    [0x2f] = {0, 0, 0},
    [0x30] = {254, 255, 255},
    [0x31] = {190, 214, 253},
    [0x32] = {204, 204, 255},
    [0x33] = {221, 196, 255},
    [0x34] = {234, 192, 249},
    [0x35] = {242, 193, 223},
    [0x36] = {241, 199, 194},
    [0x37] = {232, 208, 170},
    [0x38] = {217, 218, 157},
    [0x39] = {201, 226, 158},
    [0x3a] = {188, 230, 174},
    [0x3b] = {180, 229, 199},
    [0x3c] = {181, 223, 228},
    [0x3d] = {169, 169, 169},
    [0x3e] = {0, 0, 0},
    [0x3f] = {0, 0, 0}
};

uint8_t vram[2048];

/* register read/write */

void
ppu_reg_write(enum ppu_reg reg, uint8_t data)
{
    switch (reg)
    {
        case REG_PPUCTRL:
            break;
        case REG_PPUMASK:
            break;
        case REG_OAMADDR:
            break;
        case REG_OAMDATA:
            break;
        case REG_PPUSCROLL:
            break;
        case REG_PPUADDR:
            break;
        case REG_PPUDATA:
            break;
        case REG_OAMDMA:
            break;
    }
}

void
ppu_reg_read(enum ppu_reg reg, uint8_t data)
{
    switch (reg)
    {
        case REG_PPUSTATUS:
            break;
        case REG_PPUDATA:
            break;
        case REG_OAMDATA:
            break;
        default:
            break;
    }
}


void
ppu_cycle()
{
    PPU.dot = (PPU.dot + 1) % 341;
    PPU.scanline = (PPU.scanline + 1) % 240;
    // printf("ppu dot: %d scan: %d\n", PPU.dot, PPU.scanline);
}
