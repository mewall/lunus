/* LNORMIM.C - Correct for solid-angle normalization and 
              detector-face rotation in a diffraction image.
   
   Author: Mike Wall
   Date: 2/24/95
   Version: 1.
   
   */

#include<mwmask.h>

int lnormim(DIFFIMAGE *imdiff)
{
  size_t 
    index = 0;
  
  RCCOORDS_DATA
    r,
    c;
  
  struct xycoords
    rvec,norm_center;
  
  int 
    return_value = 0;
  
  
  float 
    correction_factor,
    radius_squared,
    distance,
    distance_squared;

  XYZCOORDS_DATA dvecnewlen;

  struct xyzmatrix R; 
  struct xyzcoords dir,dirnew,cassrad,dvec,dvecnew,dnorm;

  // Calcaulate position of closest point on detector to specimen, given cassette rotations

  cassrad.x = imdiff->cassette.x*PI/180.;
  cassrad.y = imdiff->cassette.y*PI/180.;
  cassrad.z = imdiff->cassette.z*PI/180.;

  // First calculate the direction of the detector face normal. It's in the z-direction (pointing back to the crystal) by default

  R = lrotmat(cassrad.x,cassrad.y,cassrad.z);

  dir.x = 0.0;
  dir.y = 0.0;
  dir.z = 1.0;


  dirnew = lmatvecmul(R,dir);

  // Calculate the vector from the beam center to the plane parallel to the beam face containing the crystal

  dvecnewlen = imdiff->distance_mm*ldotvec(dirnew,dir);

  dvecnew = lmulscvec(dvecnewlen,dirnew);

  // Calculate the vector from the beam center to the crystal

  dvec = lmulscvec(imdiff->distance_mm,dir);

  // Calculate the difference vector, which indicates the vector shift to the new center for the normalization

  dnorm = lsubvec(dvec,dvecnew);

  // Now project the difference vector on the detector face and calculate the norm center. 

  // First along the x direction

  dir.x = 1.0;
  dir.y = 0.0;
  dir.z = 0.0;

  dirnew = lmatvecmul(R,dir);

  norm_center.x = imdiff->beam_mm.x + ldotvec(dnorm,dirnew);

  // First along the x direction

  dir.x = 0.0;
  dir.y = 1.0;
  dir.z = 0.0;

  dirnew = lmatvecmul(R,dir);

  norm_center.y = imdiff->beam_mm.y + ldotvec(dnorm,dirnew);

  distance_squared = dvecnewlen*dvecnewlen;
  for(r=0; r < imdiff->vpixels; r++) {
    rvec.y = (float)(r*imdiff->pixel_size_mm-norm_center.y);
    for(c=0; c < imdiff->hpixels; c++) {
      if ((imdiff->image[index] != imdiff->overload_tag) &&
	  (imdiff->image[index] != imdiff->ignore_tag)) {
	rvec.x = (float)(c*imdiff->pixel_size_mm-norm_center.x);
	radius_squared = ((rvec.x*rvec.x) + (rvec.y*rvec.y));
	correction_factor = 1. + radius_squared/distance_squared;
	imdiff->image[index] -= imdiff->value_offset;
	imdiff->image[index] *= correction_factor * sqrtf(correction_factor);
	imdiff->image[index] += imdiff->value_offset;
      }
      index++;
    }
  }
  return(return_value);
}








