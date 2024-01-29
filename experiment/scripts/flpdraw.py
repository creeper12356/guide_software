#_*_coding:utf8_*_
from __future__ import print_function, division
import sys
import matplotlib.pyplot as plt
import numpy as np

path_flp = sys.argv[1]
path_steady = sys.argv[2]
layer = long(sys.argv[3])
map_title = sys.argv[4]
path_map = sys.argv[5]

fig, axs = plt.subplots(1)
total_width = -np.inf
total_height = -np.inf
with open(path_flp, 'r') as f:
    lines = f.readlines()
    for line in lines:
        if line == '\n' or line[0] == '#':
            continue

        parts = line.split()
        name = parts[0]
        width = float(parts[1])
        height = float(parts[2])
        x = float(parts[3])
        y = float(parts[4])

        rectangle = plt.Rectangle((x, y), width, height, fc='none', ec='black')
        axs.add_patch(rectangle)
        plt.text(x + width / 2, y + height / 2, name, ha='center', va='center', fontsize=8)

        total_width = max(total_width, x + width)
        total_height = max(total_height, y + height)

temps = []

with open(path_steady, 'r') as fp:
    count = 0
    for line in fp:
        if line[0] == 'L' and count <= layer:
            count += 1
            continue
        if line[0] == 'L' and count > layer:
            break
        if count - 1 == layer:
            temps.append(float(line.split()[1]))

length = len(temps)
row, cow = int(np.sqrt(length)), int(np.sqrt(length))
temps = np.reshape(temps, (row, cow))
im = axs.imshow(temps, cmap='hot_r', extent=(0, total_width, 0, total_height))
cbar = fig.colorbar(im, ax=axs)

axs.set_title('{}'.format(map_title))
axs.set_xticks([n for n in np.linspace(0, total_width, 5)])
axs.set_xlabel('Horizontal Position (mm)')

axs.set_yticks([n for n in np.linspace(0, total_height, 5)])
axs.set_ylabel('Vertical Position (mm)')

plt.axis('scaled')
plt.savefig(path_map, dpi=300)
# plt.show()
