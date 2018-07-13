# Final：数字字符切割和识别

> 郑钊  计算机应用  15331424

[TOC]

## 1. A4 纸检测 & 数字切割

### 1.1. A4 纸检测

这里直接使用之前作业的算法。

1. 编译

```sh
g++ -o bin/detect src/detect.cpp -O2 -lm -lpthread -I /usr/X11R6/include -L /usr/X11R6/lib -lX11
```

2. 运行

```
bin/detect 1

corner 1: (232, 800)
corner 2: (2238, 694)
corner 3: (382, 3690)
corner 4: (2436, 3510)
detection:      1016.268000ms
segmentation:   123.496000ms
```

```sh
bin/detect 2

corner 1: (328, 36)
corner 2: (2298, 36)
corner 3: (278, 2904)
corner 4: (2348, 2904)
detection:      750.023000ms
segmentation:   62.731000ms
```

<img src="images/a4/1.jpg" height="400px"><img src="images/a4/2.jpg" height="400px">

<img src="images/a4_bi/1.jpg" height="400px"><img src="images/a4_bi/2.jpg" height="400px">

### 1.2. 数字切割

数字切割这里主要思路是：**先进行行分割，再进行列分割**。

行分割：目的是为了分割出 A4 张上的每一行数字。具体做法：

1. 统计 A4 纸在 y 轴方向上的密度。
2. 通过一个阈值（算法里取 80）来找出所有的峰，每一个峰则对应一行数字。

列分割：目的是为了分割出每一行数字中的每一列的数字。具体做法与行分割思想相似。

<img src="images/a4_seg/1.jpg" height="400px"><img src="images/a4_seg/2.jpg" height="400px">

<img src="images/seg_1.png" height="400px"><img src="images/seg_2.png" height="400px">

在做列分割的时候，我碰到了下面这个问题：

> 连写的两个数字分不开。

这里的解决思路是，在做列分割的时候，控制好相邻峰之间的间隔，如果间隔太近说明很有可能是两个连写的数字，这个时候不能看成是一个峰，而要分成两个。

## 2. 在 MNIST 上训练和测试

### 2.1. 准备数据集 MNIST

在官网下载的 MNIST 是下面的格式：

+ t10k-images-idx3-ubyte
+ t10k-labels-idx1-ubyte
+ train-images-idx3-ubyte
+ train-labels-idx1-ubyte

不太适合直接使用，于是在我 [Kaggle](https://www.kaggle.com/scolianni/mnistasjpg) 上找到了 MNIST 的 jpg 版本：

<img src="images/mnistasjpg.png" height="200px">

准备好数据集，下面开始训练。

### 2.2. 训练

```sh
python src/svm_train.py

[2018-07-13 10:08:18] read train data
[2018-07-13 10:09:23] begin training
[2018-07-13 10:14:25] trained model: SVC(C=1.0, cache_size=200, class_weight=None, coef0=0.0,
  decision_function_shape='ovr', degree=3, gamma='auto', kernel='rbf',
  max_iter=-1, probability=False, random_state=None, shrinking=True,
  tol=0.001, verbose=False)
[2018-07-13 10:14:35] trained model is saved at: /Users/painterdrown/codespace/cv/computer-vision/final-project/models/svm.pkl
```

<img src="images/train.png" height="100px">

训练的时间大约是 5 分钟，训练后的模型放在 `models/` 目录下。

### 2.3. 测试

```sh
python src/svm_test.py 1

[2018-07-13 11:12:41] load the svm model via pickle
[2018-07-13 11:12:44] begin testing
[2018-07-13 11:12:46] result: ['14514511', '15555355079', '952521111105110055', '19134511', '15156211004', '4553115780500025', '55534111', '15156513009', '43252111180628004']
```

```sh
python src/svm_test.py 2

[2018-07-13 11:12:49] load the svm model via pickle
[2018-07-13 11:12:52] begin testing
[2018-07-13 11:12:52] result: ['15151427', '15521221510', '745121111105090000']
```

## 3. 在自制数据集 ptddigits 上训练和测试

观察 MNIST 中的数字图片，发现外国人的数字手写习惯与我们相差比较大，于是我打算尝试制作自己的手写数字训练集，看能否提高正确率。

### 3.1. 准备数据集 ptddigits

我找身边的同学，朋友收集了他们手写的 0~9 的 10 个数字，然后制作了大约每个数字有 100 张图片的数据集，如下所示：

<img src="data/ptddigits/origin/0.jpg" height="100px"><img src="data/ptddigits/origin/1.jpg" height="100px"><img src="data/ptddigits/origin/2.jpg" height="100px"><img src="data/ptddigits/origin/3.jpg" height="100px">
<img src="data/ptddigits/origin/4.jpg" height="100px"><img src="data/ptddigits/origin/5.jpg" height="100px"><img src="data/ptddigits/origin/6.jpg" height="100px"><img src="data/ptddigits/origin/7.jpg" height="100px">
<img src="data/ptddigits/origin/8.jpg" height="100px"><img src="data/ptddigits/origin/9.jpg" height="100px">

应用前面的数字提取算法，最终的 ptddigits 数据集长这样：

<img src="images/ptddigits.png" height="200px">

我自己制作的数据集 ptddigits 与 MINIST 在每一张数字图片上存在两点不同：

1. 之前是由于 MNIST 数据集上的数字普遍都比较粗，所以需要对测试的图片做 dilate 处理进行加粗。这次训练用的是自己制作数据集，就不用 dilate。
2. 为了 resize 的时候不会损失太多信息，我选择 resize 的图片大小是 64x64，而不是 28x28。

### 3.2. 训练

```sh
python src/svm_train.py

[2018-07-13 10:33:37] read train data
[2018-07-13 10:33:45] begin training
[2018-07-13 10:33:52] trained model: SVC(C=1.0, cache_size=200, class_weight=None, coef0=0.0,
  decision_function_shape='ovr', degree=3, gamma='auto', kernel='rbf',
  max_iter=-1, probability=False, random_state=None, shrinking=True,
  tol=0.001, verbose=False)
[2018-07-13 10:33:55] trained model is saved at: /Users/painterdrown/codespace/cv/computer-vision/final-project/models/svm_ptddigits.pkl
```

### 3.3. 测试

1. 提取测试图片中的数字

```sh
bin/detect 1
bin/detect 2
```

2. resize

```sh
python src/resize.py 1
python src/resize.py 2
```

3. SVM 测试

```sh
python src/svm_test.py 1

[2018-07-12 23:25:03] load the svm model via pickle
[2018-07-12 23:25:04] begin testing
[2018-07-12 23:25:06] result: ['19199911', '13936993009', '937521114906380037', '19134511', '13636919009', '9993111780660091', '14139511', '19986919004', '63252111980638002']
```

```sh
python src/svm_test.py 2

[2018-07-12 23:25:13] load the svm model via pickle
[2018-07-12 23:25:14] begin testing
[2018-07-12 23:25:15] result: ['16991999', '13521921390', '445121199195090000']
```

可以看到，相比于之前将 MNIST 数据集作为训练数据，这次的效果反而更差。

其中最大的原因还是自制数据集的训练样本太少。要解决这一个问题只能是多花时间扩充训练样本，这个的成本太大了，遂暂时放弃。
