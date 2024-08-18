from PIL import Image, ImageFont, ImageDraw
import string
from bitarray import bitarray
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("filename")
args = parser.parse_args()

font = ImageFont.truetype(args.filename, 8)
left, top, right, bottom = font.getbbox("A", stroke_width=0)
w = right - left + 1
h = bottom - top + 1
# print('w', w, 'h', h)
im = Image.new("RGB", (w, h))
draw = ImageDraw.Draw(im)

ba = bitarray()

ascii = " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~"
# print(len(ascii))
for c in ascii:
    draw.text((0, 0), c, font=font)

    for i in range(w * h):
        coord = (i % w, i // w)
        p = im.getpixel(coord)
        if p[0] > 0:
            ba.append(1)
        else:
            ba.append(0)

    draw.rectangle((0, 0, w, h), fill=(0, 0, 0, 0))
# print(w, h)

# print(len(ba))
# print(len(ascii))
# print(w, h)

# print(len(ba) / (8 * 7))
# print(len(ba.tobytes()) / 7)

print("#ifndef NU_FONT_DATA_H")
print("#define NU_FONT_DATA_H")
print("#include <nucleus/core/config.h>")
print("#define NU__FONT_DATA_WIDTH ", w);
print("#define NU__FONT_DATA_HEIGHT", h);
print("static const nu_byte_t nu__font_data_chars[] = {", ','.join(['0x{:02x}'.format(x) for x in bytes(ascii, 'utf-8')]), "};")
print("static const nu_byte_t nu__font_data[] = {", ','.join(['0x{:02x}'.format(x) for x in ba.tobytes()]), "};")
print("#endif")
