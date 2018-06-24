///////////////////////////////////////////////////////////////////////////
//
// NAME
//  WarpSpherical.h -- warp a flat (perspective) image into spherical
//      coordinates and/or undo radial lens distortion
//
// SEE ALSO
//  WarpSpherical.h   longer description
//
//  R. Szeliski and H.-Y. Shum.
//  Creating full view panoramic image mosaics and texture-mapped models.
//  Computer Graphics (SIGGRAPH'97), pages 251-258, August 1997.
//
// Copyright ?Richard Szeliski, 2001.  See Copyright.h for more details
// (modified for CSE455 Spring 2008)
//
///////////////////////////////////////////////////////////////////////////

#include "../ImageLib/include/ImageLib.h"
#include "WarpSpherical.h"
#include <math.h>

/******************* TO DO *********************
 * warpSphericalField:
 *	INPUT:
 *		srcSh: shape (width, height, number of channels) of source image
 *		dstSh: shape of destination image
 *		f: focal length in pixels (provided on the project web page, or measured by yourself)
 *		k1, k2: radial distortion parameters (ditto)
 *		r: rotation matrix
 *	OUTPUT:
 *		Return an image containing (u,v) coordinates for mapping pixels from
 *		spherical coordinates to planar image coordinates and applying
 *		radial distortion.
 *			Note that this is inverse warping, i.e., this routine will be
 *		actually used to map from planar coordinates with radial distortion
 *		to spherical coordinates without radial distortion.
 *
 */
CFloatImage WarpSphericalField(CShape srcSh, CShape dstSh, float f,
                                 float k1, float k2, const CTransform3x3 &r)
{
  // Set up the pixel coordinate image
  dstSh.nBands = 2;
  CFloatImage uvImg(dstSh);   // (u,v) coordinates

  // Fill in the values
  for (int y = 0; y < dstSh.height; y++) {
    float *uv = &uvImg.Pixel(0, y, 0);
    for (int x = 0; x < dstSh.width; x++, uv += 2) {
      float xf = (x - 0.5f*dstSh.width ) / f;
      float yf = (y - 0.5f*dstSh.height) / f;
      float xt, yt, zt;
      CVector3 p;

      // *** BEGIN TODO ***

      //convert the theta and phi to spherical coordinates 
      p[0] = sin(xf)*cos(yf);
      p[1] = sinf(yf);
      p[2] = cos(xf)*cos(yf);

      //p = r*p; //p now holds the rotated spherical coordinates ?

      //normalize all the coordinates by z_hat
      xt = p[0] / p[2];
      yt = p[1] / p[2];
      zt = 1;

      //apply radial distortion
      float r_squared = xt * xt + yt * yt;
      xt = xt * (1 + k1 * r_squared + k2 * r_squared*r_squared);
      yt = yt * (1 + k1 * r_squared + k2 * r_squared*r_squared);

      // *** END TODO ***

      // Convert back to regular pixel coordinates and store
      float xn = 0.5f*srcSh.width  + xt*f;
      float yn = 0.5f*srcSh.height + yt*f;
      uv[0] = xn;
      uv[1] = yn;
    }
  }
  return uvImg;
}
