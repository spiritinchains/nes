#include "mmap.h"
#include <stdio.h>

void mmap_init(MMAP* self)
{
    int i;
    // mirroring ram
    for (i = 0; i * 2048 < 0x2000; i++)
    {
        mmap_link_memrg(self, i * 2048, self->ram, 0x800);
    }
    /*
     * TODO: 
     * Add and mirror PPU registers
     * Add other IO registers
     * Think about how to implement ROM space
     */
}

void mmap_link_memrg(MMAP* self, unsigned short idx, char* target, int n)
{
    /*!
     * links a memory address to a specific virtual index, and the n addresses
     * after the specific index to the n indices after the virtual index.
     * Example:
     * mmap_link_memrg(180, (char*)0x600250, 4);
     * this will link,
     *  - 0x600250 to 180
     *  - 0x600251 to 181
     *  - 0x600252 to 182
     *  - 0x600253 to 183
     * to the memory map mmap. so doing mmap_getptr(mmap, 180) will yield
     * the address 0x600250.
     */
    int i;
    for (i = 0; (i < n) && ((i + idx) < 65536); i++)
    {
        // we use character arrays for the sole purpose of doing pointer arithmetic
        self->index[idx + i] = target + i;
    }
}

void* mmap_getptr(MMAP* self, unsigned short i)
{
    return self->index[i];
}

unsigned char mmap_getint8(MMAP* self, unsigned short idx)
{
    return *(unsigned char*)mmap_getptr(self, idx);
}

unsigned short mmap_getint16(MMAP* self, unsigned short idx)
{
    /*!
     * returns a 16-bit integer from location idx, following high-endian
     * conventions.
     * This is done manually rather than using C's built-in datatypes so as
     * to be architecture-agnostic (i guess).
     */
    return (((unsigned short)mmap_getint8(self, idx + 1)) << 8) + mmap_getint8(self, idx);
}