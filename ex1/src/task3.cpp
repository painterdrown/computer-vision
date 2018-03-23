// 在图上绘制一个圆形区域，圆心坐标(50,50)，半径为30，填充颜色为蓝色。
// 编译：g++ -o bin/task3 src/task3.cpp -O2 -lm -lpthread -I/usr/X11R6/include -L/usr/X11R6/lib -lm -lpthread -lX11
// 运行：bin/task3

#include "CImg.h"
using namespace cimg_library;

int main() {
  CImg<unsigned char> img("img/1.bmp");
  const unsigned char blue[] = { 0, 0, 255 };
  img.draw_circle(50, 50, 30, blue, 1.0f);
  img.display();
  return 0;
}
