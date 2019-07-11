/*
 * mmap is used to emulate the hardware address bus of the nes
 * communicating between the ppu, apu, cpu etc i suppose
 */
typedef struct _mmap
{
    char* root;
} MMAP;

void mmap_init(MMAP* mmap, char* rt);

void* mmap_getptr(MMAP* mmap, short idx);
char mmap_getint8(MMAP* mmap, short idx);