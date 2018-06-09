#include "Canny.h"
#include "Hough.h"
#include "CImg.h"
using namespace cimg_library;

#define IN_FILE           "img/dataset1/kobe.jpg"
#define OUT_FILE_GRADIENT "img/result1/kobe_g.jpg"
#define OUT_FILE_NMS      "img/result1/ymhd_n.jpg"
#define OUT_FILE          "img/result1/ymhd_r.jpg"

#define IN_FILE_1           "img/dataset1/1.jpg"
#define OUT_FILE_GRADIENT_1 "img/result1/1_gradient.jpg"
#define OUT_FILE_NMS_1      "img/result1/1_nms.jpg"
#define OUT_FILE_1          "img/result1/1_hough.jpg"

#define IN_FILE_2           "img/dataset1/2.jpg"
#define OUT_FILE_GRADIENT_2 "img/result1/2_gradient.jpg"
#define OUT_FILE_NMS_2      "img/result1/2_nms.jpg"
#define OUT_FILE_2          "img/result1/3_hough.jpg"

#define IN_FILE_3           "img/dataset1/3.jpg"
#define OUT_FILE_GRADIENT_3 "img/result1/3_gradient.jpg"
#define OUT_FILE_NMS_3      "img/result1/3_nms.jpg"
#define OUT_FILE_3          "img/result1/3_hough.jpg"

#define IN_FILE_4           "img/dataset1/4.jpg"
#define OUT_FILE_GRADIENT_4 "img/result1/4_gradient.jpg"
#define OUT_FILE_NMS_4      "img/result1/4_nms.jpg"
#define OUT_FILE_4          "img/result1/4_hough.jpg"

#define IN_FILE_5           "img/dataset1/5.jpg"
#define OUT_FILE_GRADIENT_5 "img/result1/5_gradient.jpg"
#define OUT_FILE_NMS_5      "img/result1/5_nms.jpg"
#define OUT_FILE_5          "img/result1/5_hough.jpg"

#define IN_FILE_6           "img/dataset1/6.jpg"
#define OUT_FILE_GRADIENT_6 "img/result1/6_gradient.jpg"
#define OUT_FILE_NMS_6      "img/result1/6_nms.jpg"
#define OUT_FILE_6          "img/result1/6_hough.jpg"

// g++ -o bin/task src/task.cpp src/Canny.cpp -O2 -lm -lpthread -I/usr/X11R6/include -L/usr/X11R6/lib -lm -lpthread -lX11

int main(int argc, char* argv[]) {
  // CImg<float> ymhd = doCanny(IN_FILE, OUT_FILE_GRADIENT, OUT_FILE_NMS);

	// cimg_forXY(nms, x, y) {
	// 	printf("(%d, %d) %f\n", x, y, nms.atXY(x, y));
	// }

	CImg<float> kobe(IN_FILE);
	cimg_forXY(kobe, x, y) {
		if (kobe.atXY(x, y) == 0.0) kobe.atXY(x, y) = 255.0f;
		else kobe.atXY(x, y) = 0.0f;
		// printf("(%d, %d)  %f\n", x, y, kobe.atXY(x, y));
	}
	kobe.display();
	kobe.save(OUT_FILE_NMS);

  // doCanny(IN_FILE_2, OUT_FILE_GRADIENT_2, OUT_FILE_NMS_2);
  // doCanny(IN_FILE_3, OUT_FILE_GRADIENT_3, OUT_FILE_NMS_3);
  // doCanny(IN_FILE_4, OUT_FILE_GRADIENT_4, OUT_FILE_NMS_4);
  // doCanny(IN_FILE_5, OUT_FILE_GRADIENT_5, OUT_FILE_NMS_5);
  // doCanny(IN_FILE_6, OUT_FILE_GRADIENT_6, OUT_FILE_NMS_6);

  // doHough(nms, OUT_FILE_1, IN_FILE_1);
  // doHough(OUT_FILE_NMS_2, OUT_FILE_2);
  // doHough(OUT_FILE_NMS_3, OUT_FILE_3);
  // doHough(OUT_FILE_NMS_4, OUT_FILE_4);
  // doHough(OUT_FILE_NMS_5, OUT_FILE_5);
  // doHough(OUT_FILE_NMS_6, OUT_FILE_6);

	return 0;
}
