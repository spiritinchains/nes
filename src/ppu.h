
#ifndef PPU_H
#define PPU_H

#include "common.h"

enum ppu_reg
{
    REG_PPUCTRL = 0,
    REG_PPUMASK = 1,
    REG_PPUSTATUS = 2,
    REG_OAMADDR = 3,
    REG_OAMDATA = 4,
    REG_PPUSCROLL = 5,
    REG_PPUADDR = 6,
    REG_PPUDATA = 7,
    REG_OAMDMA = 8              // in $4014
};

struct ppu
{
    uint8_t data;               // Data bus contents
    uint16_t addr;              // PPU VRAM address
    uint16_t scroll;            // PPU scroll register

    uint16_t nt_base_addr;      // base nametable address
    uint16_t pt_addr;           // sprite pattern table address
    uint16_t bg_pt_addr;        // background pattern table address

    uint16_t t;                 // temporary vram address

    unsigned w : 1;             // PPU address write latch
    unsigned x : 3;             // fine x scrolling

    /* PPUCTRL flags */
    // VRAM address increment per CPU write
    // O: +1, across. 1: +32, down
    bool increment_mode;
    bool spr_size;              // 0: 8x8, 1: 8x16
    bool master_slave;
    bool vblank_nmi;            // generate NMI on vblank
    /* PPUMASK flags */
    bool grayscale;
    bool show_left8_bg;
    bool show_left8_sprite;
    bool show_bg;
    bool show_sprite;
    bool emph_red;
    bool emph_green;
    bool emph_blue;

    /* OAM stuff */
    uint8_t oam_addr;
    uint8_t oam_data;

    /* other */
    bool even_frame;
    bool in_vblank;

    uint8_t nt;
    uint16_t at;
    uint16_t bits_low;
    uint16_t bits_high;
    
    uint32_t dot;
    uint32_t scanline;

    SDL_Color cur_pixels[16];
};

extern struct ppu PPU;

void ppu_init();
void ppu_cycle();

uint8_t ppu_reg_read(enum ppu_reg);
uint8_t ppu_read8(uint16_t);

void ppu_reg_write(enum ppu_reg, uint8_t);
void ppu_write8(uint16_t, uint8_t);

#endif
