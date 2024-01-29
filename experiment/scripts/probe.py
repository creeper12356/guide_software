import sys
import numpy as np

path_steady = sys.argv[1]  # .steady file
layer = sys.argv[2]  # target layer number 0-3
layer = int(layer)
probe_x = sys.argv[3]  # probe's x coordinate
probe_x = float(probe_x)
probe_y = sys.argv[4]  # probe's y coordinate
probe_y = float(probe_y)

temps = []

with open(path_steady, 'r') as fp:
    count = 0
    for line in fp:
        if line[0] == 'L' and count <= layer:
            count = count + 1
            continue
        if line[0] == 'L' and count > layer:
            break
        if count - 1 == layer:
            temps.append(float(line.split()[1]))
# length = len(temps)
# row, cow = int(np.sqrt(length)), int(np.sqrt(length))  # sqrt returns float type, so we need to convert it to int
# temps = np.reshape(temps, (row, cow))

col = round(probe_x / (0.015 / 63))
row = round((0.015 - probe_y) / (0.015 / 63))
inx = int(row * 63 + col)
print(temps[inx])
