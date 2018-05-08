#ifndef CANNY_H
#define CANNY_H

#include <cmath>
#include <string>
#include <vector>
#include "CImg.h"         // C++ image library

using namespace cimg_library;
using namespace std;

#define SIGMA 1.5
#define THRESHOLD 8.0

void gauss_filter(CImg<float>& filter, float sigma=1.0f, int deriv=0) {
	float width = 3*sigma;               // may be less width?
	float sigma2 = sigma*sigma;
	filter.assign(int(2*width)+1);

	int i=0;
	for (float x=-width; x<=width; x+=1.0f) {
		float g = exp(-0.5*x*x/sigma2) / sqrt(2*cimg::PI) / sigma;
		if (deriv==1) g *= -x/sigma2;
		if (deriv==2) g *= (x*x/sigma2 - 1.0f)/sigma2;
		filter[i] = g ;
		//printf ("i=%f -> %f\n", x, filter[i]);
		i++;
	}
}

void CannyDiscrete(CImg<float> in, CImg<float> &outSmooth, CImg<float> &outGradient, CImg<float> &outOrientation, CImg<float> &outThreshold, CImg<float> &outNMS) {
	const int nx = in._width;
	const int ny = in._height;

	/************ initialize memory ************/
	outGradient = in;
	outGradient.fill(0.0f);
	CImg<int> dirmax(outGradient);
	CImg<float> derivative[4];
	for(int i = 0; i < 4; i++) { derivative[i] = outGradient; }
	outOrientation = outGradient;
	outThreshold = outGradient;
	outNMS = outGradient;

	/************** smoothing the input image ******************/
	CImg<float> filter;
	gauss_filter(filter, SIGMA, 0);
	outSmooth = in.get_convolve(filter).convolve(filter.get_transpose());

	/************ loop over all pixels in the interior **********************/
	float fct = 1.0 / (2.0*sqrt(2.0f));
	for (int y = 1; y < ny-1; y++) {
		for(int x = 1; x < nx-1; x++) {
			//***** compute directional derivatives (E,NE,N,SE) ****//
			float grad_E = (outSmooth(x+1,y) - outSmooth(x-1,y))*0.5; // E
			float grad_NE = (outSmooth(x+1,y-1) - outSmooth(x-1,y+1))*fct; // NE
			float grad_N = (outSmooth(x, y-1) - outSmooth(x,  y+1))*0.5; // N
			float grad_SE = (outSmooth(x+1,y+1) - outSmooth(x-1,y-1))*fct; // SE

			//***** compute gradient magnitude *********//
			float grad_mag = grad_E*grad_E + grad_N*grad_N;
			outGradient(x,y) = grad_mag;

			//***** compute gradient orientation (continuous version)*******//
			float angle = 0.0f;
			if (grad_mag > 0.0f) { angle =  atan2(grad_N, grad_E); }
			if (angle < 0.0) angle += cimg::PI;
			outOrientation(x,y) = angle*255.0/cimg::PI + 0.5; // -> outOrientation

			//***** compute absolute derivations *******//
			derivative[0](x,y) = grad_E = fabs(grad_E);
			derivative[1](x,y) = grad_NE = fabs(grad_NE);
			derivative[2](x,y) = grad_N = fabs(grad_N);
			derivative[3](x,y) = grad_SE = fabs(grad_SE);

			//***** compute direction of max derivative //
			if ((grad_E>grad_NE) && (grad_E>grad_N) && (grad_E>grad_SE)) {
				dirmax(x,y) = 0; // E
			} else if ((grad_NE>grad_N) && (grad_NE>grad_SE)){
				dirmax(x,y) = 1; // NE
			} else if (grad_N>grad_SE) {
				dirmax(x,y) = 2; // N
			} else {
				dirmax(x,y) = 3; // SE
			}
			// one may compute the contiuous dominant direction computation...
			//outOrientation(x,y) = dirmax(x,y)*255.f/4;  
		}
	} // for x,y

	// directing vectors (E, NE, N, SE)
	int dir_vector[4][2] = {{1,0}, {1,-1}, {0,-1}, {1,1}};	
	// direction of max derivative of
	// current pixel and its two neighbouring pixel (in direction of dir)
	int dir, dir1, dir2; 

	//***** thresholding and (canny) non-max-supression *//
	for (int y = 2; y < ny-2; y++) {
		for (int x = 2; x < nx-2; x++) {
			dir = dirmax(x,y);
			if (derivative[dir](x,y) < THRESHOLD) {
				outThreshold(x,y) = 0.0f;
				outNMS(x,y) = 0.0f;
			} else {
				outThreshold(x,y) = 255.0f;
				int dx = dir_vector[dir][0];
				int dy = dir_vector[dir][1];
				dir1 = dirmax(x + dx,y + dy);
				dir2 = dirmax(x - dx,y - dy);
				outNMS(x,y) = 255.f*
					((derivative[dir](x,y) > derivative[dir1](x + dx, y + dy)) && 
					(derivative[dir](x,y) >= derivative[dir2](x-dx,y-dy)));
			} // -> outThreshold, outNMS
		}
	} // for x, y...
}

CImg<float> doCanny(string infile, string outfileGradient, string outfileNMS) {
	CImg<float> inColor(infile.c_str());
	CImg<float> in = inColor.get_RGBtoGray(); // ensure greyscale img!
	const int widthIn = in._width;
	const int heightIn = in._height;

	CImg<float> outS, outG, outO, outT, outNMS;
	CannyDiscrete(in, outS, outG, outO, outT, outNMS);
	outNMS.display("non-maximum suppression");
	outNMS.save(outfileNMS.c_str());

	return outNMS;
}

#endif