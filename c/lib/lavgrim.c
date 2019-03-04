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
	  i,
	  r,
		c,
		*n,
		radius,
		index = 0;

	struct xycoords rvec;

	RFILE_DATA_TYPE *rf;

	index = 0;
	n = (size_t *)calloc(MAX_RFILE_LENGTH, sizeof(size_t));
	rf = (RFILE_DATA_TYPE *)calloc(MAX_RFILE_LENGTH,sizeof(RFILE_DATA_TYPE));
	imdiff->rfile_length = 0;
	for(r = 0; r < imdiff->vpixels; r++) {
	  rvec.y = r*imdiff->pixel_size_mm - imdiff->beam_mm.y;
	  for(c = 0; c < imdiff->hpixels; c++) {
	      rvec.x = c*imdiff->pixel_size_mm - imdiff->beam_mm.x;
	      radius = (size_t)(sqrtf(rvec.x*rvec.x + rvec.y*rvec.y)/imdiff->pixel_size_mm+.5);
	      if ((imdiff->image[index] != imdiff->overload_tag) &&
		  (imdiff->image[index] != imdiff->ignore_tag)) {
	        if (radius >= imdiff->rfile_length) 
			imdiff->rfile_length = radius+1;
		rf[radius] += (RFILE_DATA_TYPE)(float)(imdiff->image[index]-imdiff->value_offset);
		n[radius]++;
	      }
	    index++;
	  }
	}
	for(i=0;i<imdiff->rfile_length;i++) {
	  if (n[i] > 0) {
	    imdiff->rfile[i] = rf[i]/(RFILE_DATA_TYPE)n[i];
#ifdef DEBUG
	    if (i>100 && i<=110) printf("lavgrim rf[%d] = %g,",i,rf[i]);
#endif
	  } else {
	    imdiff->rfile[i] = 0.0;
	  }
	}
	free(n);
	free(rf);
}
