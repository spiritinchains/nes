
#include "common.h"

extern SDL_Window* window;

SDL_Renderer* renderer;
SDL_Surface* rendersurface;

void graphics_init()
{
    renderer = SDL_CreateRenderer(
        window, 
        -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
}

void graphics_cleanup()
{
    SDL_DestroyRenderer(renderer);
}

void graphics_draw()
{
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}
