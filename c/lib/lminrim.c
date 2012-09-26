/* LMINRIM.C - Calculate the minimum intensity as a function of radius for an
		input image.
   
   Author: Mike Wall   
   Date: 4/4/93
   Version: 1.
   
   */

#include<mwmask.h>

int lminrim(DIFFIMAGE *imdiff)
{
	size_t
		radius,
		index = 0;

	RCCOORDS_DATA
		r,
		c;

	struct xycoords rvec;

	imdiff->rfile_length = 0;
	for(r = 0; r < imdiff->vpixels; r++) {
	  for(c = 0; c < imdiff->hpixels; c++) {
	      rvec.y = (XYZCOORDS_DATA)(r - imdiff->origin.r);
	      rvec.x = (XYZCOORDS_DATA)(c - imdiff->origin.c);
	      radius = (size_t)sqrtf(rvec.y*rvec.y + rvec.x*rvec.x);
	      if (radius > imdiff->rfile_length) imdiff->rfile_length = radius;
	      if (imdiff->image[index] != imdiff->overload_tag) {
		if ((imdiff->image[index] != 
			(IMAGE_DATA_TYPE)imdiff->rfile_mask_tag) && 
			((imdiff->rfile[radius] == imdiff->rfile_mask_tag) ||
		    	(imdiff->rfile[radius] > 
				(RFILE_DATA_TYPE)imdiff->image[index]))) {
		  imdiff->rfile[radius] = (RFILE_DATA_TYPE)imdiff->image[index];
	        }
	      }
	  index++;
	  }
	}
}
