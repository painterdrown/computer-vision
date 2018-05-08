#include "Canny.h"
#include "Hough.h"

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

int main(int argc, char* argv[]) {
  CImg<float> nms = doCanny(IN_FILE_1, OUT_FILE_GRADIENT_1, OUT_FILE_NMS_1);
  // doCanny(IN_FILE_2, OUT_FILE_GRADIENT_2, OUT_FILE_NMS_2);
  // doCanny(IN_FILE_3, OUT_FILE_GRADIENT_3, OUT_FILE_NMS_3);
  // doCanny(IN_FILE_4, OUT_FILE_GRADIENT_4, OUT_FILE_NMS_4);
  // doCanny(IN_FILE_5, OUT_FILE_GRADIENT_5, OUT_FILE_NMS_5);
  // doCanny(IN_FILE_6, OUT_FILE_GRADIENT_6, OUT_FILE_NMS_6);

  doHough(nms, OUT_FILE_1, IN_FILE_1);
  // doHough(OUT_FILE_NMS_2, OUT_FILE_2);
  // doHough(OUT_FILE_NMS_3, OUT_FILE_3);
  // doHough(OUT_FILE_NMS_4, OUT_FILE_4);
  // doHough(OUT_FILE_NMS_5, OUT_FILE_5);
  // doHough(OUT_FILE_NMS_6, OUT_FILE_6);

	return 0;
}
