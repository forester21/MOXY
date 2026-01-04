from collections import defaultdict
from PIL import Image

prefix = "heartFillingHalf"
img = Image.open("./hearts/heartFillingHalf.png").convert("RGBA")
pixels = img.load()

rows = defaultdict(list)

for y in range(img.height):
    for x in range(img.width):
        if pixels[x, y][3] > 0:
            rows[y].append(x)

print(f"    short {prefix}Y[] = {{")
i = 0
for y, xs in rows.items():
    print(f"        {y}, {len(xs)},")
    i = i + 1
print("     };")
print(f"    short {prefix}X[] = {{")
for y, xs in rows.items():
    print(f"        {xs}".replace("[", "").replace("]", "") + ",")
print("     };")

print(f"    short {prefix}Size = {i};")
