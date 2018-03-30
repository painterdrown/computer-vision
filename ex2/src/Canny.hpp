//
//  Canny.hpp
//  MyCanny
//
//  Created by painterdrown on 29/03/2018.
//  Copyright © 2018 painterdrown. All rights reserved.
//

#ifndef CANNY
#define CANNY

#include <string>
#include <vector>
#include "CImg.h"

using namespace std;
using namespace cimg_library;

typedef CImg<unsigned char> Img;
typedef CImg<float> Imgf;
typedef vector< vector<double> > Filter;

class Canny {
private:
    Img original;                // original image
    Img gray_scaled;             // grayscaled image
    Img gaussian_filtered;       // Gaussian filtered image
    Imgf angled;                 // angled image
    Img sobel_filtered;          // Sobel filtered image
    Img non_maximum_suppressed;  // non-maximum suppressed image
    Img hysteresis_threshold;    // hysteresis threshold image
public:
    Canny(const char *);                    // constructor
    Filter createFilter(int, int, double);  // creates a gaussian filter
    void grayScale();                       // do gray scale
    void gaussianFilter();                  // Gaussian filtering
    void sobelFilter();                     // Sobel filtering
    void nonMaximumSuppression();           // non-maximum suppression
    void hysteresisThreshold(int, int);     // hysteresis threshold
};

#endif
