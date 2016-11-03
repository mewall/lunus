/* LSUBRFIM.C - Subtract I(r) from a diffraction image.
   
   Author: Mike Wall
   Date: 4/4/94
   Version: 1.
   
   */

#include<mwmask.h>

int lsubrfim(DIFFIMAGE *imdiff)
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
	      if (radius < imdiff->rfile_length) {
		index = r*imdiff->hpixels+c;
	        if ((imdiff->image[index] != imdiff->overload_tag) &&
		    (imdiff->image[index] != imdiff->ignore_tag) &&
		    (imdiff->image[index] != imdiff->mask_tag)) {
		  imdiff->image[index] -= (IMAGE_DATA_TYPE)imdiff->rfile[radius]; 
	        }
	      }
	      else {
		return_value = 1;
		sprintf(imdiff->error_msg,"\nAttempt to read past end "
			"of rfile.\n\n");
	      }
	    index++;
	  }
	}
	return(return_value);
}


