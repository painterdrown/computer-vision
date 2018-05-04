#include "Canny.h"
#include "Hough.h"

#define IN_FILE           "img/dataset1/1.jpg"
#define OUT_FILE_GRADIENT "img/result1/1_gradient.jpg"
#define OUT_FILE_NMS      "img/result1/1_nms.jpg"
#define OUT_FILE          "img/result1/1_hough.jpg"

int main(int argc, char* argv[]) {
  doCanny(IN_FILE, OUT_FILE_GRADIENT, OUT_FILE_NMS);
  doHough(OUT_FILE_NMS, OUT_FILE);
	return 0;
}
