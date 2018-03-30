# Canny 边缘检测算法测试文档

## 开发、测试环境：
  + macOS High Sierra Version 10.13.3
  + g++ Apple LLVM version 9.0.0 (clang-900.0.39.2)

## 原代码(Opencv) vs. 改写代码(CImg)

PS：下面 5 组对比中，左边为原代码处理效果，右边为改写代码处理效果。

### 1. Gray Scale 对比

<img width="388" height="540" src="img/vs/old-gray.png"/> <img width="388" height="540" src="img/vs/new-gray.png"/>

### 2. Gaussian Filter 对比

<img width="388" height="540" src="img/vs/old-gaussian.png"/> <img width="388" height="540" src="img/vs/new-gaussian.png"/>

### 3. Sobel Filter 对比

<img width="388" height="540" src="img/vs/old-sobel.png"/> <img width="388" height="540" src="img/vs/new-sobel.png"/>

### 4. Non-maximum Suppression 对比

<img width="388" height="540" src="img/vs/old-non-maximum.png"/> <img width="388" height="540" src="img/vs/new-non-maximum.png"/>

### 5. Hysteresis Threshold 对比

<img width="388" height="540" src="img/vs/old-threshold.png"/> <img width="388" height="540" src="img/vs/new-threshold.png"/>

## 参数解析

### 1. 滤波算子

> 参考：[关于高斯滤波的一些理解](https://blog.csdn.net/jingbin_/article/details/52960826)

Canny 采用的滤波算子为 Gaussian Filter，目的减少图片的噪声。常用的滤波算法有均值滤波、中值滤波以及高斯滤波等。

在代码中的高斯滤波，通过 createFilter 构造滤波算子，其参数 sigma 代表标准差，值为 1。现在分别修改为 0.2, 0.5, 0.8 和 8.0, 结果如下：

<img width="474" height="496" src="img/parameters/sigma-0.2.png"/> <img width="474" height="496" src="img/parameters/sigma-0.5.png"/>

<img width="474" height="496" src="img/parameters/sigma-0.8.png"/> <img width="474" height="496" src="img/parameters/sigma-8.0.png"/>

可以看出：sigma 越小，滤波效果越不明显。可是当 sigma 增大到一定值（猜测是 1.0 左右）的时候，滤波效果增强也就不明显了。

### 2. 微分算子

微分算子用于计算灰度图像在水平和垂直两个方向的梯度，课程上介绍的算子有：Prewitt 算子、Sobel 算子和 Roberts 算子。代码里采用的是 Sobel 算子，替换为 Prewitt 算子后，结果对比如下（左边为 Sobel，右边为 Prewitt）：

<img width="474" height="496" src="img/parameters/gradient-sobel.png"/> <img width="474" height="496" src="img/parameters/gradient-prewitt.png"/>

在 Lena 这张图的处理效果中，Prewitt 算子能够滤掉更多噪音，个人觉得更佳。

### 3. Threshold

Canny 算法采用双阈值法从候选边缘点中检测和连接出最终的边缘。双阈值法首先选取高阈值 high 和低阈值 low，然后开始扫描图像。对候选边缘图像 N 中标记为候选边缘点的任一像素点(i, j)进行检测：
  + 若点(i, j)梯度幅值G(i, j)高于高阈值 high，则认为该点一定是边缘点
  + 若点(i, j)梯度幅值G(i, j)低于低阈值 low，则认为该点一定不是边缘点。
  + 而对于梯度幅值处于两个阈值之间的像素点，则将其看作疑似边缘点。再进一步依据边缘的连通性对其进行判断，若该像素点的邻接像素中有边缘点，则认为该点也为边缘点；否则 ,认为该点为非边缘点。

算法中的 low 和 high 分别为 20 和 40，下面是修改后的对比，依次为 (10, 40), (20, 40) 和 (20, 60)。

<img width="474" height="496" src="img/parameters/threshold-10-40.png"/><br/>
<img width="474" height="496" src="img/parameters/threshold-20-40.png"/><br/>
<img width="474" height="496" src="img/parameters/threshold-20-60.png"/>
