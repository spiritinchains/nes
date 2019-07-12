#include <stdlib.h>
#include <stdio.h>

#include "cpu.h"

int main(int argc, char** argv)
{
    //FILE* code = fopen("in", "r");

    int i = 0;

    CPU cpu;
    MMAP mem;
    mmap_init(&mem);
    cpu_init(&cpu, &mem);

    //----- testing mmap
    printf("%x ", mem.ram);
    printf("%x ", mmap_getptr(&mem, 0x0));
    printf("%x ", mmap_getptr(&mem, 0x20));
    printf("%x ", mmap_getptr(&mem, 0x40));
    printf("%x ", mmap_getptr(&mem, 0x801));

    // while(!feof(code))
    // {
    //     mem.ram[i] = fgetc(code);
    //     i++;
    // }

    // while(cpu.state)
    // {
    //     cpu_cycle(&cpu);
    // }
}
