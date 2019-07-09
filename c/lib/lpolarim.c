/* LPOLARIM.C - Perform polarization correction on a diffraction image.
   
   Author: Mike Wall
   Date: 4/26/94
   Version: 1.
   
   */

#include<mwmask.h>

int lpolarim(DIFFIMAGE *imdiff_in)
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
    radius,
    radius_squared,
    distance_pixels,
    arctan_argument,
    two_theta,
    cos_two_theta,
    cos_sq_two_theta,
    sin_sq_two_theta,
    cos_two_rho,
    two_rho_offset;

  XYZCOORDS_DATA ssq;

  int pidx;

  DIFFIMAGE *imdiff;

  if (imdiff_in->slist == NULL) lslistim(imdiff_in);

  for (pidx = 0; pidx < imdiff_in->num_panels; pidx++) {
    imdiff = &imdiff_in[pidx];
    index = 0;

    struct xyzcoords s;

    two_rho_offset = 2.*PI/180.*imdiff->polarization_offset;
    for(r=0; r < imdiff->vpixels; r++) {
      rvec.y = (float)(r*imdiff->pixel_size_mm-imdiff->beam_mm.y);
      for(c=0; c < imdiff->hpixels; c++) {
	if ((imdiff->image[index] != imdiff->overload_tag) &&
	    (imdiff->image[index] != imdiff->ignore_tag)) {
	  rvec.x = (float)(c*imdiff->pixel_size_mm-imdiff->beam_mm.x);
	  s = imdiff->slist[index];
	  ssq = ldotvec(s,s);
	  cos_two_theta = 1. - ssq * imdiff->wavelength *imdiff->wavelength / 2.;
	  cos_sq_two_theta = cos_two_theta * cos_two_theta;
	  sin_sq_two_theta = 1. - cos_sq_two_theta;
	  cos_two_rho = cosf(2.*acosf(s.x / sqrtf(s.x*s.x+s.y*s.y)) - two_rho_offset);
	  imdiff->image[index] -= imdiff->value_offset;
	  imdiff->image[index]=(IMAGE_DATA_TYPE)((float)imdiff->image[index] *
						 2. / (1. + cos_sq_two_theta -
						       imdiff->polarization*cos_two_rho*
						       sin_sq_two_theta));
	  imdiff->image[index] += imdiff->value_offset;
	}
	index++;
      }
    }
  }
  return(return_value);
}
