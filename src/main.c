
#include "cpu.h"
#include "cpubus.h"
#include "rom.h"
#include "timing.h"

#include <stdlib.h>
#include <stdio.h>

const char usage[] = "NES Emulator\n\nUsage:\nnesemu [ROM file]\n";

void init();
void run();

int 
main(int argc, char* argv[])
{
    if (argc < 2) {
        printf(usage);
        exit(0);
    }

    rom_open(argv[1]);

    init();
    run();

}

void
init()
{
    clock_init();
    bus_init();
    cpu_init();

    printf("NMI vector:   $%.2X%.2X\n", read8(0xfffb), read8(0xfffa));
    printf("RESET vector: $%.2X%.2X\n", read8(0xfffd), read8(0xfffc));
    printf("IRQ vector:   $%.2X%.2X\n", read8(0xffff), read8(0xfffe));
}

void
run()
{
    int count = 1;
    while (count < 27000)
    {
        clock_cycle();
        count++;
    }
}
