# Ex3：检测 A4 纸 & 硬币

## Usage

```sh
# 在 ex3 目录下

# 编译（macOS 环境下，若是其他系统请换成对应的 CImg 编译命令）
g++ -o bin/task1 src/task1.cpp -O2 -lm -lpthread -I/usr/X11R6/include -L/usr/X11R6/lib -lm -lpthread -lX11
g++ -o bin/task2 src/task2.cpp -O2 -lm -lpthread -I/usr/X11R6/include -L/usr/X11R6/lib -lm -lpthread -lX11

# 运行
bin/task1
bin/task2
```

## 作业要求

### Task 1

检测普通 A4 打印纸。上面可能有手写笔记或者打印内容，但是拍照时可能角度不正。（参考 img/dataset1）

1. 图像的边缘
2. 计算 A4 纸边缘的各直线方程
3. 提取 A4 纸的四个角点

> 思考：如何在保证精度的结果情况下加快运行速度？

在计算梯度值的时候，若检测到梯度值大于某个阈值，即可马上做霍夫变换。不用等到梯度全部算完之后再去变换。

### Task 2

检测硬币。可能一张图有多枚硬币，也有可能有其他杂物干扰。（参考 img/dataset2）

1. 图像的边缘
2. 把图像中边缘拟合成圆, 圆周像素用红色像素标出
3. 输出图像中硬币的数量
