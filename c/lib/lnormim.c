/* LNORMIM.C - Correct for solid-angle normalization and 
              detector-face rotation in a diffraction image.
   
   Author: Mike Wall
   Date: 2/24/95
   Version: 1.
   
   */


#include<mwmask.h>

int lnormim(DIFFIMAGE *imdiff_in)
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
    distance_squared,
    cos_two_theta;
  
  XYZCOORDS_DATA ssq;

  int pidx;

  DIFFIMAGE *imdiff;

  for (pidx = 0; pidx < imdiff_in->num_panels; pidx++) {
    imdiff = &imdiff_in[pidx];
    index = 0;

    if (imdiff->slist == NULL) lslistim(imdiff);

    struct xyzcoords s;

    distance_squared = imdiff->distance_mm*imdiff->distance_mm;
    for(r=0; r < imdiff->vpixels; r++) {
      for(c=0; c < imdiff->hpixels; c++) {
	if ((imdiff->image[index] != imdiff->overload_tag) &&
	    (imdiff->image[index] != imdiff->ignore_tag)) {
	  s = imdiff->slist[index];
	  ssq = ldotvec(s,s);
	  cos_two_theta = 1. - ssq * imdiff->wavelength *imdiff->wavelength / 2.;
	  radius_squared = ((rvec.x*rvec.x) + (rvec.y*rvec.y));
	  imdiff->image[index] -= imdiff->value_offset;
	  imdiff->image[index] /= cos_two_theta * cos_two_theta * cos_two_theta;
	  imdiff->image[index] += imdiff->value_offset;
	}
	index++;
      }
    }
  }
  return(return_value);
}








