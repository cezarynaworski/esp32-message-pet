#include "sprites/sprite.h"
#include "stdlib.h"
//Dummy Sprite to indicate clearing layer
const Sprite SPRITE_CLEAR = {.width = -1, .height = -1, .data = NULL};

uint16_t sprite_get_pixel(const Sprite *sprite, int x, int y)
{
    int pixel_index = y * sprite->width + x;

    
    uint16_t byte = sprite->data[pixel_index];


    return byte;
}

