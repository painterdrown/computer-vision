//
//  Canny.cpp
//  MyCanny
//
//  Created by painterdrown on 29/03/2018.
//  Copyright © 2018 painterdrown. All rights reserved.
//

#include <stdio.h>
#include <math.h>
#include "Canny.hpp"

Canny::Canny(const char *filepath) {
    original.load(filepath);
    original.display("Original");
    
    grayScale();
    gray_scaled.display("Gray Scaled");
    
    gaussianFilter();
    gaussian_filtered.display("Gaussian Filtered");
    
    gradientFilter();
    // angled.display();
    sobel_filtered.display("Sobel Filtered");
    
    nonMaximumSuppression();
    non_maximum_suppressed.display("Non-maximum Suppressed");
    
    hysteresisThreshold(20, 60);
    hysteresis_threshold.display("Hysteresis Threshold");
}

Filter Canny::createGaussianFilter(int row, int column, double sigma) {
    Filter filter;
    
    // initialize
    for (int i = 0; i < row; ++i) {
        vector<double> col;
        for (int j = 0; j < column; j++)
        {
            col.push_back(-1);
        }
        filter.push_back(col);
    }
    
    float coord_sum = 0;
    float constant = 2.0 * sigma * sigma;
    float sum = 0;
    for (int x = -row/2; x <= row/2; ++x) {
        for (int y = -column/2; y <= column/2; ++y) {
            coord_sum = (x * x + y * y);
            filter[x + row/2][y + column/2] = (exp(-(coord_sum) / constant)) / (M_PI * constant);
            sum += filter[x + row/2][y + column/2];
        }
    }
    
    // normalize the Filter
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < column; ++j) {
            filter[i][j] /= sum;
        }
    }
    
    return filter;
}

void Canny::grayScale() {
    gray_scaled = Img(original.width(), original.height(), 1);
    cimg_forXY(original, x, y) {
        unsigned char r = original.atXY(x, y, 0, 0);
        unsigned char g = original.atXY(x, y, 0, 1);
        unsigned char b = original.atXY(x, y, 0, 2);
        double gray = (r * 0.2126 + g * 0.7152 + b * 0.0722);
        gray_scaled.atXY(x, y) = gray;
    }
}

void Canny::gaussianFilter() {
    Filter filter = createGaussianFilter(3, 3, 1);
    int size = (int)filter.size() / 2;
    gaussian_filtered = Img(gray_scaled.width() - 2*size, gray_scaled.height() - 2*size, 1);
    for (int i = size; i < gray_scaled.height() - size; ++i) {
        for (int j = size; j < gray_scaled.width() - size; ++j) {
            double sum = 0;
            for (int x = 0; x < filter.size(); ++x) {
                for (int y = 0; y < filter.size(); ++y) {
                    sum += filter[x][y] * (double)(gray_scaled.atXY(j + y - size, i + x - size));
                }
            }
            gaussian_filtered.atXY(j - size, i - size) = sum;
        }
    }
}

Filter Canny::createSobelFilterX() {
    // Sobel x filter
    double sobel_x1[] = {-1.0, 0, 1.0};
    double sobel_x2[] = {-2.0, 0, 2.0};
    double sobel_x3[] = {-1.0, 0, 1.0};
    Filter x_filter(3);
    x_filter[0].assign(sobel_x1, sobel_x1 + 3);
    x_filter[1].assign(sobel_x2, sobel_x2 + 3);
    x_filter[2].assign(sobel_x3, sobel_x3 + 3);
    return x_filter;
}

Filter Canny::createSobelFilterY() {
    // Sobel y filter
    double sobel_y1[] = {1.0, 2.0, 1.0};
    double sobel_y2[] = {0, 0, 0};
    double sobel_y3[] = {-1.0, -2.0, -1.0};
    Filter y_filter(3);
    y_filter[0].assign(sobel_y1, sobel_y1 + 3);
    y_filter[1].assign(sobel_y2, sobel_y2 + 3);
    y_filter[2].assign(sobel_y3, sobel_y3 + 3);
    return y_filter;
}

Filter Canny::createPrewittFilterX() {
    double prewitt_x1[] = {-1.0, 0, 1.0};
    double prewitt_x2[] = {-1.0, 0, 1.0};
    double prewitt_x3[] = {-1.0, 0, 1.0};
    Filter x_filter(3);
    x_filter[0].assign(prewitt_x1, prewitt_x1 + 3);
    x_filter[1].assign(prewitt_x2, prewitt_x2 + 3);
    x_filter[2].assign(prewitt_x3, prewitt_x3 + 3);
    return x_filter;
}

Filter Canny::createPrewittFilterY() {
    double prewitt_y1[] = {1.0, 1.0, 1.0};
    double prewitt_y2[] = {0, 0, 0};
    double prewitt_y3[] = {-1.0, -1.0, -1.0};
    Filter y_filter(3);
    y_filter[0].assign(prewitt_y1, prewitt_y1 + 3);
    y_filter[1].assign(prewitt_y2, prewitt_y2 + 3);
    y_filter[2].assign(prewitt_y3, prewitt_y3 + 3);
    return y_filter;
}

void Canny::gradientFilter() {

    // Sobel filter
    Filter x_filter = createSobelFilterX();
    Filter y_filter = createSobelFilterY();

    // Prewitt filter
    // Filter x_filter = createPrewittFilterX();
    // Filter y_filter = createPrewittFilterY();
    
    int size = (int)x_filter.size()/2;  // limit size
    const int width = gaussian_filtered.width() - 2*size;
    const int height = gaussian_filtered.height() - 2*size;
    sobel_filtered = Img(width, height, 1);
    angled = Imgf(width, height, 1);

    for (int i = size; i < gaussian_filtered.height() - size; ++i) {
        for (int j = size; j < gaussian_filtered.width() - size; ++j) {
            double sumx = 0;
            double sumy = 0;
            for (int x = 0; x < x_filter.size(); ++x) {
                for (int y = 0; y < y_filter.size(); ++y) {
                    sumx += x_filter[x][y] * (double)(gaussian_filtered.atXY(j + y - size, i + x - size)); // Sobel x filter value
                    sumy += y_filter[x][y] * (double)(gaussian_filtered.atXY(j + y - size, i + x - size)); // Sobel y filter value
                }
            }
            double sumxsq = sumx * sumx;
            double sumysq = sumy * sumy;
            double sq2 = sqrt(sumxsq + sumysq);
            if (sq2 > 255) sq2 = 255;  // unsigned char fix
            sobel_filtered.atXY(j - size, i - size) = sq2;
            if (sumx == 0)  // arctan fix
                angled.atXY(j - size, i - size) = 90;
            else
                angled.atXY(j - size, i - size) = atan(sumy / sumx);
        }
    }
}

void Canny::nonMaximumSuppression() {
    non_maximum_suppressed = Img(sobel_filtered.width()-2, sobel_filtered.height()-2, 1);
    for (int i = 1; i < sobel_filtered.height()-1; ++i) {
        for (int j = 1; j < sobel_filtered.width()-1; ++j) {
            float tangent = angled.atXY(j, i);
            non_maximum_suppressed.atXY(j-1, i-1) = sobel_filtered.atXY(j, i);
            // horizontal edge
            if (((-22.5 < tangent) && (tangent <= 22.5)) || ((157.5 < tangent) && (tangent <= -157.5))) {
                if ((sobel_filtered.atXY(j, i) < sobel_filtered.atXY(j+1, i)) || (sobel_filtered.atXY(j, i) < sobel_filtered.atXY(j-1, i)))
                    non_maximum_suppressed.atXY(j-1, i-1) = 0;
            }
            // vertical edge
            if (((-112.5 < tangent) && (tangent <= -67.5)) || ((67.5 < tangent) && (tangent <= 112.5))) {
                if ((sobel_filtered.atXY(j, i) < sobel_filtered.atXY(j, i+1)) || (sobel_filtered.atXY(j, i) < sobel_filtered.atXY(j, i-1)))
                    non_maximum_suppressed.atXY(j-1, i-1) = 0;
            }
            // -45 degree edge
            if (((-67.5 < tangent) && (tangent <= -22.5)) || ((112.5 < tangent) && (tangent <= 157.5))) {
                if ((sobel_filtered.atXY(j, i) < sobel_filtered.atXY(j+1, i-1)) || (sobel_filtered.atXY(j, i) < sobel_filtered.atXY(j-1, i+1)))
                    non_maximum_suppressed.atXY(j-1, i-1) = 0;
            }
            // 45 degree edge
            if (((-157.5 < tangent) && (tangent <= -112.5)) || ((22.5 < tangent) && (tangent <= 67.5))) {
                if ((sobel_filtered.atXY(j, i) < sobel_filtered.atXY(j+1, i+1)) || (sobel_filtered.atXY(j, i) < sobel_filtered.atXY(j-1, i-1)))
                    non_maximum_suppressed.atXY(j-1, i-1) = 0;
            }
        }
    }
}

void Canny::hysteresisThreshold(int low, int high) {
    if(low > 255) low = 255;
    if(high > 255) high = 255;
    hysteresis_threshold = Img(non_maximum_suppressed);
    for (int i = 0; i < non_maximum_suppressed.height(); ++i) {
        for (int j = 0; j < non_maximum_suppressed.width(); ++j) {
            if (hysteresis_threshold.atXY(j, i) > high)
                hysteresis_threshold.atXY(j, i) = 255;
            else if(hysteresis_threshold.atXY(j, i) < low)
                hysteresis_threshold.atXY(j, i) = 0;
            else {
                bool anyHigh = false;
                bool anyBetween = false;
                for (int x = i-1; x < i+2; ++x) {
                    for (int y = j-1; y < j+2; ++y) {
                        if(x <= 0 || y <= 0 || x > hysteresis_threshold.height() || y > hysteresis_threshold.width())  // out of bounds
                            continue;
                        else {
                            if(hysteresis_threshold.atXY(y, x) > high) {
                                hysteresis_threshold.atXY(j, i) = 255;
                                anyHigh = true;
                                break;
                            } else if (hysteresis_threshold.atXY(y, x) <= high && hysteresis_threshold.atXY(y, x) >= low)
                                anyBetween = true;
                        }
                    }
                    if (anyHigh) break;
                }
                if (!anyHigh && anyBetween) {
                    for (int x = i-2; x < i+3; ++x) {
                        for (int y = j-1; y < j+3; ++y) {
                            if (x < 0 || y < 0 || x > hysteresis_threshold.height() || y > hysteresis_threshold.width()) // out of bounds
                                continue;
                            else {
                                if (hysteresis_threshold.atXY(y, x) > high) {
                                    hysteresis_threshold.atXY(j, i) = 255;
                                    anyHigh = true;
                                    break;
                                }
                            }
                        }
                        if (anyHigh) break;
                    }
                }
                if (!anyHigh)
                    hysteresis_threshold.atXY(j, i) = 0;
            }
        }
    }
}
