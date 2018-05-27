#ifndef HOUGH_H
#define HOUGH_H

#include <string>
#include "CImg.h"         // C++ image library

using namespace cimg_library;
using namespace std;

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
  double theta = (double)angle * cimg::PI / 180.0;
  double m = -cos(theta) / sin(theta);
  double b = (double)distance / sin(theta);
  return m*x + b;
}

// Polar coordinate intersection at y  
int crossY(int angle, int distance, int y) {
  double theta = (double)angle * cimg::PI / 180.0;
  double m = -cos(theta) / sin(theta);
  double b = (double)distance / sin(theta);
  return ((double)(y - b) / m);
}

void hough(CImg<float>& img, CImg<float>& HoughSpace, float in_thresh) {
	const int WIDTH = img._width;
	const int HEIGHT = img._height;

	// Hough transform *******************************************************
	const float WIDTH2 = 0.5*WIDTH;
	const float HEIGHT2 = 0.5*HEIGHT;
	const float DIAGONAL = sqrt(WIDTH*WIDTH+HEIGHT*HEIGHT);
	const int OFFSET_N = (int)DIAGONAL;             // how many bins?
	const int THETA_N = 100;                        // how many bins?
	const float THETA_STEP = cimg::PI/(float)THETA_N;
	HoughSpace.assign(THETA_N, OFFSET_N);
	HoughSpace.fill(0.0f);

	cimg_forXY(img, x, y) {
		if (img(x,y) < in_thresh) continue;
		cimg_forX(HoughSpace, angle) {
			double theta = (double)angle * cimg::PI / 180.0;
			int polar = (int)(x*cos(theta) + y*sin(theta));
			if (polar >= 0 && polar < HoughSpace.height()) {
				HoughSpace(angle, polar) += 1;
			}
    }
	}
	printf("Voting done\n");
	HoughSpace.display("HoughSpace");
}

vector<Line*> findLines(CImg<float> orgn_img, CImg<float> hough_img, int threshold, int diff) {
  vector<Dot*> peaks;
  cimg_forXY(hough_img, angle, polar) {
    const int ymin = 0;
    const int ymax = orgn_img.height() - 1;
    const int xmin = 0;
    const int xmax = orgn_img.width() - 1;
    if (hough_img(angle, polar) > threshold) {  // 是否是峰值
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
          if (distance(peaks[i]->x - angle, peaks[i]->y - polar) < diff) {  // 存在相邻峰值
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
	printf("lines: \n");
  vector<Line*> lines;
  for (int i = 0; i < peaks.size(); ++i) {
    double angle = (double)peaks[i]->x * cimg::PI / 180.0;
    double m = -cos(angle) / sin(angle);
    double b = (double)peaks[i]->y / sin(angle);
    lines.push_back(new Line(m, b));
		printf("y = %fx + %f\n", m, b);
  }

  return lines;
}

void drawLines(CImg<float> &input, vector<Line*> lines, double slope_flag) {
  // Draw lines
  for (int i = 0; i < lines.size(); ++i) {
    const int ymin = 0;
    const int ymax = input.height() - 1;
    const int x0 = (double)(ymin - lines[i]->b) / lines[i]->m;
    const int x1 = (double)(ymax - lines[i]->b) / lines[i]->m;
    const int xmin = 0;
    const int xmax = input.width() - 1;
    const int y0 = xmin*lines[i]->m + lines[i]->b;
    const int y1 = xmax*lines[i]->m + lines[i]->b;
    const double color[] = { 255, 0, 0 };
    if (abs(lines[i]->m) > slope_flag) {
      input.draw_line(x0, ymin, x1, ymax, color);
    } else {
      input.draw_line(xmin, y0, xmax, y1, color);
    }
  }
}

void doHough(CImg<float> input, string outfile, string orig) {
	float in_thresh = 50.0f; // absolute threshold for gradient magnitude
  int line_thresh = 100;
  int diff = 0;

	CImg<float> houghspace, output;
	hough(input, houghspace, in_thresh);
	vector<Line*> lines = findLines(input, houghspace, line_thresh, diff);
  CImg<float> result(orig.c_str());
  drawLines(result, lines, 1.0);
  result.display("Lines");
	output.save(outfile.c_str());
}

#endif