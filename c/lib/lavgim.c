/* LAVGIM.C - Calculate the average intensity for an input image.
   
   Author: Mike Wall   
   Date: 1/11/95
   Version: 1.
   
*/

#include<mwmask.h>

int lavgim(DIFFIMAGE *imdiff)
{
	size_t
		r,
		c,
		n=0,
		index = 0;

	struct rccoords rvec;

	float
	  avg_pixel_value = 0;

	for(r = 0; r < imdiff->vpixels; r++) {
	  for(c = 0; c < imdiff->hpixels; c++) {
	      if ((imdiff->image[index] != imdiff->overload_tag) &&
		  (imdiff->image[index] != imdiff->ignore_tag)) {
		avg_pixel_value = (n*avg_pixel_value + 
		  (float)imdiff->image[index])/(float)(n+1.);
		n++;
	      }
	    index++;
	  }
	}
	imdiff->avg_pixel_value = (RFILE_DATA_TYPE)avg_pixel_value;
}
