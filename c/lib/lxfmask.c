/* LXFMASK.C - Mask pixels in a diffraction image which are masked in another
		image.

   Author: Mike Wall
   Version:1.0
   Date:4/11/94
				 
	 Input argument is two diffraction image.  Output is stored in 
	 first image. 
*/

#include<mwmask.h>

int lxfmask(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2)
{
	size_t
		index2 = 0,
		index1 = 0;
	
	int 
		return_value = 0;

	struct rccoords
		delta_origin;

	RCCOORDS_DATA
		r1,
		c1,
		r2,
		c2;

  delta_origin.r = imdiff2->origin.r - imdiff1->origin.r;
  delta_origin.c = imdiff2->origin.c - imdiff1->origin.c;
  for(r1 = 0; r1 < imdiff1->vpixels; r1++) {
    r2 = r1 + delta_origin.r;
    for(c1 = 0; c1 < imdiff2->hpixels; c1++) {
      c2 = c1 + delta_origin.c;
      if ((r2 >= 0) && (r2 < imdiff2->vpixels) && (c2 >= 0) &&
          (c2 < imdiff2->hpixels)) {
        index2 = r2*imdiff2->hpixels + c2;
        if ((imdiff2->image[index2] == imdiff2->overload_tag) ||
            (imdiff2->image[index2] == imdiff2->ignore_tag)) {
          if (imdiff1->image[index1] != imdiff1->overload_tag) {
	    imdiff1->image[index1] = imdiff1->ignore_tag; 
          }
        }
      }
      index1++;
    }
  }
  return(return_value);
}
