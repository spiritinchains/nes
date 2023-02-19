
#ifndef PPU_H
#define PPU_H

#include <inttypes.h>
#include <stdbool.h>

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
    /* PPUCTRL functions */
    uint8_t nt_base_addr;
    bool increment_mode;
    bool pt_addr;
    bool bg_pt_addr;
    bool spr_size;
    bool master_slave;
    bool vblank_nmi;
    /* PPUMASK functions */
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
    int32_t dot;
    int32_t scanline;
};

void ppu_init();
void ppu_cycle();
void ppu_reg_write(enum ppu_reg, uint8_t);

#endif
