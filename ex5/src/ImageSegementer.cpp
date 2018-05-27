#include "ImageSegementer.hpp"
#include <stdio.h>
#include <math.h>

void ImageSegementer::loadImage(const char *filepath) {
	origin.assign(filepath);
	gray = origin.get_RGBtoGray();
}

CImg<float> ImageSegementer::applyGlobalThresholding() {
	float T = 22.0f, new_T = 0.0f, delta = 0.0f;  // T 任意，这里取我的幸运数字 22
	float G1_intensity = 0.0f, G2_intensity = 0.0f;
	int G1_count = 0, G2_count = 0;
	int interation = 0;
	gt.assign(gray._width, gray._height, 1, 1, 0.0f);
	do {
		cimg_forXY(gray, x, y) {
			if (gray.atXY(x, y) > T) {
				gt.atXY(x, y) = 255.0f;
				G1_intensity += gray.atXY(x, y);
				++G1_count;
			} else {
				gt.atXY(x, y) = 0.0f;
				G2_intensity += gray.atXY(x, y);
				++G2_count;
			}
		}
		float m1 = G1_intensity / G1_count;
		float m2 = G2_intensity / G2_count;
		new_T = (m1 + m2) / 2;
		delta = abs(new_T - T);
		printf("interation %d: G1_count = %d, G2_count = %d, T = %f, new_T = %f, delta = %f\n", interation, G1_count, G2_count, T, new_T, delta);
		T = new_T;
		interation++;
	} while (delta > T_DELTA);
	return gt;
}

CImg<float> ImageSegementer::applyOtsu() {
	const int L = ceil(gray.max());
	const int all_count = gray._width * gray._height;
	int t_max = 0;
	float g_max = 0.0f;
	for (int t = 0; t <= L; ++t) {
		int foreground_count = 0, background_count = 0;
		float foreground_gray = 0.0f, background_gray = 0.0f;
		cimg_forXY(gray, x, y) {
			if (gray.atXY(x, y) > t) {
				foreground_gray += gray.atXY(x, y);
				++foreground_count;
			} else {
				background_gray += gray.atXY(x, y);
				++background_count;
			}
		}
		float w0 = (float)foreground_count / (float)all_count;
		float w1 = (float)background_count / (float)all_count;
		float u0 = foreground_gray / foreground_count;
		float u1 = background_gray / background_count;
		float g = w0 * w1 * pow(u0 - u1, 2);
		if (g > g_max) {
			t_max = t;
			g_max = g;
			printf("better threshold found: t = %d, g = %f\n", t, g);
		}
	}
	otsu.assign(gray._width, gray._height, 1, 1, 0.0f);
	cimg_forXY(gray, x, y) {
		if (gray.atXY(x, y) > t_max) {
			otsu.atXY(x, y) = 255.0f;
		} else {
			otsu.atXY(x, y) = 0.0f;
		}
	}
	return otsu;
}
