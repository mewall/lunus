/* LSCALERF.C - Sum two rfiles.
   
   Author: Veronica Pillar
   Date: 9/26/17
   Version: 1.
   
   */

#include<mwmask.h>

int lscalerf(DIFFIMAGE *imdiff, float scale)
{
	size_t
		radius;

	int
		return_value = 0;

  for(radius = 0; radius < imdiff->rfile_length; radius++) {
    if (imdiff->rfile[radius] != imdiff->rfile_mask_tag) {
        imdiff->rfile[radius]=imdiff->rfile[radius] *= scale;
    }
  }
  return(return_value);
}
