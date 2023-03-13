
#include "common.h"
#include "controls.h"

static uint8_t ctrl_bits1;
static uint8_t ctrl_bits2;

extern uint8_t* keyboard_state;

void 
ctrl_write(uint8_t data)
{
    // TODO: make keybindings reassignable

    /*
     * PORT 1:
     * SHIFT BUTTON KB BUTTON
     * 0     A      "X"
     * 1     B      "Z"
     * 2     SELECT "RETURN"
     * 3     START  "SPACE"
     * 4     UP     "UP ARROW"
     * 5     DOWN   "DOWN ARROW"
     * 6     LEFT   "LEFT ARROW"
     * 7     RIGHT  "RIGHT ARROW"
     */
    
    ctrl_bits1 = 0;
    ctrl_bits1 |= keyboard_state[SDL_SCANCODE_X];
    ctrl_bits1 |= keyboard_state[SDL_SCANCODE_Z] << 1;
    ctrl_bits1 |= keyboard_state[SDL_SCANCODE_RETURN] << 2;
    ctrl_bits1 |= keyboard_state[SDL_SCANCODE_SPACE] << 3;
    ctrl_bits1 |= keyboard_state[SDL_SCANCODE_UP] << 4;
    ctrl_bits1 |= keyboard_state[SDL_SCANCODE_DOWN] << 5;
    ctrl_bits1 |= keyboard_state[SDL_SCANCODE_LEFT] << 6;
    ctrl_bits1 |= keyboard_state[SDL_SCANCODE_RIGHT] << 7;

    // TODO: add second port
}

uint8_t 
ctrl_read(int port)
{
    uint8_t r = 0;

    if (port == 1)
    {
        r |= ctrl_bits1 & 1;
        ctrl_bits1 >>= 1;
    }

    return r;
}
