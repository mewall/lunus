/* LTAGIM.C - Tag pixels of a specified value with an ignore tag.
   
   Author: Mike Wall
   Date: 4/18/94
   Version: 1.
   
   */

#include<mwmask.h>

int ltagim(DIFFIMAGE *imdiff)
{
	size_t 
		index;

	int 
		return_value = 0;

  for(index=0; index < imdiff->image_length; index++) {
    if (imdiff->image[index] == imdiff->mask_tag) {
      imdiff->image[index] = imdiff->ignore_tag;
    }
  }
  return(return_value);
}
