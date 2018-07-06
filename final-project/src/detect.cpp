#include <time.h>
#include <vector>
#include "CImg.h"
#undef Success
#include "Eigen/Dense"
#include "Eigen/LU"

using namespace std;
using namespace cimg_library;
using namespace Eigen;

#define PI          3.14159265358979323846
#define GRADLIMIT   15
#define DIFF        90
#define BLUR        3
#define SLOPE_FLAG  1
#define THRESHOLD1  800
#define THRESHOLD2  130
#define THRESHOLD3  80
#define THRESHOLD4  5
#define THRESHOLD5  28
#define MARGIN      10

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

CImg<double> detect(const CImg<double> &orgn_img) {
  CImg<double> gray_img(orgn_img);
  CImg<double> grad_img(orgn_img._width, orgn_img._height, 1, 1, 0);
  CImg<double> rslt_img;

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
  const int ymin = 0;
  const int ymax = orgn_img.height() - 1;
  const int xmin = 0;
  const int xmax = orgn_img.width() - 1;
  cimg_forXY(hough_img, angle, polar) {
    if (hough_img(angle, polar) > THRESHOLD1) {  // 是否是峰值
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

  return rslt_img;
}

void clear_point(CImg<double> &img, int x, int y) {
  if (img(x, y) != 0) return;
  else img(x, y) = 255;
  if (x-1 >= 0) clear_point(img, x-1, y);
  if (x+1 < img._width) clear_point(img, x+1, y);
  if (y-1 >= 0) clear_point(img, x, y-1);
  if (y+1 < img._height) clear_point(img, x, y+1);
}

void clear_edges(CImg<double> &img) {
  // 上边、下边
  for (int x = 0; x < img._width; ++x) {
    clear_point(img, x, 0);
    clear_point(img, x, img._height-1);
  }
  // 左边、右边
  for (int y = 0; y < img._height; ++y) {
    clear_point(img, 0, y);
    clear_point(img, img._width-1, y);
  }
}

vector<int> divide_rows(const CImg<double> &img) {
  vector<int> hill, integration, rows;
  hill.resize(img._height);
  integration.resize(img._height);
  bool flag = true;
  for (int y = 0; y < img._height; ++y) {
    hill[y] = 0;
    for (int x = 0; x < img._width; ++x) {
      hill[y] += img(x, y) == 0 ? 1 : 0;
    }
    hill[y] = hill[y] < THRESHOLD4 ? 0 : hill[y];
    integration[y] = (hill[y] == 0) ? 0 : (flag ? hill[y] : hill[y] + integration[y-1]);
    flag = hill[y] == 0;
  }
  int peak = -1;
  for (int y = img._height-1; y >= 0; --y) {
    // 还没找到一个顶峰
    if (peak == -1) {
      if (integration[y] > THRESHOLD3) {
        rows.push_back(y+MARGIN < img._height ? y+MARGIN : img._height-1);
        peak = y;
      }
    } else {
      if (integration[y] == 0) {
        rows.push_back(y-MARGIN >= 0 ? y-MARGIN : 0);
        peak = -1;
      }
    }
  }
  return rows;
}

vector<int> divide_cols(const CImg<double> &img, int y0, int y1) {
  vector<int> rols;
  if (y0 >= y1) return rols;
  return rols;
}

CImg<double> cut(const CImg<double> &img, int y0, int y1, int x0, int x1) {}

void resize_28by28(CImg<double> &img) {}

int main(int argc, char *argv[]) {
  char *filename = argv[1];
  char origin_path[50];
  char a4_path[50];
  char a4_bi_path[50];
  CImg<double> origin, a4, a4_bi;
  clock_t time0, time1, time2, time3, time4;

  sprintf(origin_path, "data/test/%s", filename);
  sprintf(a4_path, "data/test/a4_%s", filename);
  sprintf(a4_bi_path, "data/test/a4_bi_%s", filename);

  time0 = clock();

  // 提取 A4
  origin.assign(origin_path);
  origin = origin.resize_halfXY();
  a4 = detect(origin);
  a4.save(a4_path);
  time1 = clock();
  printf("detection: %fms\n", (double)(time1-time1)/1000);

  // 二值化
  a4_bi.assign(a4._width, a4._height, 1, 1, 0);
  cimg_forXY(a4, x, y) {
    a4_bi(x, y) = (a4(x, y, 0, 0) > THRESHOLD2 && a4(x, y, 0, 0) > THRESHOLD2 && a4(x, y, 0, 0) > THRESHOLD2) ? 255 : 0;
  }
  clear_edges(a4_bi);
  time2 = clock();
  printf("binarization: %fms\n", (double)(time2-time1)/1000);

  // 行分割
  const double black[] = { 0, 0, 0 };
  vector<int> rows = divide_rows(a4_bi);
  for (int i = 0; i < rows.size(); i += 2) {
    // if (rows[i]-rows[i+1] < THRESHOLD5) continue;
    vector<int> cols = divide_cols(a4_bi, rows[i], rows[i+1]);
    for (int j = 0; j < cols.size(); j += 2) {
      a4_bi.draw_line(cols[j], rows[i], cols[j+1], rows[i], black);
      a4_bi.draw_line(cols[j], rows[i+1], cols[j+1], rows[i+1], black);
      a4_bi.draw_line(cols[j], rows[i], cols[j], rows[i+1], black);
      a4_bi.draw_line(cols[j+1], rows[i], cols[j+1], rows[i+1], black);
      // CImg<double> digit = cut(a4_bi, rows[i], rows[i+1], cols[j], cols[j+1]);
    }
  }
  a4_bi.save(a4_bi_path);

  return 0;
}
