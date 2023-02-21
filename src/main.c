
#include "common.h"
#include "cpu.h"
#include "cpubus.h"
#include "rom.h"
#include "timing.h"

const char usage[] = "NES Emulator\n\nUsage:\nnesemu [ROM file]\n";

void init();
void run();

SDL_Window* window;
SDL_Event ev;

int 
main(int argc, char* argv[])
{
    if (argc < 2) {
        printf(usage);
        exit(0);
    }

    rom_open(argv[1]);

    window = SDL_CreateWindow(
        "NES Emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN
    );

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
    while (count < 100000)
    {
        // printf("%d\n", count);
        clock_cycle();
        count++;
        while(SDL_PollEvent(&ev))
        {
            switch (ev.type)
            {
                case SDL_QUIT:
                    SDL_Quit();
                    exit(0);
                    break;
            }
        }
    }
}
