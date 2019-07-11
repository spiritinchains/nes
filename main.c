#include <stdlib.h>
#include <stdio.h>

#include "cpu.h"

char a[65536];

int main(int argc, char** argv)
{
    FILE* code = fopen("in", "r");

    int i = 0;

    while(!feof(code))
    {
        a[i] = fgetc(code);
        i++;
    }

    CPU cpu;
    MMAP mem;
    mmap_init(&mem, a);
    cpu_init(&cpu, &mem);

    while(cpu.state)
    {
        cpu_cycle(&cpu);
    }
}
