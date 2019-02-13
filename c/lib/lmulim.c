/* LMULIM.C - Calculate the pixel-by-pixel product of two images, storing 
		the result in the first image.

   Author: Mike Wall
   Version:1.0
   Date:4/27/94
			
	 Input argument is two diffraction images.  Output difference is
  stored in imdiff1->image.

*/

#include<mwmask.h>

int lmulim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2)
{
	RCCOORDS_DATA
		r1,
		r2,
		c1,
		c2;
	size_t
		radius,
		index1 = 0,
		index2 = 0;

	struct rccoords 
		rvec;

        float
		value;

	int 
		return_value = 0;

	for(r1 = 0; r1 < imdiff1->vpixels; r1++) {
          rvec.r = r1 - imdiff1->origin.r;
	  r2 = (rvec.r + imdiff2->origin.r);
	  for(c1 = 0; c1 < imdiff1->hpixels; c1++) {
	      rvec.c = c1 - imdiff1->origin.c;
	      if ((imdiff1->image[index1] != imdiff1->overload_tag) &&
		  (imdiff1->image[index1] != imdiff1->ignore_tag)) {
		c2 = (rvec.c + imdiff2->origin.c);
		if ((r2 > 0) && (r2 < imdiff2->vpixels) && (c2 > 0) && 
			(c2 < imdiff2->hpixels)) {
		  index2 = r2*imdiff2->hpixels + c2;
		  if ((imdiff2->image[index2] != imdiff2->overload_tag) &&
		      (imdiff2->image[index2] != imdiff2->ignore_tag )) {
                    value = imdiff1->image[index1] * imdiff2->image[index2];
                    if (value < MAX_IMAGE_DATA_VALUE) {
                      imdiff1->image[index1] = (IMAGE_DATA_TYPE)value; 
                    }
                    else imdiff1->image[index1] = (MAX_IMAGE_DATA_VALUE);
		  }
		  else {
		    imdiff1->image[index1] = imdiff1->ignore_tag;
                  }
	        }
		else {
		  imdiff1->image[index1] = imdiff1->ignore_tag;
                }
	      }
	    index1++;
	  }
	}
	return(return_value);

}
