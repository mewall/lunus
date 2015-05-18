/* LAVGRIM.C - Calculate the average intensity vs. radius for an
		input image.
   
   Author: Mike Wall   
   Date: 4/3/93
   Version: 1.
   
   */

#include<mwmask.h>

int lavgrim(DIFFIMAGE *imdiff)
{
	size_t
		r,
		c,
		*n,
		radius,
		index = 0;

	struct xycoords rvec;

	n = (size_t *)calloc(MAX_RFILE_LENGTH, sizeof(size_t));
	imdiff->rfile_length = 0;
	for(r = 0; r < imdiff->vpixels; r++) {
	  rvec.y = r*imdiff->pixel_size_mm - imdiff->beam_mm.y;
	  for(c = 0; c < imdiff->hpixels; c++) {
	      rvec.x = c*imdiff->pixel_size_mm - imdiff->beam_mm.x;
	      radius = (size_t)(sqrtf(rvec.x*rvec.x + rvec.y*rvec.y)/imdiff->pixel_size_mm+.5);
	      if ((imdiff->image[index] != imdiff->overload_tag) &&
		  (imdiff->image[index] != imdiff->ignore_tag)) {
	        if (radius > imdiff->rfile_length) 
			imdiff->rfile_length = radius;
		imdiff->rfile[radius] = (RFILE_DATA_TYPE)
		  ((float)(imdiff->image[index]-imdiff->value_offset)+ 
				(float)n[radius]*(float)imdiff->rfile[radius]) /
				(RFILE_DATA_TYPE)(n[radius] + 1);
		n[radius]++;
	      }
	    index++;
	  }
	}
	//	free((size_t *)n);
}
