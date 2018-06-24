#include <assert.h>
#include <math.h>
#include <FL/Fl.H>
#include <FL/Fl_Image.H>
#include "Features.h"
#include "../ImageLib/include/FileIO.h"

#define PI 3.14159265358979323846

// Compute features of an image.
bool computeFeatures(CFloatImage &image, FeatureSet &features, int featureType) {
	// TODO: Instead of calling dummyComputeFeatures, write your own
	// feature computation routines and call them here.
	switch (featureType) {
	case 1:
		dummyComputeFeatures(image, features);
		break;
	case 2:
		ComputeHarrisFeatures(image, features);
		break;
	default:
		return false;
	}

	// This is just to make sure the IDs are assigned in order, because
	// the ID gets used to index into the feature array.
	for (unsigned int i=0; i<features.size(); i++) {
		features[i].id = i+1;
	}

	return true;
}

// Perform a query on the database.  This simply runs matchFeatures on
// each image in the database, and returns the feature set of the best
// matching image.
bool performQuery(const FeatureSet &f, const ImageDatabase &db, int &bestIndex, vector<FeatureMatch> &bestMatches, double &bestScore, int matchType) {
	// Here's a nice low number.
	bestScore = -1e100;

	vector<FeatureMatch> tempMatches;
	double tempScore;

	for (unsigned int i=0; i<db.size(); i++) {
		if (!matchFeatures(f, db[i].features, tempMatches, tempScore, matchType)) {
			return false;
		}

		if (tempScore > bestScore) {
			bestIndex = i;
			bestScore = tempScore;
			bestMatches = tempMatches;
		}
	}

	return true;
}

// Match one feature set with another.
bool matchFeatures(const FeatureSet &f1, const FeatureSet &f2, vector<FeatureMatch> &matches, double &totalScore, int matchType) {
	// TODO: We have given you the ssd matching function, you must write your own
	// feature matching function for the ratio test.
	
	printf("\nMatching features.......\n");

	switch (matchType) {
	case 1:
		ssdMatchFeatures(f1, f2, matches, totalScore);
		return true;
	case 2:
		ratioMatchFeatures(f1, f2, matches, totalScore);
		return true;
	default:
		return false;
	}
}

// Evaluate a match using a ground truth homography.  This computes the
// average SSD distance between the matched feature points and
// the actual transformed positions.
double evaluateMatch(const FeatureSet &f1, const FeatureSet &f2, const vector<FeatureMatch> &matches, double h[9]) {
	double d = 0;
	int n = 0;

	double xNew;
	double yNew;

    unsigned int num_matches = matches.size();
	for (unsigned int i=0; i<num_matches; i++) {
		int id1 = matches[i].id1;
        int id2 = matches[i].id2;
        applyHomography(f1[id1-1].x, f1[id1-1].y, xNew, yNew, h);
		d += sqrt(pow(xNew-f2[id2-1].x,2)+pow(yNew-f2[id2-1].y,2));
		n++;
	}	

	return d / n;
}

void addRocData(const FeatureSet &f1, const FeatureSet &f2, const vector<FeatureMatch> &matches, double h[9],vector<bool> &isMatch,double threshold,double &maxD) {
	double d = 0;

	double xNew;
	double yNew;

    unsigned int num_matches = matches.size();
	for (unsigned int i=0; i<num_matches; i++) {
		int id1 = matches[i].id1;
        int id2 = matches[i].id2;
		applyHomography(f1[id1-1].x, f1[id1-1].y, xNew, yNew, h);

		// Ignore unmatched points.  There might be a better way to
		// handle this.
		d = sqrt(pow(xNew-f2[id2-1].x,2)+pow(yNew-f2[id2-1].y,2));
		if (d<=threshold)
		{
			isMatch.push_back(1);
		}
		else
		{
			isMatch.push_back(0);
		}

		if (matches[i].score>maxD)
			maxD=matches[i].score;
	}	
}

vector<ROCPoint> computeRocCurve(vector<FeatureMatch> &matches,vector<bool> &isMatch,vector<double> &thresholds)
{
	vector<ROCPoint> dataPoints;

	for (int i=0; i < (int)thresholds.size();i++)
	{
		//printf("Checking threshold: %lf.\r\n",thresholds[i]);
		int tp=0;
		int actualCorrect=0;
		int fp=0;
		int actualError=0;
		int total=0;

        int num_matches = (int) matches.size();
		for (int j=0;j < num_matches;j++)
		{
			if (isMatch[j])
			{
				actualCorrect++;
				if (matches[j].score<thresholds[i])
				{
					tp++;
				}
			}
			else
			{
				actualError++;
				if (matches[j].score<thresholds[i])
				{
					fp++;
				}
            }				
			
			total++;
		}

		ROCPoint newPoint;
		//printf("newPoints: %lf,%lf",newPoint.trueRate,newPoint.falseRate);
		newPoint.trueRate=(double(tp)/actualCorrect);
		newPoint.falseRate=(double(fp)/actualError);
		//printf("newPoints: %lf,%lf",newPoint.trueRate,newPoint.falseRate);

		dataPoints.push_back(newPoint);
	}

	return dataPoints;
}


// Compute silly example features.  This doesn't do anything
// meaningful.
void dummyComputeFeatures(CFloatImage &image, FeatureSet &features) {
	CShape sh = image.Shape();
	Feature f;

	for (int y=0; y<sh.height; y++) {
		for (int x=0; x<sh.width; x++) {
			double r = image.Pixel(x,y,0);
			double g = image.Pixel(x,y,1);
			double b = image.Pixel(x,y,2);

			if ((int)(255*(r+g+b)+0.5) % 100  == 1) {
				// If the pixel satisfies this meaningless criterion,
				// make it a feature.
				
				f.type = 1;
				f.id += 1;
				f.x = x;
				f.y = y;

				f.data.resize(1);
				f.data[0] = r + g + b;

				features.push_back(f);
			}
		}
	}
}

void ComputeHarrisFeatures(CFloatImage &image, FeatureSet &features)
{
	//Create the destination image for Gaussian filter
	CFloatImage smoothImage(image.Shape().width,image.Shape().height,1);

	//Create grayscale image used for Harris detection
	CFloatImage grayImage=ConvertToGray(image);

	//Create image to store Harris values
	CFloatImage harrisImage(image.Shape().width,image.Shape().height,1);

	//Create image to store local maximum harris values as 1, other pixels 0
	CByteImage harrisMaxImage(image.Shape().width,image.Shape().height,1);

	//This image represents the window around the feature you need to compute to store as the feature descriptor
    const int windowSize = 8;
    CFloatImage destImage(windowSize,windowSize,1);

	//TO DO--------------------------------------------------------------------
	//Apply the 7x7 Gaussian filter to smoothImage to obtain image used for warping
    Convolve(grayImage, smoothImage, ConvolveKernel_7x7);
    CByteImage tmp(smoothImage.Shape());
    convertToByteImage(smoothImage, tmp);
    //WriteFile(tmp, "smooth.tga");    

	//TO DO--------------------------------------------------------------------
	//function puts harris values at each pixel position in harrisImage and 
    // orientations at each pixel position in orientationImage
    CFloatImage orientationImage(image.Shape().width, image.Shape().height, 1);
    computeHarrisValues(grayImage, smoothImage, harrisImage, orientationImage);

	//TO DO---------------------------------------------------------------------
	//Loop through harrisValues and find the best features in a local 3x3 maximum
	//compute the feature descriptor
	computeLocalMaxima(harrisImage,harrisMaxImage);

    convertToByteImage(harrisImage, tmp);
    WriteFile(tmp, "harris.tga");
    WriteFile(harrisMaxImage, "harrisMax.tga");
	
	//TO DO--------------------------------------------------------------------
	//Loop through feature points in harrisMaxImage and create feature descriptor 
	//for each point above a threshold
    int id = 0;
    for (int y=0;y<harrisMaxImage.Shape().height;y++) {
		for (int x=0;x<harrisMaxImage.Shape().width;x++) {
		
            if (harrisMaxImage.Pixel(x, y, 0) == 0)
                continue;

			/*
		    //Compute the inverse transform as described in the feature description.
		    //You'll need to compute the transform from each pixel in the 8x8 image 
		    //to sample from the appropriate pixels in the 40x40 rotated window surrounding the feature
		    CTransform3x3 xform;
    	
		    //TO DO---------------------------------------------------------------------
		    //Fill in xform with correct transform information

            double rad = orientationImage.Pixel(x, y, 0);
            double degrees = 180.0 * rad / PI;
            CTransform3x3 trans1 = CTransform3x3::Translation(-(0.5 * windowSize - 0.5), -(0.5 * windowSize - 0.5));
            CTransform3x3 trans = CTransform3x3::Translation((float) x, (float) y);
            CTransform3x3 rotate = CTransform3x3::Rotation((float) degrees);

            CTransform3x3 scale;
            scale[0][0] = 5.0;
            scale[1][1] = 5.0;

            xform = trans * scale * rotate * trans1; //  * scale * rotate; //  * trans1;

		    //Call the Warp Global function to do the mapping
		    WarpGlobal(smoothImage,destImage,xform,EWarpInterpolationMode::eWarpInterpLinear);

#if 0
            char buf[256];
            sprintf(buf, "f%03d.tga", id);
            CByteImage featureImage(windowSize,windowSize,1);
            convertToByteImage(destImage, featureImage);
            WriteFile(featureImage, buf);
#endif


		    //TO DO---------------------------------------------------------------------
		    //Fill in feature with descriptor data here
    		Feature f;
            f.id = id;
            f.type = 2;
            f.x = x;
            f.y = y;
            f.angleRadians = orientationImage.Pixel(x, y, 0);
            f.data.resize(windowSize * windowSize);

            int count = 0;
            for (int yi = 0; yi < windowSize; yi++) {
                for (int xi = 0; xi < windowSize; xi++) {
                    f.data[count] = destImage.Pixel(xi, yi, 0);
                    count++;
                }
            }
			

#if 0
            double mean = 0.0;
            for (int i = 0; i < 64; i++) 
                mean += f.data[i];
            mean /= 64;

            double var = 0.0;
            for (int i = 0; i < 64; i++)
                var += (mean - f.data[i]) * (mean - f.data[i]);
            var = sqrt(var / 64);

            for (int i = 0; i < 64; i++)
                f.data[i] = (f.data[i] - mean) / var;
#endif
			*/
			
			//5x5
			
			Feature f;
            f.id = id;
            f.type = 2;
            f.x = x;
            f.y = y;
            //f.angleRadians = orientationImage.Pixel(x, y, 0);
            f.data.resize(5 * 5);

            int count = 0;
            for (int yi = -2; yi <=2; yi++) {
                for (int xi = -2; xi <= 2; xi++) {
                    //f.data[count] = grayImage.Pixel(xi, yi, 0);
					if (((x+xi)>=0) && ((x+xi)<grayImage.Shape().width) && ((y+yi)>=0) && ((y+yi)<grayImage.Shape().height))
						f.data[count] = grayImage.Pixel(x+xi, y+yi, 0);
					else
						f.data[count] = 0;
                    count++;
                }
            }
			
			
            features.push_back(f);
            id++;
		}
	}
}



//TO DO---------------------------------------------------------------------
//Loop through the image to compute the harris corner values as described in class
// srcImage:  grayscale of original image
// harrisImage:  populate the harris values per pixel in this image
void computeHarrisValues(CFloatImage &srcImage, CFloatImage &smoothImage, CFloatImage &harrisImage, CFloatImage &orientationImage)
{
    CFloatImage sobelX(3, 3, 1);

    // compute the gradient image
    CFloatImage xGradient(srcImage.Shape());
    Convolve(srcImage, xGradient, ConvolveKernel_SobelX);
    CFloatImage yGradient(srcImage.Shape());
    Convolve(srcImage, yGradient, ConvolveKernel_SobelY);

    CFloatImage xGradientSmooth(srcImage.Shape());
    Convolve(smoothImage, xGradientSmooth, ConvolveKernel_SobelX);
    CFloatImage yGradientSmooth(srcImage.Shape());
    Convolve(smoothImage, yGradientSmooth, ConvolveKernel_SobelY);

    CByteImage tmp(srcImage.Shape());
    convertToByteImage(xGradient, tmp);
    // WriteFile(tmp, "xgrad.tga");
    convertToByteImage(yGradient, tmp);
    // WriteFile(tmp, "ygrad.tga");

	int w = srcImage.Shape().width;
    int h = srcImage.Shape().height;

    int knlW = 5;
    int knlRad = 2;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            /* Compute the harris matrix at this pixel */
            double H[4] = { 0.0, 0.0, 0.0, 0.0 };
            double wsum = 0.0;

            for (int i = -knlRad; i <= knlRad; i++) {
                for (int j = -knlRad; j <= knlRad; j++) {
                    int yi = y+i, xj = x+j;

                    if (!srcImage.Shape().InBounds(xj, yi))
                        continue;

                    double w = gaussian5x5[(i+knlRad) * knlW + (j+knlRad)];
                    double gx = xGradient.Pixel(xj, yi, 0);
                    double gy = yGradient.Pixel(xj, yi, 0);
                    
                    H[0] += w * gx * gx;
                    H[1] += w * gx * gy;
                    H[2] += w * gx * gy;
                    H[3] += w * gy * gy;
                
                    wsum += w;
                }
            }

            double det = H[0] * H[3] - H[1] * H[2];
            double tr = H[0] + H[3];

            double score;
            if (tr == 0.0)
                score = 0.0;
            else 
                score = det / tr;

            harrisImage.Pixel(x,y,0) = (float) score;        
        
            /* Compute the gradient direction */
#if 1
            double gx = xGradientSmooth.Pixel(x, y, 0);
            double gy = yGradientSmooth.Pixel(x, y, 0);
        
            double orientation = atan2(gy, gx);
            orientationImage.Pixel(x, y, 0) = (float) orientation;
#else
            /* Compute the eigenvectors */
            double eval1, eval2;
            double discr = sqrt(4.0 * H[1] * H[2] + (H[0] - H[3]) * (H[0] - H[3]));
            eval1 = 0.5 * (H[0] + H[3] + discr);
            eval2 = 0.5 * (H[0] + H[3] - discr);

            double evec1[2] = { -H[1], H[0] - eval1 };
            double evec2[2] = { -H[1], H[0] - eval2 };

            /* Normalize */
            double mag1 = sqrt(evec1[0] * evec1[0] + evec1[1] * evec1[1]); 
            double mag2 = sqrt(evec2[0] * evec2[0] + evec2[1] * evec2[1]);

            evec1[0] /= mag1;
            evec1[1] /= mag1;

            evec2[0] /= mag2;
            evec2[1] /= mag2;

            double x1 = H[0] * evec1[0] + H[1] * evec1[1];
            double y1 = H[2] * evec1[0] + H[3] * evec1[1];
            double x2 = H[0] * evec2[0] + H[1] * evec2[1];
            double y2 = H[2] * evec2[0] + H[3] * evec2[1];

#if 0
            if (score > 0.5) {
                printf("scale1: %0.3f, %0.3f; %0.3f\n", x1 / evec1[0], y1 / evec1[1], eval1);
                printf("scale2: %0.3f, %0.3f; %0.3f\n", x2 / evec2[0], y2 / evec2[1], eval2);
            }
#endif

            double hi[2], lo[2];
            if (eval1 > eval2) {
                hi[0] = evec1[0];
                hi[1] = evec1[1];
                lo[0] = evec2[0];
                lo[1] = evec2[1];
            } else {
                hi[0] = evec2[0];
                hi[1] = evec2[1];
                lo[0] = evec1[0];
                lo[1] = evec1[1];
            }

#if 1
            double gx = xGradient.Pixel(x, y, 0);
            double gy = yGradient.Pixel(x, y, 0);

            /* Compute the direction closest to the gradient direction */
            double dot = hi[0] * gx + hi[1] * gy;
            if (dot < 0) {
                hi[0] = -hi[0];
                hi[1] = -hi[1];
            }
#endif

            double orientation = atan2(hi[1], hi[0]);
            orientationImage.Pixel(x, y, 0) = (float) orientation;
#endif
        }
    }
}



//TO DO---------------------------------------------------------------------
//Loop through the image to compute the harris corner values as described in class
// srcImage:  image with Harris values
// destImage: Assign 1 to local maximum in 3x3 window, 0 otherwise
void computeLocalMaxima(CFloatImage &srcImage,CByteImage &destImage)
{
	int w = srcImage.Shape().width;
    int h = srcImage.Shape().height;

    const int rad = 3;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            double score = srcImage.Pixel(x, y, 0);
            if (score < 1.0e-2) {
                destImage.Pixel(x, y, 0) = 0;
                continue;
            }

            bool maximum = true;

            for (int dy = -rad; dy < rad; dy++) {
                for (int dx = -rad; dx < rad; dx++) {
                    if (dx == 0 && dy == 0) {
                        continue;
                    }

                    int yi = y + dy, xi = x + dx;

                    if (!srcImage.Shape().InBounds(xi, yi))
                        continue;

                    if (score <= srcImage.Pixel(xi, yi, 0)) {
                        maximum = false;
                        break;
                    }
                }
            }

            if (maximum)
                destImage.Pixel(x, y, 0) = 255;
            else
                destImage.Pixel(x, y, 0) = 0;
        }
    }
}


// Perform simple feature matching.  This just uses the SSD
// distance between two feature vectors, and matches a feature in the
// first image with the closest feature in the second image.  It can
// match multiple features in the first image to the same feature in
// the second image.
void ssdMatchFeatures(const FeatureSet &f1, const FeatureSet &f2, vector<FeatureMatch> &matches, double &totalScore) {
	int m = f1.size();
	int n = f2.size();

	matches.resize(m);
	totalScore = 0;

	double d;
	double dBest;
	int idBest;

	for (int i=0; i<m; i++) {
		dBest = 1e100;
		idBest = 0;

		for (int j=0; j<n; j++) {
			d = distanceSSD(f1[i].data, f2[j].data);

			if (d < dBest) {
				dBest = d;
				idBest = f2[j].id;
			}
		}

    matches[i].id1 = f1[i].id;
		matches[i].id2 = idBest;
		matches[i].score = dBest;
		totalScore += matches[i].score;
	}
}

// TODO: Write this function to perform ratio feature matching.  
// This just uses the ratio of the SSD distance of the two best matches
// and matches a feature in the first image with the closest feature in the second image.
// It can match multiple features in the first image to the same feature in
// the second image.  (See class notes for more information)
void ratioMatchFeatures(const FeatureSet &f1, const FeatureSet &f2, vector<FeatureMatch> &matches, double &totalScore) 
{
  int m = f1.size();
	int n = f2.size();

	matches.resize(m);
	totalScore = 0;

	double d;
	double dBest, dSecondBest;
	int idBest;

	for (int i = 0; i < m; i++) {
		dBest = 1e100;
		idBest = 0;

		for (int j = 0; j < n; j++)
		{
			d = distanceSSD(f1[i].data, f2[j].data);

			if (d < dBest)
			{
				dSecondBest = dBest;
				dBest = d;
				idBest = f2[j].id;
			}
		}

		matches[i].id1 = f1[i].id;
		matches[i].id2 = idBest;
		matches[i].score = dBest / dSecondBest;
		totalScore += matches[i].score;
	}
}


// Convert Fl_Image to CFloatImage.
bool convertImage(const Fl_Image *image, CFloatImage &convertedImage) {
	if (image == NULL) {
		return false;
	}

	// Let's not handle indexed color images.
	if (image->count() != 1) {
		return false;
	}

	int w = image->w();
	int h = image->h();
	int d = image->d();

	// Get the image data.
	const char *const *data = image->data();

	int index = 0;

	for (int y=0; y<h; y++) {
		for (int x=0; x<w; x++) {
			if (d < 3) {
				// If there are fewer than 3 channels, just use the
				// first one for all colors.
				convertedImage.Pixel(x,y,0) = ((uchar) data[0][index]) / 255.0f;
				convertedImage.Pixel(x,y,1) = ((uchar) data[0][index]) / 255.0f;
				convertedImage.Pixel(x,y,2) = ((uchar) data[0][index]) / 255.0f;
			}
			else {
				// Otherwise, use the first 3.
				convertedImage.Pixel(x,y,0) = ((uchar) data[0][index]) / 255.0f;
				convertedImage.Pixel(x,y,1) = ((uchar) data[0][index+1]) / 255.0f;
				convertedImage.Pixel(x,y,2) = ((uchar) data[0][index+2]) / 255.0f;
			}

			index += d;
		}
	}
	
	return true;
}

// Convert CFloatImage to CByteImage.
void convertToByteImage(CFloatImage &floatImage, CByteImage &byteImage) {
	CShape sh = floatImage.Shape();

    assert(floatImage.Shape().nBands == byteImage.Shape().nBands);
	for (int y=0; y<sh.height; y++) {
		for (int x=0; x<sh.width; x++) {
			for (int c=0; c<sh.nBands; c++) {
				float value = floor(255*floatImage.Pixel(x,y,c) + 0.5f);

				if (value < byteImage.MinVal()) {
					value = byteImage.MinVal();
				}
				else if (value > byteImage.MaxVal()) {
					value = byteImage.MaxVal();
				}

				// We have to flip the image and reverse the color
				// channels to get it to come out right.  How silly!
				byteImage.Pixel(x,sh.height-y-1,sh.nBands-c-1) = (uchar) value;
			}
		}
	}
}

// Compute SSD distance between two vectors.
double distanceSSD(const vector<double> &v1, const vector<double> &v2) {
	int m = v1.size();
	int n = v2.size();

	if (m != n) {
		// Here's a big number.
		return 1e100;
	}

	double dist = 0;
	
	for (int i=0; i<m; i++) {
		dist += pow(v1[i]-v2[i], 2);
	}

	
	return sqrt(dist);
}

// Transform point by homography.
void applyHomography(double x, double y, double &xNew, double &yNew, double h[9]) {
	double d = h[6]*x + h[7]*y + h[8];

	xNew = (h[0]*x + h[1]*y + h[2]) / d;
	yNew = (h[3]*x + h[4]*y + h[5]) / d;
}

// Compute AUC given a ROC curve
double computeAUC(vector<ROCPoint> &results)
{
	double auc=0;
	double xdiff,ydiff;
	for (int i = 1; i < (int) results.size(); i++)
    {
        //fprintf(stream,"%lf\t%lf\t%lf\n",thresholdList[i],results[i].falseRate,results[i].trueRate);
		xdiff=(results[i].falseRate-results[i-1].falseRate);
		ydiff=(results[i].trueRate-results[i-1].trueRate);
		auc=auc+xdiff*results[i-1].trueRate+xdiff*ydiff/2;
    	    
    }
	return auc;
}