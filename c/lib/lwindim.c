/* LWINDIM.C - Mark all pixels outside a window in an image with an ignore tag.
   
   Author: Mike Wall  
   Date: 4/25/94
   Version: 1.
   
   */

#include<mwmask.h>

int lwindim(DIFFIMAGE *imdiff_in)
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
    for(r=0; r < imdiff->vpixels; r++) {
      for(c=0; c < imdiff->hpixels; c++) {
	if ((r < imdiff->window_lower.r) || (r > imdiff->window_upper.r) || 
	    (c < imdiff->window_lower.c) || (c > imdiff->window_upper.c)) {
	  imdiff->image[index] = imdiff->ignore_tag;
	}
	index++;
      }
    }
  }
  return(return_value);
}
