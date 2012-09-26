/* LBUTTIM.C - Mask a diffraction image using a butterfly shape.
   
   Author: Mike Wall
   Date: 6/22/94
   Version: 1.
   
   */

#include<mwmask.h>

int lbuttim(DIFFIMAGE *imdiff)
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
    rho,
    rho_offset,
    upper_limit_1,
    lower_limit_1,
    upper_limit_2,
    lower_limit_2;

  rho_offset = PI/180.*imdiff->polarization_offset;
  upper_limit_1 = rho_offset + imdiff->polarization/2.;
  lower_limit_1 = rho_offset - imdiff->polarization/2.;
  upper_limit_2 = upper_limit_1 + 180.;
  lower_limit_2 = lower_limit_1 + 180.;
  for(r=0; r < imdiff->vpixels; r++) {
    rvec.y = (float)(r - imdiff->origin.r);
    for(c=0; c < imdiff->hpixels; c++) {
      rvec.x = (float)(c - imdiff->origin.c);
      rho = 180./PI*atanf(rvec.y/rvec.x);
      if (rvec.x < 0) {
        rho = rho + 180.;
      }
      if (!(((upper_limit_1 > rho) && (lower_limit_1 < rho)) ||
          ((upper_limit_2 > rho) && (lower_limit_2 < rho)))) {
        imdiff->image[index] = imdiff->ignore_tag;
      }
      index++;
    }
  }
  return(return_value);
}
