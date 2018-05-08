# Ex4：检测并矫正 A4 纸 & 图像渐变

## Usage

```sh
# 在 ex4 目录下

# 编译（macOS 环境下，若是其他系统请换成对应的 CImg 编译命令）
g++ -o bin/task1 src/task1.cpp -O2 -lm -lpthread -I/usr/X11R6/include -L/usr/X11R6/lib -lm -lpthread -lX11
g++ -o bin/task2 src/task2.cpp -O2 -lm -lpthread -I/usr/X11R6/include -L/usr/X11R6/lib -lm -lpthread -lX11

# 运行
bin/task1
bin/task2
```

## 作业要求

### Task 1

输入：普通 A4 打印纸，上面可能有手写笔记或者打印内容，但是拍照时可能角度 不正。(参考：img/dataset1)。

输出：已经矫正好的标准普通 A4 纸(长宽比为 A4 纸的比例)，并裁掉无用的其他内容， 只保留完整 A4 纸张。

> 思考：如何在保证精度的结果情况下加快运行速度。

### Task 2

输入图像：（参考：img/dataset2）

输出：根据 Image Morphing 的方法完成中间 11 帧的差值，得到 一个 Image Morphing 的动画视频。
