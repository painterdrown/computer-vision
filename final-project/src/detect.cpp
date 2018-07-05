#include <time.h>
#include <vector>
#include "CImg.h"
#undef Success
#include "Eigen/Dense"
#include "Eigen/LU"

using namespace std;
using namespace cimg_library;
using namespace Eigen;

#define GRADLIMIT   15
#define THRESHOLD   800
#define DIFF        90
#define BLUR        3
#define SLOPE_FLAG  1
#define PI          3.14159265358979323846

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

void projectiveTransform(const CImg<double> &original_img, CImg<double> &result_img, const MatrixXd &PT) {
  const double a = PT(0, 0),
               b = PT(0, 1),
               c = PT(0, 2),
               d = PT(1, 0),
               e = PT(1, 1),
               f = PT(1, 2),
               g = PT(2, 0),
               h = PT(2, 1);
  const double p1 = h * f - e,
               q1 = b - h * c,
               r1 = e * c - b * f,
               p2 = d - g * f,
               q2 = g * c - a,
               r2 = a * f - d * c,
               p3 = g * e - d * h,
               q3 = a * h - g * b,
               r3 = d * b - a * e;
  cimg_forXY(result_img, u, v) {
    double down = p3 * u + q3 * v + r3;
    if (down == 0.0) continue;
    double x_up = p1 * u + q1 * v + r1;
    double y_up = p2 * u + q2 * v + r2;
    int x = x_up / down;
    int y = y_up / down;
    result_img(u, v, 0, 0) = original_img(x, y, 0, 0);
    result_img(u, v, 0, 1) = original_img(x, y, 0, 1);
    result_img(u, v, 0, 2) = original_img(x, y, 0, 2);
  }
}

void detect(char *file_path, char *result_path) {
  printf("detect\n");

  CImg<double> orgn_img(file_path);
  orgn_img = orgn_img.resize_halfXY();
  CImg<double> gray_img(orgn_img);
  CImg<double> grad_img(orgn_img._width, orgn_img._height, 1, 1, 0);
  CImg<double> rslt_img;

  clock_t time1, time2, time3, time4;

  time1 = clock();

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

  time2 = clock();

  // Find peaks
  vector<Dot*> peaks;
  const int ymin = 0;
  const int ymax = orgn_img.height() - 1;
  const int xmin = 0;
  const int xmax = orgn_img.width() - 1;
  cimg_forXY(hough_img, angle, polar) {
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

  // 排序后取前四的直线
  const int size = peaks.size();
  for (int i = 0; i < 4; ++i) {
    int pos = i;
    for (int j = i+1; j < size; ++j) {
      if (peaks[j]->value > peaks[pos]->value) pos = j;
    }
    Dot *temp = peaks[i];
    peaks[i] = peaks[pos];
    peaks[pos] = temp;
  }
  peaks.resize(4);

  // Transform polar coordinates to rectangular coordinates
  vector<Line*> lines;
  for (int i = 0; i < peaks.size(); ++i) {
    double angle = (double)peaks[i]->x*PI / 180.0;
    double m = -cos(angle) / sin(angle);
    double b = (double)peaks[i]->y / sin(angle);
    lines.push_back(new Line(m, b));
  }

  // Calculate line intersections
  std::vector<Dot*> intersections;
  for (int i = 0; i < lines.size(); ++i) {
    for (int j = i + 1; j < lines.size(); ++j) {
      double m0 = lines[i]->m;
      double m1 = lines[j]->m;
      double b0 = lines[i]->b;
      double b1 = lines[j]->b;
      double x = (b1 - b0) / (m0 - m1);
      double y = (m0*b1 - m1*b0) / (m0 - m1);
      if (x >= 0 && x < orgn_img.width() && y >= 0 && y < orgn_img.height()) {
        intersections.push_back(new Dot(x, y, 0));
      }
    }
  }

  orgn_img.display();

  Dot *dot0, *dot1, *dot2, *dot3;
  double dis[4][4];
  int nearpos[4];
  const int img_width = orgn_img._width;
  const int img_height = orgn_img._height;
  for (int i = 0; i < 4; ++i) {
    double x = intersections[i]->x;
    double y = intersections[i]->y;
    dis[i][0] = pow(x - 0, 2) + pow(y - 0, 2);
    dis[i][1] = pow(img_width - x, 2) + pow(y - 0, 2);
    dis[i][2] = pow(x - 0, 2) + pow(img_height - y, 2);
    dis[i][3] = pow(img_width - x, 2) + pow(img_height - y, 2);
  }
  for (int i = 0; i < 4; ++i) {
    nearpos[i] = 0;
    for (int j = 1; j < 4; ++j) {
      if (dis[j][i] < dis[nearpos[i]][i]) nearpos[i] = j;
    }
  }
  dot0 = intersections[nearpos[0]];
  dot1 = intersections[nearpos[1]];
  dot2 = intersections[nearpos[2]];
  dot3 = intersections[nearpos[3]];
  if (pow(dot0->x - dot1->x, 2) + pow(dot0->y - dot1->y, 2) > pow(dot0->x - dot2->x, 2) + pow(dot0->y - dot2->y, 2)) {
    Dot * temp = dot0;
    dot0 = dot2;
    dot2 = dot3;
    dot3 = dot1;
    dot1 = temp;
  }
  int x0 = dot0->x;
  int y0 = dot0->y;
  int x1 = dot1->x;
  int y1 = dot1->y;
  int x2 = dot2->x;
  int y2 = dot2->y;
  int x3 = dot3->x;
  int y3 = dot3->y;
  int m = (sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2)) + sqrt(pow(x3 - x2, 2) + pow(y3 - y2, 2))) / 2;
  int n = m * 297 / 210;

  time3 = clock();

  MatrixXd A(8,8), PT(3, 3);
  VectorXd b(8), x(8);
  A << x0, y0, 1, 0, 0, 0, 0, 0,
       x1, y1, 1, 0, 0, 0, -m*x1, -m*y1,
       x2, y2, 1, 0, 0, 0, 0, 0,
       x3, y3, 1, 0, 0, 0, -m*x3, -m*y3,
       0, 0, 0, x0, y0, 1, 0, 0,
       0, 0, 0, x1, y1, 1, 0, 0,
       0, 0, 0, x2, y2, 1, -n*x2, -n*y2,
       0, 0, 0, x3, y3, 1, -n*x3, -n*y3;
  b << 0, m, 0, m, 0, 0, n, n;
  x = A.lu().solve(b);
  PT << x(0), x(1), x(2), x(3), x(4), x(5), x(6), x(7), 1;

  rslt_img.assign(m, n, orgn_img._depth, orgn_img._spectrum);
  projectiveTransform(orgn_img ,rslt_img, PT);

  time4 = clock();

  printf("detect edge: %fms\n", (double)(time2 - time1)/1000);
  printf("find lines: %fms\n", (double)(time3 - time2)/1000);
  printf("projective transform: %fms\n", (double)(time4 - time3)/1000);

  rslt_img.save(result_path);
}

int main(int argc, char *argv[]) {
  char *file_path = argv[1];
  char *result_path = argv[2];
  detect(file_path, result_path);

  return 0;
}
