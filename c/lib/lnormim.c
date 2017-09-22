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
    rvec;
  
  int 
    return_value = 0;
  
  
  float 
    correction_factor,
    radius_squared,
    distance,
    distance_squared;
  
  distance_squared = imdiff->distance_mm*imdiff->distance_mm;
  for(r=0; r < imdiff->vpixels; r++) {
    rvec.y = (float)(r*imdiff->pixel_size_mm-imdiff->beam_mm.y);
    for(c=0; c < imdiff->hpixels; c++) {
      if ((imdiff->image[index] != imdiff->overload_tag) &&
	  (imdiff->image[index] != imdiff->ignore_tag)) {
	rvec.x = (float)(c*imdiff->pixel_size_mm-imdiff->beam_mm.x);
	radius_squared = ((rvec.x*rvec.x) + (rvec.y*rvec.y));
	correction_factor = 1. + radius_squared/distance_squared;
	  // - 2*PI/180.*(rvec.x*imdiff->cassette.y -
	  //    rvec.y*imdiff->cassette.x)/imdiff->distance_mm;
	// imdiff->image[index] -= imdiff->value_offset;
	imdiff->image[index] *= correction_factor * sqrtf(correction_factor);
	// imdiff->image[index] += imdiff->value_offset;
      }
      index++;
    }
  }
  return(return_value);
}








