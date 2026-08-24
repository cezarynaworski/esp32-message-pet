#!/usr/bin/env python3
# simple script that generates sprite C Table. It creates .c and .h file respectively in src/sprites and include/sprites
# Example usage (arguments: "folder with png files", size, outputname):
# python3 sprite_generator.py folder

from PIL import Image
import argparse
from pathlib import Path


def rgb888_to_rgb565(r, g, b):
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)


def generate_c_array(data, name):
    lines = []

    lines.append(f"static const uint16_t {name}_data[] = {{")

    for i in range(0, len(data), 8):
        row = data[i : i + 8]
        lines.append("    " + ", ".join(f"0x{x:04X}" for x in row) + ",")

    lines.append("};")

    return "\n".join(lines)


def generate_files(input_png, output_name):

    img = Image.open(input_png).convert("RGB")

    # img = reduce_to_palette(img, 0xd2f0)
    width, height = img.size

    pixels = []

    for y in range(height):
        for x in range(width):
            r, g, b = img.getpixel((x, y))
            rgb565 = rgb888_to_rgb565(r, g, b)
            pixels.append(rgb565)

    # .h
    with open(output_name + ".h", "w") as f:
        f.write(f"""#pragma once

#include <stdint.h>
#include "sprites/sprite.h"
extern const Sprite {output_name};

""")

    # .c
    with open(output_name + ".c", "w") as f:

        f.write(f'#include "sprites/{output_name}.h"\n\n')

        f.write(generate_c_array(pixels, output_name))

        f.write(f"""

const Sprite {output_name} =
{{
    .width = {width},
    .height = {height},
    .data = {output_name}_data
    
}};
""")


def generate_folder(input_dir):

    input_dir = Path(input_dir)

    output_name = input_dir.name

    png_files = sorted(input_dir.glob("*.png"))

    with open("../include/sprites/"+output_name + ".h", "w") as h:

        h.write(f"""#pragma once

#include <stdint.h>
#include "sprites/sprite.h"

""")

        for png in png_files:
            sprite_name = png.stem
            h.write(f"extern const Sprite {sprite_name};\n")

    with open("../src/sprites/"+output_name + ".c", "w") as c:

        c.write(f'#include "sprites/{output_name}.h"\n\n')

        for png in png_files:

            sprite_name = png.stem

            img = Image.open(png).convert("RGB")

            width, height = img.size

            pixels = []

            for y in range(height):
                for x in range(width):

                    r, g, b = img.getpixel((x, y))

                    pixels.append(rgb888_to_rgb565(r, g, b))

            c.write(generate_c_array(pixels, sprite_name))

            c.write(f"""

const Sprite {sprite_name} =
{{
    .width = {width},
    .height = {height},
    .data = {sprite_name}_data
}};

""")


if __name__ == "__main__":

    parser = argparse.ArgumentParser()

    parser.add_argument("input", help="folder")

    args = parser.parse_args()

    generate_folder(
        args.input,
    )

    print(f"Generated {args.input}.c and {args.input}.h")
