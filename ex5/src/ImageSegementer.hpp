#ifndef IMAGE_SEGEMENTER
#define IMAGE_SEGEMENTER

#define T_DELTA 10.0

#include "CImg.h"

using namespace cimg_library;

class ImageSegementer {
public:
	void loadImage(const char *filepath);
	CImg<float> applyGlobalThresholding();
	CImg<float> applyOtsu();
private:
	CImg<float> origin;
	CImg<float> gray;
	CImg<float> gt;
	CImg<float> otsu;
};

#endif