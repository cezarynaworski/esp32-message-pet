#ifndef FONT_H
#define FONT_H

#include <stdint.h>

#define MAX_WORD_SIZE 64
typedef struct
{
    uint32_t unicode;

    uint8_t width;
    uint8_t height;

    int8_t xOffset;
    int8_t yOffset;

    uint8_t xAdvance;

    const uint8_t *bitmap;

} Glyph;


typedef struct
{
    const Glyph *glyphs;

    uint16_t count;

    uint8_t space_width;
    uint8_t lineHeight;

} Font;


const Glyph* find_glyph(uint32_t unicode, const Font* font);


#endif