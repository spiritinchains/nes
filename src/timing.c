
#include "common.h"
#include "timing.h"
#include "cpu.h"
#include "ppu.h"

#include <time.h>

#define NESCLK_MASTER 46

#define NESCLK_CPU_DIV 12
#define NESCLK_PPU_DIV 4

static int NESCLK_COMMON_DIV;
static int count = 0;
static int ppu_sleep = 29658;

static struct timespec st, ed;

size_t cycles = 0;

void 
clock_start()
{
    clock_gettime(CLOCK_MONOTONIC, &st);
}

void 
clock_end()
{
    do {
        clock_gettime(CLOCK_MONOTONIC, &ed);
    }
    while (ed.tv_nsec - st.tv_nsec < NESCLK_MASTER);
}


int 
lcm(int x, int y)
{
    int r = x * y;
    int t;
    while (y != 0)
    {
        t = x % y;
        x = y;
        y = t;
    }
    return r / x;
}

void 
clock_init()
{
    NESCLK_COMMON_DIV = lcm(NESCLK_CPU_DIV, NESCLK_PPU_DIV);
}

void 
clock_cycle()
{
    // printf("CYCLE %d\n", cycles);
    cycles++;
    count = (count + 1) % NESCLK_COMMON_DIV;
    clock_start();
    if (count % NESCLK_CPU_DIV)
    {
        cpu_cycle();
    }
    if (count % NESCLK_PPU_DIV)
    {
        if (ppu_sleep > 0)
            ppu_sleep--;
        else
            ppu_cycle();
    }
    clock_end();
}
