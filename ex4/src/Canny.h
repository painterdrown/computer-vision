#ifndef CANNY_H
#define CANNY_H

#include <cmath>
#include <string>
#include <vector>
#include "CImg.h"         // C++ image library

using namespace cimg_library;
using namespace std;

/** a vector of pixel coordinates.
  * Usage:
  *    unsigned i;
  *    int x, y;
  *    TVectorOfPairs nonmax;
  *    nonmax.push_back (make_pair(x,y));   // adding new pixel coordinates:
  *    x = nonmax[i].first;                 // get x-coordinate of i-th pixel
  *    y = nonmax[i].second;                // get y-coordinate of i-th pixel
  */
typedef std::vector<std::pair<int,int> > TVectorOfPairs;

/** compute Gaussian derivatives filter weights
  * \param sigma = bandwidth of the Gaussian 
  * \param deriv = computing the 'deriv'-th derivatives of a Gaussian
  * the width of the filter is automatically determined from sigma.
  * g  = \frac{1}{\sqrt{2\pi}\sigma}   \exp(-0.5 \frac{x^2}{\sigma^2} )
  * g' = \frac{x}{\sqrt{2\pi}\sigma^3} \exp(-0.5 \frac{x^2}{\sigma^2} )
  *    = -\frac{x}{\sigma^2} g
  * g''= (\frac{x^2}{\sigma^2} - 1) \frac{1}{\sigma^2} g
  */
void gauss_filter (CImg<float>& filter, float sigma=1.0f, int deriv=0) {
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

/** apply non-maximum suppression
	* \param input: some float image
	* \param nonmax: a list of (x,y)-tuple of maxima
	* \param thresh: ignore those with too small response
	* \param halfwidth: halfwidth of the neighbourhood size
	*/
void non_maximum_suppression (CImg<float>& img, TVectorOfPairs& nonmax, float thresh, int halfwidth) {
	nonmax.clear();
	for (int y=halfwidth; y<img._height-halfwidth; y++) {
		for (int x=halfwidth; x<img._width-halfwidth; x++) {
			float value = img(x,y);
			if (value<thresh) { continue; }

			bool ismax = true;
			for (int ny=y-halfwidth; ny<=y+halfwidth; ny++) {
				for (int nx=x-halfwidth; nx<=x+halfwidth; nx++) {
					ismax = ismax && (img(nx,ny)<=value);
				}}
			if (!ismax) continue;

			nonmax.push_back (make_pair(x,y));
		}}
}

/** CannyDiscrete.
  * 
  * Version with discrete implementation of the non-maximum-suppression.
  * 
  * During the nms, the gradient and its orientation are treated discrete,
  * e.g., only four directions are considered: E, NE, N, SE.
  */
void CannyDiscrete(CImg<float> in, float sigma, float threshold, CImg<float> &outSmooth, CImg<float> &outGradient, CImg<float> &outOrientation, CImg<float> &outThreshold, CImg<float> &outNMS) {
	const int nx = in._width;
	const int ny = in._height;

	/************ initialize memory ************/
	outGradient = in; outGradient.fill(0.0f);
	CImg<int> dirmax(outGradient);
	CImg<float> derivative[4];
	for(int i = 0; i < 4; i++) { derivative[i] = outGradient; }
	outOrientation = outGradient;
	outThreshold = outGradient;
	outNMS = outGradient;

	/************** smoothing the input image ******************/
	CImg<float> filter;
	gauss_filter(filter, sigma, 0);
	outSmooth = in.get_convolve(filter).convolve(filter.get_transpose());

	/************ loop over all pixels in the interior **********************/
	float fct = 1.0 / (2.0*sqrt(2.0f));
	for (int y = 1; y < ny-1; y++) {
		for(int x = 1; x < nx-1; x++) {
			//***** compute directional derivatives (E,NE,N,SE) ****//
			float grad_E = (outSmooth(x+1,y  ) - outSmooth(x-1,y  ))*0.5; // E
			float grad_NE = (outSmooth(x+1,y-1) - outSmooth(x-1,y+1))*fct; // NE
			float grad_N = (outSmooth(x,  y-1) - outSmooth(x,  y+1))*0.5; // N
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
			if (derivative[dir](x,y) < threshold) {
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

void doCanny(string infile, string outfileGradient, string outfileNMS) {
	// canny parameters
	float sigma = 1.5f;
	float threshold = 6.0f;

	//***** read image *****************//
	// cimg::imagemagick_path("C:\\Program Files\\ImageMagick-7.0.7-Q16\\convert.exe");
	CImg<float> inColor(infile.c_str());
	CImg<float> in = inColor.get_RGBtoGray(); // ensure greyscale img!
	const int widthIn = in._width;
	const int heightIn = in._height;
	if ( widthIn == 0 || heightIn == 0 ) {
		printf("Error when loading input image.\n");
		return;
	}

	//***** declare output images ******//
	CImg<float> outS, outG, outO, outT, outNMS;

	//***** apply Canny filter *********//
	CannyDiscrete(in, sigma, threshold, outS, outG, outO, outT, outNMS);

	//***** display output images ******//
	char  header[100];
	sprintf(header, "gaussian smoothed image: sigma = %f", sigma);
	// outS.display(header);
	float maxgrad = 0.0f;
	cimg_forXY(outG,x,y) { maxgrad = std::max(maxgrad, outG(x,y)); }
	printf("normalising [0..%f] to [0..255]\n", maxgrad);
	sprintf(header, "gradient magnitude [0..%f]", maxgrad);
	// outG.display(header);
	// outO.display("orientation map");
	sprintf(header, "thresholded with %f", threshold);
	// outT.display(header);
	// outNMS.display("non-maximum suppression");

	//***** write output images ********//
	if (outfileGradient.length()>0) {
		printf("saving gradient to %s\n", outfileGradient.c_str());
		outG *= (255.f/maxgrad);
		outG.save(outfileGradient.c_str()); 
	}
	if (outfileNMS.length()>0) {
		printf("saving NMS to %s\n", outfileNMS.c_str());
		outNMS.save(outfileNMS.c_str());
	}
}

#endif