/*
 * mmap is used to emulate the hardware address bus of the nes
 * communicating between the ppu, apu, cpu etc i suppose
 */
typedef struct _mmap
{
    void* index[65536];
    unsigned char ram[2048];
} MMAP;

void mmap_init(MMAP* mmap);

void mmap_link_memrg(MMAP* mmap, unsigned short idx, char* target, int n);

void* mmap_getptr(MMAP* mmap, unsigned short idx);

unsigned char mmap_getint8(MMAP* mmap, unsigned short idx);
unsigned short mmap_getint16(MMAP* mmap, unsigned short idx);

void mmap_setint8(MMAP* self, unsigned short idx, unsigned int val);
void mmap_setint16(MMAP* self, unsigned short idx, unsigned int val);
