#ifndef SPRITE_H
#define SPRITE_H
#include <stdint.h>


typedef struct
{
    uint16_t width;
    uint16_t height;
    const uint16_t *data;
} Sprite;

uint16_t sprite_get_pixel(const Sprite *sprite, int x, int y);

extern const Sprite SPRITE_CLEAR;


#endif