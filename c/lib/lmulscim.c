/* LMULSCIM.C - Calculate the product of an image and a scalar.

   Author: Mike Wall
   Version:1.0
   Date:10/25/2016
			

*/

#include<mwmask.h>

int lmulscim(DIFFIMAGE *imdiff)
{
	RCCOORDS_DATA
	  r, c;
	size_t
	  index;

	int 
		return_value = 0;

	index = 0;
	for(r = 0; r < imdiff->vpixels; r++) {
	  for(c = 0; c < imdiff->hpixels; c++) {
	    if ((imdiff->image[index] != imdiff->overload_tag) &&
		(imdiff->image[index] != imdiff->ignore_tag)) {
	      imdiff->image[index] *= imdiff->amplitude;
	    }
	    index++;
	  }
	}
	return(return_value);

}
