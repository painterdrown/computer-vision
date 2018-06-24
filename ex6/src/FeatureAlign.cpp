///////////////////////////////////////////////////////////////////////////
//
// NAME
//  FeatureAlign.h -- image registration using feature matching
//
// SEE ALSO
//  FeatureAlign.h      longer description
//
// Copyright ?Richard Szeliski, 2001.  See Copyright.h for more details
// (modified for CSE576 Spring 2005)
//
///////////////////////////////////////////////////////////////////////////

#include "../ImageLib/include/ImageLib.h"
#include "FeatureAlign.h"
#include <math.h>

/******************* TO DO *********************
* alignPair:
*	INPUT:
*		f1, f2: source feature sets
*		matches: correspondences between f1 and f2
*               *NOTE* Each match in 'matches' contains two feature ids of matching features, id1 (in f1) and id2 (in f2).
*               These ids are 1-based indices into the feature arrays,
*               so you access the appropriate features as f1[id1-1] and f2[id2-1].
*		m: motion model
*		f: focal length
*		nRANSAC: number of RANSAC iterations
*		RANSACthresh: RANSAC distance threshold
*		M: transformation matrix (output)
*	OUTPUT:
*		repeat for nRANSAC iterations:
*			choose a minimal set of feature matches
*			estimate the transformation implied by these matches
*			count the number of inliers
*		for the transformation with the maximum number of inliers,
*		compute the least squares motion estimate using the inliers,
*		and store it in M
*/
int alignPair(const FeatureSet &f1, const FeatureSet &f2,
              const vector<FeatureMatch> &matches, MotionModel m, float f,
              int nRANSAC, double RANSACthresh, CTransform3x3& M)
{
    // BEGIN TODO
    // write this entire method

	int bestIndex;   // holds index of match with most inclusive translation
	int testIndex;	 // holds current index being tested for number of inliers
	vector<int> bestIndexAssociatedInliers; //the indices of matches that follow the same translation as the bestIndex
	int bestInlierCount = 0; //number of inliers in bestIndexAssociatedInliers

	FeatureMatch match;
	int xTrans, yTrans;
	Feature feature1, feature2;
	CTransform3x3 transMatrix;


	/*repeat the following cycle:
	1. pick a random feature pair (ie: a feature and its match from the other image)
	2. determine the translation (ie: change in x and y that would be required to make
	the coordinates of f1 equal to the coordinates of its match)
	for ex: if f1 was at pixel 4,7 and f2 was at pixel 5,8 in its coordinate space, then the required translation owuld be +1,+1
	3. apply that translation to every feature in f1Set and see if it is within tolerance of its match from f2Set
	4. if it is close enough to f2, count it as an inlier, if it isn't, do not count it.
	5. keep track of the translation and the number of inliers.
	*/

	for (int i = 0; i < nRANSAC; i++) {

		testIndex = rand() % matches.size(); // provides a random index between 0 and the size of matches	

		feature1 = f1[matches[testIndex].id1 - 1];
		feature2 = f2[matches[testIndex].id2 - 1];

		xTrans = feature2.x - feature1.x;
		yTrans = feature2.y - feature1.y;

		transMatrix = CTransform3x3::Translation((float)xTrans, (float)yTrans);

		vector<int> inliers;
		int count = countInliers(f1, f2, matches, m, f, transMatrix, RANSACthresh, inliers);

		if (count > bestInlierCount) {
			bestIndex = i;
			bestInlierCount = count;
			bestIndexAssociatedInliers = inliers;
			M = transMatrix;
		}

	}

	leastSquaresFit(f1, f2, matches, m, f, bestIndexAssociatedInliers, M);

    // END TODO

    return 0;
}

/******************* TO DO *********************
* countInliers:
*	INPUT:
*		f1, f2: source feature sets
*		matches: correspondences between f1 and f2
*               *NOTE* Each match contains two feature ids of matching features, id1 (in f1) and id2 (in f2).
*               These ids are 1-based indices into the feature arrays,
*               so you access the appropriate features as f1[id1-1] and f2[id2-1].
*		m: motion model
*		f: focal length
*		M: transformation matrix
*		RANSACthresh: RANSAC distance threshold
*		inliers: inlier feature IDs
*	OUTPUT:
*		transform the matched features in f1 by M
*
*		count the number of matching features for which the transformed
*		feature f1[id1-1] is within SSD distance RANSACthresh of its match
*		f2[id2-1]
*
*		store the indices of these matches in inliers
*
*		
*/
int countInliers(const FeatureSet &f1, const FeatureSet &f2,
                 const vector<FeatureMatch> &matches, MotionModel m, float f,
                 CTransform3x3 M, double RANSACthresh, vector<int> &inliers)
{
    inliers.clear();
    int count = 0;

    for (unsigned int i=0; i<(int) matches.size(); i++) {
        // BEGIN TODO
        // determine if the ith matched feature f1[id1-1], when transformed by M,
        // is within RANSACthresh of its match in f2
        //
        // if so, increment count and append i to inliers
        //
        // *NOTE* Each match contains two feature ids of matching features, id1 and id2.
        //        These ids are 1-based indices into the feature arrays,
        //        so you access the appropriate features as f1[id1-1] and f2[id2-1].

		CVector3 feat1Vec = CVector3();
		Feature testF1 = f1[matches[i].id1 - 1];
		Feature testF2 = f2[matches[i].id2 - 1];

		feat1Vec[0] = testF1.x;
		feat1Vec[1] = testF1.y;
		feat1Vec[2] = 1;

		CVector3 translatedF1 = M * feat1Vec;
		float dx = testF2.x - translatedF1[0];
		float dy = testF2.y - translatedF1[1];
		float distError = sqrt(dx*dx + dy * dy);
		if (distError <= RANSACthresh) {
			inliers.push_back(i);
			count++;
		}

        // END TODO
    }

    return count;
}

/******************* TO DO *********************
* leastSquaresFit:
*	INPUT:
*		f1, f2: source feature sets
*		matches: correspondences between f1 and f2
*		m: motion model
*		f: focal length
*		inliers: inlier match indices (indexes into 'matches' array)
*		M: transformation matrix (output)
*	OUTPUT:
*		compute the transformation from f1 to f2 using only the inliers
*		and return it in M
*/
int leastSquaresFit(const FeatureSet &f1, const FeatureSet &f2,
                    const vector<FeatureMatch> &matches, MotionModel m, float f,
                    const vector<int> &inliers, CTransform3x3& M)
{
    // for project 2, the transformation is a translation and
    // only has two degrees of freedom
    //
    // therefore, we simply compute the average translation vector
    // between the feature in f1 and its match in f2 for all inliers
    double u = 0;
    double v = 0;

    for (int i=0; i<inliers.size(); i++) {
        double xTrans, yTrans;

        // BEGIN TODO
        // compute the translation implied by the ith inlier match
        // and store it in (xTrans,yTrans)

		Feature testF1 = f1[matches[inliers[i]].id1 - 1];
		Feature testF2 = f2[matches[inliers[i]].id2 - 1];

		xTrans = testF2.x - testF1.x;
		yTrans = testF2.y - testF1.y;

        // END TODO

        u += xTrans;
        v += yTrans;
    }

    u /= inliers.size();
    v /= inliers.size();

    M[0][0] = 1;
    M[0][1] = 0;
    M[0][2] = u;
    M[1][0] = 0;
    M[1][1] = 1;
    M[1][2] = v;
    M[2][0] = 0;
    M[2][1] = 0;
    M[2][2] = 1;

    return 0;
}
