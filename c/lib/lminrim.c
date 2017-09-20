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

	size_t i;

	if (imdiff->rfile != NULL) {
	  free(imdiff->rfile);
	}
	imdiff->rfile = (RFILE_DATA_TYPE *)malloc(MAX_RFILE_LENGTH*sizeof(RFILE_DATA_TYPE));
	for (i=0;i<MAX_RFILE_LENGTH;i++) imdiff->rfile[i]=imdiff->ignore_tag;
	imdiff->rfile_length = 0;
	for(r = 0; r < imdiff->vpixels; r++) {
	  rvec.y = r*imdiff->pixel_size_mm - imdiff->beam_mm.y;
	  for(c = 0; c < imdiff->hpixels; c++) {
	      rvec.x = c*imdiff->pixel_size_mm - imdiff->beam_mm.x;
	      radius = (size_t)(sqrtf(rvec.x*rvec.x + rvec.y*rvec.y)/imdiff->pixel_size_mm+.5);
	      if (radius > imdiff->rfile_length) imdiff->rfile_length = radius+1;
	      if (imdiff->image[index] != imdiff->overload_tag && imdiff->image[index] != imdiff->mask_tag && imdiff->image[index] != imdiff->ignore_tag) {
		/*		if ((imdiff->image[index] != 
			(IMAGE_DATA_TYPE)imdiff->rfile_mask_tag) && 
			((imdiff->rfile[radius] == imdiff->rfile_mask_tag) ||
		    	(imdiff->rfile[radius] > 
			(IMAGE_DATA_TYPE)imdiff->image[index]))) {*/
		if (imdiff->rfile[radius] > 
		    (RFILE_DATA_TYPE)imdiff->image[index] || imdiff->rfile[radius] == imdiff->ignore_tag) {
		  imdiff->rfile[radius] = (RFILE_DATA_TYPE)imdiff->image[index];
	        }
	      }
	  index++;
	  }
	}
}
