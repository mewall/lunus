/* LWINDIM.C - Mark all pixels outside a window in an image with an ignore tag.
   
   Author: Mike Wall  
   Date: 4/25/94
   Version: 1.
   
   */

#include<mwmask.h>

int lwindim(DIFFIMAGE *imdiff_in)
{
  size_t 
    index = 0;

  RCCOORDS_DATA
    r,
    c;

  int 
    return_value = 0;

  int pidx;

  DIFFIMAGE *imdiff;

  for (pidx = 0; pidx < imdiff_in->num_panels; pidx++) {
    imdiff = &imdiff_in[pidx];
    index = 0;

    if (imdiff->slist == NULL) lslistim(imdiff);

    struct xyzcoords s;

    XYZCOORDS_DATA rr, cos_two_theta, ssq;

    struct xycoords rvec;

    struct rccoords pixel;

    for(r=0; r < imdiff->vpixels; r++) {
      for(c=0; c < imdiff->hpixels; c++) {
	s = imdiff->slist[index];
	ssq = ldotvec(s,s);
	cos_two_theta = 1. - ssq * imdiff->wavelength * imdiff->wavelength / 2.;
	rr = imdiff->distance_mm / cos_two_theta; 
	rvec.x = imdiff->wavelength * s.x * rr;
	rvec.y = imdiff->wavelength * s.y * rr;
	pixel.c = (RCCOORDS_DATA)((rvec.x + imdiff->beam_mm.x) / 
				  imdiff->pixel_size_mm + 0.5);
	pixel.r = (RCCOORDS_DATA)((rvec.y + imdiff->beam_mm.y) / 
				  imdiff->pixel_size_mm + 0.5);
	if ((pixel.r < imdiff->window_lower.r) || 
	    (pixel.r > imdiff->window_upper.r) || 
	    (pixel.c < imdiff->window_lower.c) || 
	    (pixel.c > imdiff->window_upper.c)) {
	  imdiff->image[index] = imdiff->ignore_tag;
	}
	index++;
      }
    }
  }
  return(return_value);
}
