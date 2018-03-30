# Ex2：用 CImg 重写、封装给定的 Canny 代码，并测试

1. 附件有三个 Canny 相关的 Code 以及测试数据若干(测试数据自己转化成 BMP 图像)；

由于原生的 CImg 库不支持 .jpg 格式的图片，需要转成 .bmp 格式。可以通过安装 [ImageMagick](http://www.imagemagick.org) 解决这一问题。

2. 改写 Code1；

封装要求：
  + 所有的图像读写、数据处理只能用 CImg 库(整个工程文件不允许使用 Opencv 之类的第三方库)；
  + 代码封装要求函数接口简洁清晰。

改写结果参见：
  + src/Canny.hpp
  + src/Canny.cpp

3. 对算法的若干组参数，对所有测试图像进行测试，并分析各参数对结果的影响。

算法里面涉及到的参数有：
  + 滤波算子（代码里采用的是 Gaussian 算子）
  + 微分算子（代码里采用的是 Sobel 算子）
  + Hysteresis Threshold 中的上界 high 和下界 low

这些参数对结果的印象参考 [test/test.pdf](https://github.com/painterdrown/computer-vision/blob/master/ex2/test/test.md)。
