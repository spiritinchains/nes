
#include "rom.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PRG_SIZE (ROM.prg_banks * 16384)
#define CHR_SIZE (ROM.chr_banks * 8192)

struct rom ROM;

void
print_bytes(char* buf, size_t n)
{
    for (int i = 0; i < n; i++)
    {
        printf("%.2x ", (uint8_t)buf[i]);
    }
    printf("\n");
}


const struct rom*
rom_open(const char fname[])
{

    FILE* romfile = fopen(fname, "rb");
    char header[16];
    fread(header, 1, 16, romfile);

    if (strncmp("NES\x1a", header, 4) != 0)
    {
        printf("Invalid ROM\n");
        return NULL;
    }

    if ((header[6] >> 3) & 1)
    {
        ROM.mirroring = MIRRORING_4SCREEN;
    }
    else if (header[6] & 1)
    {
        ROM.mirroring = MIRRORING_VERTICAL;
    }
    else
    {
        ROM.mirroring = MIRRORING_HORIZONTAL;
    }

    ROM.is_PAL = header[7] & 1;
    ROM.has_batt_ram = (header[6] >> 1) & 1;
    ROM.has_trainer = (header[6] >> 2) & 1;

    ROM.ram_banks = header[8];
    ROM.prg_banks = header[4];
    ROM.chr_banks = header[5];

    ROM.mapper = (uint8_t)((header[7] & 0xF0) | (header[6] >> 4));

    ROM.prg = malloc(PRG_SIZE);
    ROM.chr = malloc(CHR_SIZE);

    ROM.trainer = malloc(512);

    if (ROM.has_trainer)
    {
        fread(ROM.trainer, 1, 512, romfile);
    }

    size_t prg_read = fread(ROM.prg, 1, PRG_SIZE, romfile);
    size_t chr_read = fread(ROM.chr, 1, CHR_SIZE, romfile);

    assert(prg_read == PRG_SIZE);
    assert(chr_read == CHR_SIZE);

    /* finished reading, print messages */

    printf("Finished reading ROM: %s\n", fname);
    printf((ROM.is_PAL) ? "PAL\n" : "NTSC\n");
    printf("Mapper No.: %d\n", ROM.mapper);
    printf("Trainer present: %s\n", (ROM.has_trainer) ? "YES" : "NO");
    printf("Number of PRG-ROM banks: %d\n", ROM.prg_banks);
    printf("Number of CHR-ROM banks: %d\n", ROM.chr_banks);

}

void
rom_close()
{
    free(ROM.prg);
    free(ROM.chr);
}
