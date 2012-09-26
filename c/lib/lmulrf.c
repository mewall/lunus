/* LMULRF.C - Multiply two rfiles.
   
   Author: Mike Wall
   Date: 1/12/95
   Version: 1.
   
   */

#include<mwmask.h>

int lmulrf(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2)
{
	size_t
		radius;

	int
		return_value = 0;

  for(radius = 0; radius < imdiff1->rfile_length; radius++) {
    if (imdiff1->rfile[radius] != imdiff1->rfile_mask_tag) {
      if ((imdiff2->rfile[radius] != imdiff2->rfile_mask_tag)) {
        imdiff1->rfile[radius]=imdiff1->rfile[radius] * imdiff2->rfile[radius];
      }
    }
    else {
      imdiff1->rfile[radius] = imdiff1->rfile_mask_tag;
    }
  }
  return(return_value);
}
