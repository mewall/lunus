/* LCONSTIM - Create an image with constant pixel value based on an input
		image format.
   
   Author: Mike Wall
   Date: 4/18/94
   Version: 1.
   
   */

#include<mwmask.h>

int lconstim(DIFFIMAGE *imdiff)
{
	size_t 
		index;

	int 
		return_value = 0;

	IMAGE_DATA_TYPE 
		image_value;
 
  image_value = imdiff->image[0];
  for(index=0; index < imdiff->image_length; index++) {
    imdiff->image[index] = image_value;
  }
  return(return_value);
}
