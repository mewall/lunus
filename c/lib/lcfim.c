/* LCFIM.C - Calculate correction factor for a diffraction image.
   
   Author: Mike Wall
   Date: 6/5/2017
   Version: 1.
   
   */

#include<mwmask.h>

int lcfim(DIFFIMAGE *imdiff)
{
  size_t 
    index = 0;
  
  RCCOORDS_DATA
    r,
    c;
  
  struct xycoords
    rvec;
  
  int 
    return_value = 0;
  
  
  float 
    scale,
    radius,
    radius_squared,
    distance_pixels,
    arctan_argument,
    two_theta,
    cos_two_theta,
    sin_two_theta,
    cos_two_rho,
    two_rho_offset,
    maxcorr;

  scale = imdiff->correction_factor_scale;
  two_rho_offset = 2.*PI/180.*imdiff->polarization_offset;
  for(r=0; r < imdiff->vpixels; r++) {
    rvec.y = (float)(r*imdiff->pixel_size_mm-imdiff->beam_mm.y);
    for(c=0; c < imdiff->hpixels; c++) {
	rvec.x = (float)(c*imdiff->pixel_size_mm-imdiff->beam_mm.x);
	radius_squared = ((rvec.x*rvec.x) + (rvec.y*rvec.y));
	radius = sqrtf(radius_squared);
	arctan_argument = radius / imdiff->distance_mm;
	//	if (arctan_argument > POLARIZATION_CORRECTION_THRESHOLD) {
	  two_theta = atanf(arctan_argument);
	  cos_two_theta = cosf(two_theta);
	  sin_two_theta = sinf(two_theta);
	  cos_two_rho = cosf(2*acosf(rvec.x / radius) - two_rho_offset);
          // polarization
	  imdiff->correction[index] = scale*(2. / (1. + cos_two_theta*cos_two_theta -
			       imdiff->polarization*cos_two_rho*
			         sin_two_theta*sin_two_theta));
          // solid angle normalization
	  imdiff->correction[index] *= powf((1. + arctan_argument*arctan_argument),3./2.);
	  //	  if (imdiff->correction[index]>maxcorr) {
	  //  maxcorr = imdiff->correction[index];
	  //}
	  //    	} else {
	  //	  imdiff->correction[index] = scale;
	  //	}
      index++;
    }
  }
  //  printf("Maximum correction = %f\n",maxcorr);
  return(return_value);
}
