#include <stdlib.h>
#include <stdio.h>

#include "cpu.h"

int main(int argc, char** argv)
{
    const char text_usage[] = "Usage\nnes [rom file]\n";

    printf("%d", argc);

    // check argument
    if (argc <= 1)
    {
        printf(text_usage);
    }

    // initialization code
    CPU cpu;
    MMAP mem;
    mmap_init(&mem);
    cpu_init(&cpu, &mem);
}
