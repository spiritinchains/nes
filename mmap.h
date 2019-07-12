/*
 * mmap is used to emulate the hardware address bus of the nes
 * communicating between the ppu, apu, cpu etc i suppose
 */
typedef struct _mmap
{
    void* index[65536];
    char ram[2048];
} MMAP;

void mmap_init(MMAP* mmap);

void mmap_link_memrg(MMAP* mmap, short idx, char* target, int n);

void* mmap_getptr(MMAP* mmap, short idx);
char mmap_getint8(MMAP* mmap, short idx);