/* LPUNCHIM.C - Mark all pixels inside a window in an image with an ignore tag.
   
   Author: Mike Wall  
   Date: 5/12/94
   Version: 1.
   
   */

#include<mwmask.h>

int lpunchim(DIFFIMAGE *imdiff_in)
{
  size_t 
    index = 0;

  RCCOORDS_DATA
    r,
    c;

  int 
    return_value = 0;

  int pidx;

  DIFFIMAGE *imdiff;

  for (pidx = 0; pidx < imdiff_in->num_panels; pidx++) {
    imdiff = &imdiff_in[pidx];
    index = 0;

    struct xyzcoords s;

    XYZCOORDS_DATA rr, cos_two_theta, ssq;

    struct xycoords rvec;

    struct rccoords pixel;

    for(r=0; r < imdiff->vpixels; r++) {
      for(c=0; c < imdiff->hpixels; c++) {
	if ((r > imdiff->punchim_lower.r) && 
	    (r < imdiff->punchim_upper.r) && 
	    (c > imdiff->punchim_lower.c) && 
	    (c < imdiff->punchim_upper.c)) {
	  imdiff->image[index] = imdiff->ignore_tag;
	}
	index++;
      }
    }
  }
  return(return_value);
}
