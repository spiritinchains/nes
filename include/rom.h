
#pragma once

#include <inttypes.h>

struct rom
{
    uint8_t mapper;

    unsigned is_PAL : 1;
    unsigned has_batt_ram : 1;
    unsigned has_trainer : 1;

    enum rom_mirroring
    { 
        MIRRORING_HORIZONTAL, 
        MIRRORING_VERTICAL, 
        MIRRORING_4SCREEN 
    } mirroring;

    uint8_t prg_banks;
    uint8_t ram_banks;
    uint8_t chr_banks;

    uint8_t* prg;
    uint8_t* chr;

    uint8_t* trainer;
};

extern struct rom ROM;

const struct rom* rom_open(const char []);