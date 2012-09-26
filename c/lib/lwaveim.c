/* LWAVEIM - Create an image with a fixed length scale fluctuation.
   
   Author: Mike Wall
   Date: 4/6/95
   Version: 1.
   
   */

#include<mwmask.h>

int lwaveim(DIFFIMAGE *imdiff)
{
  size_t 
    i,
    j,
    index = 0;
  
  int 
    return_value = 0;
  
  IMAGE_DATA_TYPE 
    image_value;

  float
    x_factor,
    y_factor;
  
  x_factor = 2*PI/imdiff->pitch;
  y_factor = 2*PI/imdiff->pitch;
  for(j=0; j < imdiff->vpixels; j++) {
    for(i=0; i < imdiff->hpixels; i++) {
      imdiff->image[index] =
	(IMAGE_DATA_TYPE)imdiff->amplitude*(sinf(x_factor*(float)i)
					    *sinf(y_factor*(float)j));
      index++;
    }
  }
  return(return_value);
}





