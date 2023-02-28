
#include "common.h"
#include "rom.h"
#include "ppu.h"

extern SDL_Window* window;

SDL_Renderer* renderer;
SDL_Texture* rendertarget;

SDL_Rect bg_tiles[32][30];

SDL_Texture* dummy_tex;

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

    SDL_Surface* surface = SDL_LoadBMP("misc/pattern.bmp");
    dummy_tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // initialize tile positions
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 30; j++)
        {
            bg_tiles[i][j].x = i * 8;
            bg_tiles[i][j].y = j * 8;
            bg_tiles[i][j].w = 8;
            bg_tiles[i][j].h = 8;
        }
    }
}

void graphics_cleanup()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(rendertarget);
    SDL_DestroyTexture(dummy_tex);
}

void graphics_draw()
{
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, rendertarget);
    // printf("NAMETABLE\n");
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 30; j++)
        {
            // printf("%x ", ppu_read8(PPU.nt_base_addr + (i + j * 30)));
            SDL_RenderCopy(renderer, dummy_tex, NULL, &bg_tiles[i][j]);
        }
        // printf("\n");
    }
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, rendertarget, NULL, NULL);
    SDL_RenderPresent(renderer);
}
