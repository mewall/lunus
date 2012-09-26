/* LNIGN.C - Calculate the number of ignored pixels vs radius for an
		input image.
   
   Author: Mike Wall   
   Date: 5/5/93
   Version: 1.
   
   */

#include<mwmask.h>

int lnign(DIFFIMAGE *imdiff)
{
	size_t
		r,
		c,
		*n,
		radius,
		index = 0;

	struct rccoords rvec;

	n = (size_t *)calloc(MAX_RFILE_LENGTH, sizeof(size_t));
	imdiff->rfile_length = 0;
	for(r = 0; r < imdiff->vpixels; r++) {
	  for(c = 0; c < imdiff->hpixels; c++) {
	      rvec.r = r - imdiff->origin.r;
	      rvec.c = c - imdiff->origin.c;
	      radius = (size_t)sqrtf((float)(rvec.r*rvec.r + rvec.c*rvec.c));
	      if ((imdiff->image[index] == imdiff->overload_tag) || 
		  (imdiff->image[index] == imdiff->ignore_tag)) {
	        if (radius > imdiff->rfile_length) 
			imdiff->rfile_length = radius;
		n[radius]++;
	      }
	    index++;
	  }
	}
	for(index=0; index <= imdiff->rfile_length; index++) {
	  imdiff->rfile[index] = n[index];
	}
	free((size_t *)n);
}
