// 把1.bmp文件的白色区域变成红色，黑色区域变成绿色
// 编译：g++ -o bin/task2 src/task2.cpp -O2 -lm -lpthread -I/usr/X11R6/include -L/usr/X11R6/lib -lm -lpthread -lX11
// 运行：bin/task2

#include "CImg.h"
using namespace cimg_library;

int main() {
  CImg<unsigned char> img("img/1.bmp");
  cimg_forXY(img, x, y) {
    unsigned char &r = img.atXY(x, y, 0, 0);
    unsigned char &g = img.atXY(x, y, 0, 1);
    unsigned char &b = img.atXY(x, y, 0, 2);
    // 将白色变成红色
    if (r == 255 && g == 255 & b == 255) {
      r = 255;
      g = 0;
      b = 0;
    }
    // 将黑色变成绿色
    if (r == 0 && g == 0 && b == 0) {
      r = 0;
      g = 255;
      b = 0;
    }
  }
  img.display();
  return 0;
}
