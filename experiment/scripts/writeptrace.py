#_*_coding:utf8_*_
import sys

path_flp = sys.argv[1] #.flp文件路径
path_txt = sys.argv[2] #.txt文件路径，该文件为McPAT输出文件
path_ptrace = sys.argv[3] #.ptrace文件路径，该文件为要生成的hotspot输入文件

# 从.txt文件读取所有模块功耗信息
with open(path_txt) as f:
    found = False
    for line in f:
        if 'Instruction Cache' in line:
            found = True
        elif found and 'Runtime Dynamic' in line:
            # print(line.split()[-2])
            Icache = line.split()[-2]
            break

with open(path_txt) as f:
    found = False
    for line in f:
        if 'Branch Target Buffer' in line:
            found = True
        elif found and 'Runtime Dynamic' in line:
            # print(line.split()[-2])
            BTB = line.split()[-2]
            break

with open(path_txt) as f:
    found = False
    for line in f:
        if 'Branch Predictor' in line:
            found = True
        elif found and 'Runtime Dynamic' in line:
            # print(line.split()[-2])
            Bpred = line.split()[-2]
            break

with open(path_txt) as f:
    found = False
    for line in f:
        if 'Instruction Buffer' in line:
            found = True
        elif found and 'Runtime Dynamic' in line:
            # print(line.split()[-2])
            InstBuffer = line.split()[-2]
            break

with open(path_txt) as f:
    found = False
    for line in f:
        if 'Instruction Decoder' in line:
            found = True
        elif found and 'Runtime Dynamic' in line:
            # print(line.split()[-2])
            InstDecoder = line.split()[-2]
            break

with open(path_txt) as f:
    found = False
    for line in f:
        if 'Renaming Unit' in line:
            found = True
        elif found and 'Runtime Dynamic' in line:
            # print(line.split()[-2])
            RenaUnit = line.split()[-2]
            break

with open(path_txt) as f:
    found = False
    for line in f:
        if 'Data Cache' in line:
            found = True
        elif found and 'Runtime Dynamic' in line:
            # print(line.split()[-2])
            Dcahce = line.split()[-2]
            break

with open(path_txt) as f:
    found = False
    for line in f:
        if 'LoadQ' in line:
            found = True
        elif found and 'Runtime Dynamic' in line:
            # print(line.split()[-2])
            LoadQ = line.split()[-2]
            break

with open(path_txt) as f:
    found = False
    for line in f:
        if 'StoreQ' in line:
            found = True
        elif found and 'Runtime Dynamic' in line:
            # print(line.split()[-2])
            StoreQ = line.split()[-2]
            break

total = 0
Itlb = 0
Dtlb = 0

with open(path_txt) as f:
    found = False
    for line in f:
        if 'Memory Management Unit' in line:
            found = True
        elif found and 'Runtime Dynamic' in line:
            total = line.split()[-2]
            break

with open(path_txt) as f:
    found = False
    for line in f:
        if 'Itlb' in line:
            found = True
        elif found and 'Runtime Dynamic' in line:
            Itlb = line.split()[-2]
            break

with open(path_txt) as f:
    found = False
    for line in f:
        if 'Dtlb' in line:
            found = True
        elif found and 'Runtime Dynamic' in line:
            Dtlb = line.split()[-2]
            break

total = float(total)
Itlb = float(Itlb)
Dtlb = float(Dtlb)
# print(total*Itlb/(Itlb+Dtlb))
ITB = total * Itlb / (Itlb + Dtlb)
# print(total*Dtlb/(Itlb+Dtlb))
DTB = total * Dtlb / (Itlb + Dtlb)

with open(path_txt) as f:
    found = False
    for line in f:
        if 'Integer RF' in line:
            found = True
        elif found and 'Runtime Dynamic' in line:
            # print(line.split()[-2])
            IntRF = line.split()[-2]
            break

with open(path_txt) as f:
    found = False
    for line in f:
        if 'Floating Point RF' in line:
            found = True
        elif found and 'Runtime Dynamic' in line:
            # print(line.split()[-2])
            FpRF = line.split()[-2]
            break

with open(path_txt) as f:
    found = False
    for line in f:
        if 'Instruction Window' in line:
            found = True
        elif found and 'Runtime Dynamic' in line:
            # print(line.split()[-2])
            InsWin = line.split()[-2]
            break

with open(path_txt) as f:
    found = False
    for line in f:
        if 'FP Instruction Window' in line:
            found = True
        elif found and 'Runtime Dynamic' in line:
            # print(line.split()[-2])
            FpInsWin = line.split()[-2]
            break

with open(path_txt) as f:
    found = False
    for line in f:
        if 'ROB' in line:
            found = True
        elif found and 'Runtime Dynamic' in line:
            # print(line.split()[-2])
            ROB = line.split()[-2]
            break

with open(path_txt) as f:
    found = False
    for line in f:
        if 'Integer ALUs' in line:
            found = True
        elif found and 'Runtime Dynamic' in line:
            # print(line.split()[-2])
            IntUs = line.split()[-2]
            break

with open(path_txt) as f:
    found = False
    for line in f:
        if 'Floating Point Units (FPUs)' in line:
            found = True
        elif found and 'Runtime Dynamic' in line:
            # print(line.split()[-2])
            FPUs = line.split()[-2]
            break
with open(path_txt) as f:
    found = False
    for line in f:
        if 'L2' in line:
            found = True
        elif found and 'Runtime Dynamic' in line:
            # print(line.split()[-2])
            L2 = line.split()[-2]
            break

with open(path_txt) as f:
    found = False
    for line in f:
        if 'Memory Controller' in line:
            found = True
        elif found and 'Runtime Dynamic' in line:
            # print(line.split()[-2])
            MemCtrl = line.split()[-2]
            break

with open(path_txt) as f:
    found = False
    for line in f:
        if 'BUSES' in line:
            found = True
        elif found and 'Runtime Dynamic' in line:
            # print(line.split()[-2])
            BUS = line.split()[-2]
            break

# 从.flp文件读取目标模块的名字，存入names[]
names = []
with open(path_flp, 'r') as f:
    lines = f.readlines()
    for line in lines:
        if line == '\n' or line[0] == '#':
            continue

        parts = line.split()
        name = parts[0]
        names.append(name)

# values[]记录目标模块的功耗数据
values = [0 for _ in range(len(names))]
values[0] = round(float(L2) * 22 / 106, 8)
values[1] = round(float(L2) * 60 / 106, 8)
values[2] = round(float(L2) * 22 / 106, 8)
values[3] = Icache
values[4] = round(float(Dcahce), 8)
values[5] = round(float(Dcahce), 8)
values[6] = RenaUnit
values[7] = InstDecoder
values[8] = InstBuffer
values[9] = round(DTB, 8)
values[10] = round(DTB, 8)
values[11] = ITB
values[12] = StoreQ
values[13] = LoadQ
values[14] = Bpred
values[15] = BTB
values[16] = round(float(BUS), 8)
values[17] = round(float(BUS), 8)
values[18] = FpRF
values[19] = IntRF
values[20] = InsWin
values[21] = FpInsWin
values[22] = MemCtrl
values[23] = ROB
values[24] = IntUs
values[25] = FPUs

# 创建power.ptrace
values_str = map(str, values)  # convert float to str
with open(path_ptrace, 'w') as output:
    output.write('\t'.join(names))
    output.write('\n')
    output.write('\t'.join(values_str))
    output.write('\n')

'''
for i in range(len(names)):
    print(i, names[i], values[i])
'''
