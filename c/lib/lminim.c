/* LMINIM.C - Calculate the minimum intensity for an input image.
   
   Author: Mike Wall   
   Date: 9/14/2017
   Version: 1.
   
*/

#include<mwmask.h>

int lminim(DIFFIMAGE *imdiff)
{
	size_t
		r,
		c,
		n=0,
		index = 0;

	IMAGE_DATA_TYPE
	  minval;

	struct rccoords rvec;

	minval = imdiff->ignore_tag;       
	for(r = 0; r < imdiff->vpixels; r++) {
	  for(c = 0; c < imdiff->hpixels; c++) {
	      if ((imdiff->image[index] != imdiff->overload_tag) &&
		  (imdiff->image[index] != imdiff->ignore_tag)) {
		if (minval > imdiff->image[index] || 
		    minval == imdiff->ignore_tag) {
		  minval = imdiff->image[index];
		  rvec.r=r;
		  rvec.c=c;
		}
	      }
	    index++;
	  }
	}
	imdiff->min_pixel_value = minval;
	//	printf("Minimum value of %d at position %d,%d\n",minval,rvec.r,rvec.c);
}
