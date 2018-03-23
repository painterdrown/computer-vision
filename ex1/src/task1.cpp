// 读取 1.bmp 文件，并用 CImg.display() 显示
// 编译：g++ -o bin/task1 src/task1.cpp -O2 -lm -lpthread -I/usr/X11R6/include -L/usr/X11R6/lib -lm -lpthread -lX11
// 运行：bin/task1

#include "CImg.h"
using namespace cimg_library;

int main() {
  CImg<unsigned char> img("img/1.bmp");
  img.display();
  return 0;
}
