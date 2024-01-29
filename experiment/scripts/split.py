#_*_coding:utf8_*_
import sys
import os
import re

path_stats = sys.argv[1]# stats.txt文件路径
path_1234 = sys.argv[2] # 1.txt、2.txt、3.txt、4.txt文件所在文件夹的路径

# 打开文件并读取全部内容
with open(path_stats) as f:
    content = f.read()

# 使用正则表达式分割文本,以"---------- End Simulation Statistics ----------" 为分割标志
segments = re.split(r'---------- End Simulation Statistics   ----------', content)

# 获取分割后的段落数量
num_segments = len(segments)-1

# 如果段落数小于3,打印提示信息,并返回False
if num_segments < 3:
    # print('段落数小于3')
    result = False
else:
    # 如果段落数大于等于3,开始写入文件

    # 对每个段落
    for i in range(num_segments):
        # 打开一个以段落序号命名的文件用于写入

        with open(os.path.join(path_1234, str(i + 1) + '.txt'), 'w') as f:
            # 将当前段落内容写入该文件
            segments[i] = segments[i].lstrip()
            f.write(segments[i])

    # 设置返回值为True
    result = True

print(result)
