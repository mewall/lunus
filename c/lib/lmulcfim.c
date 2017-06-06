/* LMULCFIM.C - Multiply correction factors by an image pixel-by-pixel

   Author: Mike Wall
   Version:1.0
   Date:6/6/2017
			

*/

#include<mwmask.h>

int lmulcfim(DIFFIMAGE *imdiff)
{
	size_t
	  i;

	int 
		return_value = 0;

	for (i = 0;i<imdiff->image_length;i++) {
	  if ((imdiff->image[i] != imdiff->overload_tag) &&
	      (imdiff->image[i] != imdiff->ignore_tag)) {
	    imdiff->image[i] *= imdiff->correction[i];
	  }
	}

	return(return_value);

}
