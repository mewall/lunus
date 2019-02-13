/* LMULRFIM - Multiply an image by an rfile -> image.
   
   Author: Mike Wall
   Date: 4/4/94
   Version: 1.
   
   */

#include<mwmask.h>

int lmulrfim(DIFFIMAGE *imdiff)
{
	size_t
		r,
		c,
		radius,
		index = 0;

	struct rccoords rvec;

	int
		return_value = 0;

	for(r = 0; r < imdiff->vpixels; r++) {
	  for(c = 0; c < imdiff->hpixels; c++) {
	      rvec.r = r - imdiff->origin.r;
	      rvec.c = c - imdiff->origin.c;
	      radius = (size_t)sqrtf((float)(rvec.r*rvec.r + rvec.c*rvec.c));
	      if (radius < imdiff->rfile_length) {
	        if (imdiff->image[index] != imdiff->overload_tag) {
		  imdiff->image[index] = (IMAGE_DATA_TYPE)(
		      (RFILE_DATA_TYPE)imdiff->image[index] * 
		      imdiff->rfile[radius]); 
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
