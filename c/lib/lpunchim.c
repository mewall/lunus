/* LPUNCHIM.C - Mark all pixels inside a window in an image with an ignore tag.
   
   Author: Mike Wall  
   Date: 5/12/94
   Version: 1.
   
   */

#include<mwmask.h>

int lpunchim(DIFFIMAGE *imdiff)
{
	size_t 
		index = 0;

	RCCOORDS_DATA
		r,
		c;

	int 
		return_value = 0;

  for(r=0; r < imdiff->vpixels; r++) {
    for(c=0; c < imdiff->hpixels; c++) {
      if ((r > imdiff->punchim_lower.r) && (r < imdiff->punchim_upper.r) && 
	  (c > imdiff->punchim_lower.c) && (c < imdiff->punchim_upper.c)) {
        imdiff->image[index] = imdiff->ignore_tag;
      }
      index++;
    }
  }
  return(return_value);
}
