#include "mmap.h"

void mmap_init(MMAP* self, char* rt)
{
    self->root = rt;
}

void* mmap_getptr(MMAP* self, short idx)
{
    return self->root[idx];
}

char mmap_getint8(MMAP* self, short idx)
{
    return *(char*)mmap_getptr(self, idx);
}
