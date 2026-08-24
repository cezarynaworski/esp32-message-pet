#!/usr/bin/env python3
# simple script that generates bitmap C table. It creates .c and .h file respectively in src/fonts and include/fonts
# Example usage (arguments: font.ttf, size, outputname):
# python3 font_generator.py Roboto-Regular.ttf 16 Roboto16 
# Graphic.c and Font.c uses it


from PIL import Image, ImageFont, ImageDraw
import sys


if len(sys.argv) != 4:
    print("usage: python font_generator.py font.ttf size name")
    sys.exit(1)


ttf = sys.argv[1]
size = int(sys.argv[2])
name = sys.argv[3]


font = ImageFont.truetype(ttf, size)

ascent, descent = font.getmetrics()

LINE_HEIGHT = ascent + descent
SPACE_WIDTH = int(font.getlength(" "))


chars = []

for i in range(32, 127):
    chars.append(chr(i))


chars += list("ąćęłńóśźżĄĆĘŁŃÓŚŹŻ")


glyph_data = []


def generate_bitmap(ch):

    
    bbox = font.getbbox(ch)
    print(f"{ch} : {bbox[0]}, {bbox[1]}, {bbox[2]}, {bbox[3]}")

    width = bbox[2] - bbox[0]
    height = bbox[3] - bbox[1]

    if width <= 0 or height <= 0:
        width = 1
        height = 1

    img = Image.new("1", (width, height), 0)

    draw = ImageDraw.Draw(img)

    draw.text((-bbox[0], -bbox[1]), ch, font=font, fill=1)

    bitmap = []

    byte = 0
    bits = 0

    for y in range(height):

        for x in range(width):

            byte <<= 1
            bits += 1

            if img.getpixel((x, y)):
                byte |= 1

            if bits == 8:
                bitmap.append(byte)

                byte = 0
                bits = 0

    if bits != 0:
        byte <<= 8 - bits
        bitmap.append(byte)

    advance = int(font.getlength(ch))

    return (width, height, bbox[0], bbox[1], advance, bitmap)


with open("../include/fonts/" + name + ".h", "w") as h:

    h.write(f"""
#ifndef {name.upper()}_H
#define {name.upper()}_H

#include "font.h"

extern const Font {name};

#endif
""")


with open("../src/fonts/" + name + ".c", "w") as c:

    c.write(f"""
#include "fonts/{name}.h"


""")

    glyphs = []

    for ch in chars:

        width, height, xOffset, yOffset, advance, bitmap = generate_bitmap(ch)

        array_name = f"glyph_{ord(ch):04X}"

        glyphs.append((ch, array_name, width, height, xOffset, yOffset, advance))

        c.write(f"""
static const uint8_t {array_name}[] =
{{
""")

        for b in bitmap:
            c.write(f"0x{b:02X}, ")

        c.write("""
};

""")

    c.write("""
static const Glyph glyphs[] =
{

""")

    for ch, array_name, width, height, xOffset, yOffset, advance in glyphs:

        c.write(f"""
{{
    0x{ord(ch):04X},
    {width},
    {height},
    {xOffset},
    {yOffset},
    {advance},
    {array_name}
}},
""")

    c.write(f"""
}};


const Font {name} =
{{
    glyphs,
    sizeof(glyphs) / sizeof(Glyph),
    {SPACE_WIDTH},
    {LINE_HEIGHT}
}};
""")


print(f"Generated {name}.c and {name}.h")
