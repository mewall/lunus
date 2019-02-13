/* LXAVGRIM.C - Calculate the average pixel intensity product between two 
		images as a fuction of radius

   Author: Mike Wall
   Version:1.0
   Date:4/7/94
				 
	 Input argument is two diffraction image.  Output is stored in rfile
	 of first image (imdiff1->rfile).
*/

#include<mwmask.h>

int lxavgrim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2)
{
	RCCOORDS_DATA
		r1,
		r2,
		c1,
		c2;
	size_t
		*n,
		radius,
		index1 = 0,
		index2 = 0;

	struct rccoords 
		rvec;

	n = (size_t *)calloc(MAX_RFILE_LENGTH, sizeof(size_t));
	imdiff1->rfile_length = 0;
	for(r1 = 0; r1 < imdiff1->vpixels; r1++) {
          rvec.r = r1 - imdiff1->origin.r;
	  r2 = (rvec.r + imdiff2->origin.r);
	  for(c1 = 0; c1 < imdiff1->hpixels; c1++) {
	      rvec.c = c1 - imdiff1->origin.c;
	      radius = (size_t)sqrtf((float)(rvec.r*rvec.r) + 
			(float)(rvec.c*rvec.c));
	      if ((imdiff1->image[index1] != imdiff1->overload_tag) &&
		  (imdiff1->image[index1] != imdiff1->ignore_tag)) {
		c2 = (rvec.c + imdiff2->origin.c);
		if ((r2 > 0) && (r2 < imdiff2->vpixels) && (c2 > 0) && 
			(c2 < imdiff2->hpixels)) {
		  index2 = r2*imdiff2->hpixels + c2;
		  if ((imdiff2->image[index2] != imdiff2->overload_tag) &&
		      (imdiff2->image[index2] != imdiff2->ignore_tag )) {
	            if (radius > imdiff1->rfile_length) imdiff1->rfile_length = 
							radius;
		    imdiff1->rfile[radius] = (RFILE_DATA_TYPE)
			((float)imdiff1->image[index1] * 
			 (float)imdiff2->image[index2] + 
			(float)n[radius]*(float)imdiff1->rfile[radius]) /
			(RFILE_DATA_TYPE)(n[radius] + 1);
		    n[radius]++;
		  }
	        }
	      }
	    index1++;
	  }
	}
	free((size_t *)n);
}
