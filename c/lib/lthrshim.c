/* LTHRSHIM.C - Set all pixels above and below given thresholds equal to an 
		ignore_tag.
   
   Author: Mike Wall
   Date: 5/13/94
   Version: 1.
   
   */

#include<mwmask.h>

int lthrshim(DIFFIMAGE *imdiff)
{
	size_t 
		index;

	int 
		return_value = 0;

  for(index=0; index < imdiff->image_length; index++) {
    if (imdiff->image[index] - imdiff->value_offset < imdiff->lower_threshold) {
      imdiff->image[index] = imdiff->ignore_tag;
    }
    if (imdiff->image[index] - imdiff->value_offset > imdiff->upper_threshold) {
      imdiff->image[index] = imdiff->overload_tag;
    }
  }
  return(return_value);
}
