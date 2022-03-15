
#include "rom.h"

#include <stdlib.h>
#include <stdio.h>

const char usage[] = "NES Emulator\n\nUsage:\nnesemu [ROM file]\n";

int 
main(int argc, char* argv[])
{
    if (argc < 2) {
        printf(usage);
        exit(0);
    }

    rom_open(argv[1]);    
}
