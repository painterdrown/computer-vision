/* 编译运行
  编译：g++ -o bin/task2 src/task2.cpp -O2 -lm -lpthread -I/usr/X11R6/include -L/usr/X11R6/lib -lm -lpthread -lX11
  运行：bin/task2
  结果：img/result2/#_edge.jpg & img/result2/#_result.jpg
*/

#include "CImg.h"
#include <vector>
#include <iostream>  

#define GRADLIMIT   15  // 20
#define THRESHOLD   400  // 650
#define DIFF        90

#define BLUR        3
#define SLOPE_FLAG  1
#define PI          3.14159265358979323846

using namespace std;
using namespace cimg_library;

// 2D Dot(x, y) [value]    
struct Dot {
  int x, y, value;
  Dot(int _x, int _y, int _value) :x(_x), y(_y), value(_value) {}
};

// Calculate the distance  
double distance(double x, double y) {
    return sqrt(x*x + y*y);
}

// Polar coordinate intersection at x  
int crossX(int angle, int distance, int x) {
  double theta = (double)angle * PI / 180.0;
  double m = -cos(theta) / sin(theta);
  double b = (double)distance / sin(theta);
  return m*x + b;
}

// Polar coordinate intersection at y  
int crossY(int angle, int distance, int y) {
  double theta = (double)angle * PI / 180.0;
  double m = -cos(theta) / sin(theta);
  double b = (double)distance / sin(theta);
  return ((double)(y - b) / m);
}

void process(const char *filepath, const char *houghpath) {
  CImg<double> orgn_img(filepath);
  CImg<double> gray_img(orgn_img);
  CImg<double> grad_img(orgn_img.width(), orgn_img.height(), 1, 1, 0);
  CImg<double> rslt_img(orgn_img);
  // CImg<double> empt_img(orgn_img.width(), orgn_img.height());

  // Gray scale;  
  cimg_forXY(gray_img, x, y) {
    double r = gray_img(x, y, 0, 0);
    double g = gray_img(x, y, 0, 1);
    double b = gray_img(x, y, 0, 2);
    double gray = (r * 299 + g * 587 + b * 114 + 500) / 1000;
    gray_img(x, y, 0, 0) = gray;
    gray_img(x, y, 0, 1) = gray;
    gray_img(x, y, 0, 2) = gray;
  }
  
  // Blur
  gray_img.blur(BLUR);
  
  int max_distance = distance(orgn_img.width(), orgn_img.height());
  CImg<double> hough_img(orgn_img.width(), orgn_img.height(), 1, 1, 0);
  // 定义 3*3 领域矩阵 I
  CImg_3x3(I, double);
  // 遍历计算梯度值
  const double radius = 222.2;
  cimg_for3x3(gray_img, x, y, 0, 0, I, double) {
    const double ix = Inc - Ipc;
    const double iy = Icp - Icn;
    double grad = sqrt(ix*ix + iy*iy);
    // 梯度大于阈值才赋值
    if (grad > GRADLIMIT) {
      grad_img(x, y) = grad;
      cimg_forXY(hough_img, a, b) {
        if (abs(distance(a - x, b - y) - radius) < 1.0) {
          hough_img(a, b) += 1;
        }
      }
    }
  }

  hough_img.save(houghpath);
  // hough_img.display();
}

int main() {
  process("img/dataset2/1.jpg", "img/result2/1_hough.jpg");
  // process("img/dataset2/2.jpg", "img/result2/2_hough.jpg");
  // process("img/dataset2/3.jpg", "img/result2/3_hough.jpg");
  // process("img/dataset2/4.jpg", "img/result2/4_hough.jpg");
  // process("img/dataset2/5.jpg", "img/result2/5_hough.jpg");
  // process("img/dataset2/6.jpg", "img/result2/6_hough.jpg");

  return 0;
}
