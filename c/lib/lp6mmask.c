/* LP6MMASK.C - Mark all pixels on or adjacent to a Pilatus 6M chip boundary with an ignore tag.
   
   Author: Veronica Pillar  
   Date: 5/16/16
   Version: 1.
   
   */

#include<mwmask.h>

int lp6mmask(DIFFIMAGE *imdiff)
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
      if (((r+211) % 212 > 192 ) || ((c+493) % 494 > 484)) {
        imdiff->image[index] = imdiff->ignore_tag;
      }
      index++;
    }
  }
  return(return_value);
}
