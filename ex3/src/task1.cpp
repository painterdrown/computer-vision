/* 编译运行
  编译：g++ -o bin/task1 src/task1.cpp -O2 -lm -lpthread -I/usr/X11R6/include -L/usr/X11R6/lib -lm -lpthread -lX11
  运行：bin/task1
  结果：img/result1/#_edge.jpg & img/result1/#_result.jpg
*/

/* 调参
  1.jpg  3  15  800   80
  2.jpg  3  20  800   50
  3.jpg  3  20  800   50
         3  15  1500  80
  4.jpg  3  15  800   150
         3  15  800   80
         3  10  1200  80
  5.jpg  3  15  800   150
         3  10  1000  150
  6.jpg  3  10  1000  150
  结果：  3  15  800   90
*/

#include "CImg.h"
#include <vector>
#include <iostream>

#define FILE_PATH    "img/dataset1/6.jpg"
#define EDGE_PATH    "img/result1/6_edge.jpg"
#define RESULT_PATH  "img/result1/6_result.jpg"

#define GRADLIMIT   15
#define THRESHOLD   800
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

// 2D Straight Line(x, y) [y = m*x+b]  
struct Line {
  double m, b;
  Line(double _m, double _b) :m(_m), b(_b) {}
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

int main() {
  CImg<double> orgn_img(FILE_PATH);
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
  CImg<double> hough_img(360, max_distance, 1, 1, 0);
  // 定义 3*3 领域矩阵 I
  CImg_3x3(I, double);
  // 遍历计算梯度值
  cimg_for3x3(gray_img, x, y, 0, 0, I, double) {
    const double ix = Inc - Ipc;
    const double iy = Icp - Icn;
    double grad = sqrt(ix*ix + iy*iy);
    // 梯度大于阈值才赋值
    if (grad > GRADLIMIT) {
      grad_img(x, y) = grad;
      cimg_forX(hough_img, angle) {
        double theta = (double)angle * PI / 180.0;
        int polar = (int)(x*cos(theta) + y*sin(theta));
        if (polar >= 0 && polar < hough_img.height()) {
          hough_img(angle, polar) += 1;
        }
      }
    }
  }

  // Find peaks
  vector<Dot*> peaks;
  cimg_forXY(hough_img, angle, polar) {
    const int ymin = 0;
    const int ymax = orgn_img.height() - 1;
    const int xmin = 0;
    const int xmax = orgn_img.width() - 1;
    if (hough_img(angle, polar) > THRESHOLD) {  // 是否是峰值
      bool flag = false;
      const int x0 = crossY(angle, polar, ymin);
      const int x1 = crossY(angle, polar, ymax);
      const int y0 = crossX(angle, polar, xmin);
      const int y1 = crossX(angle, polar, xmax);
      if ((x0 >= 0 && x0 <= xmax) ||  // 表示的直线是否在图像内
          (x1 >= 0 && x1 <= xmax) ||
          (y0 >= 0 && y0 <= ymax) ||
          (y1 >= 0 && y1 <= ymax)) {
        for (int i = 0; i < peaks.size(); ++i) {  // 遍历数组，找相邻峰值
          if (distance(peaks[i]->x - angle, peaks[i]->y - polar) < DIFF) {  // 存在相邻峰值
            flag = true;
            if (peaks[i]->value < hough_img(angle, polar)) {  // 如果比相邻峰值还大
              peaks[i] = new Dot(angle, polar, hough_img(angle, polar));  // 替换为当前峰值
            }
          }
        }
        if (flag == false) {  // 当前峰值无相邻峰值
          peaks.push_back(new Dot(angle, polar, hough_img(angle, polar)));  // 加入新峰值
        }
      }
    }
  }

  // Transform polar coordinates to rectangular coordinates
  cout << "lines: " << endl;
  vector<Line*> lines;
  for (int i = 0; i < peaks.size(); ++i) {
    double angle = (double)peaks[i]->x*PI / 180.0;
    double m = -cos(angle) / sin(angle);
    double b = (double)peaks[i]->y / sin(angle);
    lines.push_back(new Line(m, b));
    cout << "y = " << m << "x + " << b << endl;
  }

  // Calculate line intersections
  std::cout << std::endl << "intersections: " << std::endl;
  std::vector<Dot*> intersections;
  for (int i = 0; i < lines.size(); ++i) {
    for (int j = i + 1; j < lines.size(); ++j) {
      double m0 = lines[i]->m;
      double m1 = lines[j]->m;
      double b0 = lines[i]->b;
      double b1 = lines[j]->b;
      double x = (b1 - b0) / (m0 - m1);
      double y = (m0*b1 - m1*b0) / (m0 - m1);
      if (x >= 0 && x < rslt_img.width() && y >= 0 && y < rslt_img.height()) {
        intersections.push_back(new Dot(x, y, 0));
        std::cout << "(" << x << ", " << y << ")" << std::endl;
      }
    }
  }

  // Draw lines
  for (int i = 0; i < lines.size(); ++i) {
    const int ymin = 0;
    const int ymax = rslt_img.height() - 1;
    const int x0 = (double)(ymin - lines[i]->b) / lines[i]->m;
    const int x1 = (double)(ymax - lines[i]->b) / lines[i]->m;
    const int xmin = 0;
    const int xmax = rslt_img.width() - 1;
    const int y0 = xmin*lines[i]->m + lines[i]->b;
    const int y1 = xmax*lines[i]->m + lines[i]->b;
    const double color[] = { 255, 0, 0 };
    if (abs(lines[i]->m) > SLOPE_FLAG) {
      rslt_img.draw_line(x0, ymin, x1, ymax, color);
      // empt_img.draw_line(x0, ymin, x1, ymax, color);
    } else {
      rslt_img.draw_line(xmin, y0, xmax, y1, color);
      // empt_img.draw_line(x0, ymin, x1, ymax, color);
    }
  }

  // Draw intersections  
  for (int i = 0; i < intersections.size(); ++i) {
    const double color[] = { 255, 0, 0 };
    rslt_img.draw_circle(intersections[i]->x, intersections[i]->y, 22, color);
    // empt_img.draw_circle(intersections[i]->x, intersections[i]->y, 22, color);
  }

  grad_img.save(EDGE_PATH);
  rslt_img.save(RESULT_PATH);

  // Display images
  // orgn_img.display("Original Image");
  // gray_img.display("Gray Blur Image");
  grad_img.display("Gradient Image");
  hough_img.display("Hough Image");
  rslt_img.display("Result Image");
  // empt_img.display();

  return 0;
}
