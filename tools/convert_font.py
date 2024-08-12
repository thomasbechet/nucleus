from PIL import Image, ImageFont, ImageDraw
import string
from bitarray import bitarray

font = ImageFont.truetype("PublicPixel.ttf", 8)
w, h = font.getsize('A')
print('w', w, 'h', h)
im = Image.new("RGB", (w, h))
draw = ImageDraw.Draw(im)

ba = bitarray()

ascii = " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~éèê"
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

print(len(ba))
print(len(ascii))
print(len(ba) // (w * h))

with open('font.bin', 'wb') as fh:
    ba.tofile(fh)

im.save("img.png")