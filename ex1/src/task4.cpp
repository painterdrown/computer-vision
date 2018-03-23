// 在图上绘制一个圆形区域，圆心坐标(50,50)，半径为3，填充颜色为黄色。
// 编译：g++ -o bin/task4 src/task4.cpp -O2 -lm -lpthread -I/usr/X11R6/include -L/usr/X11R6/lib -lm -lpthread -lX11
// 运行：bin/task4

#include "CImg.h"
using namespace cimg_library;

int main() {
  CImg<unsigned char> img("img/1.bmp");
  const unsigned char yellow[] = { 255, 255, 0 };
  img.draw_circle(50, 50, 3, yellow, 1.0f);
  img.display();
  return 0;
}
