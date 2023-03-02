
#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include "common.h"

void graphics_init();
void graphics_draw();
void graphics_cleanup();

void draw_begin();
void draw_end();
void draw_pixel(int, int, SDL_Color);

#endif
