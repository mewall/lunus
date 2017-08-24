/* LCULLIM.C - Set all pixel values outside a specified radius range to a mask value
   
   Author: Mike Wall
   Date: 1/19/2016
   Version: 1.
   
   */

#include<mwmask.h>

int lcullim(DIFFIMAGE *imdiff)
{
	size_t
		r,
		c,
		radius,
		index = 0;

	struct xycoords rvec;

	int
		return_value;

	for(r = 0; r < imdiff->vpixels; r++) {
	  rvec.y = r*imdiff->pixel_size_mm - imdiff->beam_mm.y;
	  for(c = 0; c < imdiff->hpixels; c++) {
	      rvec.x = c*imdiff->pixel_size_mm - imdiff->beam_mm.x;
	      radius = (size_t)(sqrtf(rvec.x*rvec.x + rvec.y*rvec.y)/imdiff->pixel_size_mm+.5);
	      if (radius < imdiff->rfirange.l || radius > imdiff->rfirange.u) {
		imdiff->image[index] = imdiff->ignore_tag;
	      }
	      index++;
	  }
	}
	return(return_value);
}


