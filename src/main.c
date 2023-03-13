
#include "common.h"
#include "cpu.h"
#include "cpubus.h"
#include "ppu.h"
#include "rom.h"
#include "timing.h"
#include "graphics.h"

#include <pthread.h>

const char usage[] = "NES Emulator\n\nUsage:\nnesemu [ROM file]\n";

void init();
void cleanup();

void* run(void*);

pthread_t emulation_thread;
bool running;

SDL_Window* window;
SDL_Event ev;

const uint8_t* keyboard_state;

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
        640, 600,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN
    );

    init();
    while (running)
    {
        while(SDL_PollEvent(&ev))
        {
            switch (ev.type)
            {
                case SDL_QUIT:
                    SDL_Quit();
                    running = false;
                    break;
            }
        }
    }
    cleanup();
    exit(0);

}

void
init()
{
    running = true;

    keyboard_state = SDL_GetKeyboardState(NULL);

    clock_init();
    bus_init();
    cpu_init();
    ppu_init();
    graphics_init();

    // printf("NMI vector:   $%.2X%.2X\n", read8(0xfffb), read8(0xfffa));
    // printf("RESET vector: $%.2X%.2X\n", read8(0xfffd), read8(0xfffc));
    // printf("IRQ vector:   $%.2X%.2X\n", read8(0xffff), read8(0xfffe));

    printf("NMI vector:   $%.4X\n", read16(0xfffa));
    printf("RESET vector: $%.4X\n", read16(0xfffc));
    printf("IRQ vector:   $%.4X\n", read16(0xfffe));

    pthread_create(&emulation_thread, NULL, run, NULL);
}

void*
run(void* arg)
{
    while (running)
    {
        clock_cycle();
    }
    printf("Stopping Emulation\n");
}

void cleanup()
{
    // pthread_cancel(emulation_thread);
    pthread_join(emulation_thread, NULL);
    rom_close();
    graphics_cleanup();
}
