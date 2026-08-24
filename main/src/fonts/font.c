#include "fonts/font.h"
#include "stdio.h"
#include <stdint.h>

const Glyph *find_glyph(uint32_t unicode, const Font *font) {
  for (int i = 0; i < font->count; i++) {
    if (font->glyphs[i].unicode == unicode) {
      return &font->glyphs[i];
    }
  }

  return NULL;
}

