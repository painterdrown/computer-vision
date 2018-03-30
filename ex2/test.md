# Canny 边缘检测算法测试文档

## 开发、测试环境：
  + macOS High Sierra Version 10.13.3
  + g++ Apple LLVM version 9.0.0 (clang-900.0.39.2)

## 原代码(Opencv) vs. 改写代码(CImg)

PS：下面 5 组对比中，前者为原代码处理效果，后者为改写代码处理效果。

1. Gray Scale 对比

![](img/vs/old-gray.png)

![](img/vs/new-gray.png)

2. Gaussian Filter 对比

![](img/vs/old-gaussian.png)

![](img/vs/new-gaussian.png)

3. Sobel Filter 对比

![](img/vs/old-sobel.png)

![](img/vs/new-sobel.png)

4. Non-maximum Suppression 对比

![](img/vs/old-non-maximum.png)

![](img/vs/new-non-maximum.png)

5. Hysteresis Threshold 对比

![](img/vs/old-threshold.png)

![](img/vs/new-threshold.png)
