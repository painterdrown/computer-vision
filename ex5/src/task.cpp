#include "ImageSegementer.hpp"
#include <stdio.h>

int main(void) {

	for (int i = 1; i <= 100; ++i) {
		printf("********************** processing image %d **********************\n", i);
		char filepath[32], gt_filepath[32], otsu_filepath[32];
		ImageSegementer is;
		sprintf(filepath, "img/dataset/origin/%d.jpg", i);
		sprintf(gt_filepath, "img/dataset/gt/%d.jpg", i);
		sprintf(otsu_filepath, "img/dataset/otsu/%d.jpg", i);
		is.loadImage(filepath);
		is.applyOtsu().save(otsu_filepath);
	}

	return 0;
}
