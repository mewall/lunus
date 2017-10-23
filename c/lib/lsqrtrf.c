/* LSQRTRF.C - Take the square root of an rfile.
   
   Author: Veronica Pillar (based off Mike Wall's lmulrf.c)
   Date: 9/25/15
   Version: 1.
   
   */

#include<mwmask.h>

int lsqrtrf(DIFFIMAGE *imdiff1)
{
	size_t
		radius;

	int
		return_value = 0;

  for(radius = 0; radius < imdiff1->rfile_length; radius++) {
    if (imdiff1->rfile[radius] != imdiff1->rfile_mask_tag) {
      imdiff1->rfile[radius]=sqrt(imdiff1->rfile[radius]);
    }
    else {
      imdiff1->rfile[radius] = imdiff1->rfile_mask_tag;
    }
  }
  return(return_value);
}
