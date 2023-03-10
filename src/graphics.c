
#include "common.h"
#include "rom.h"

extern SDL_Window* window;

SDL_Renderer* renderer;
SDL_Texture* rendertarget;
// SDL_Texture* dummy_tex;

SDL_mutex* draw_mutex;

void graphics_init()
{
    renderer = SDL_CreateRenderer(
        window, 
        -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    rendertarget = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        256, 240
    );

    // SDL_Surface* surface = SDL_LoadBMP("misc/pattern.bmp");
    // dummy_tex = SDL_CreateTextureFromSurface(renderer, surface);
    // SDL_FreeSurface(surface);
    draw_mutex = SDL_CreateMutex();
}

void graphics_cleanup()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(rendertarget);
    // SDL_DestroyTexture(dummy_tex);
}

void graphics_draw()
{
    int draw = SDL_TryLockMutex(draw_mutex);
    if (draw == 0)
    {
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, rendertarget, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_UnlockMutex(draw_mutex);
    }
}

void draw_begin()
{
    SDL_LockMutex(draw_mutex);
    SDL_SetRenderTarget(renderer, rendertarget);
}

void draw_end()
{
    SDL_SetRenderTarget(renderer, NULL);
    SDL_UnlockMutex(draw_mutex);
}

void draw_pixel(int x, int y, SDL_Color px)
{
    SDL_SetRenderDrawColor(renderer, px.r, px.g, px.b, px.a);
    SDL_RenderDrawPoint(renderer, x, y);
}
