/* LNOISEIM - Create a noisy image.
   
   Author: Mike Wall
   Date: 4/10/95
   Version: 1.
   
   */

#include<mwmask.h>

int lnoiseim(DIFFIMAGE *imdiff)
{
  size_t 
    i,
    j,
    index = 0;
  
  int 
    return_value = 0;
  
  IMAGE_DATA_TYPE 
    image_value;

  long
    peak_to_peak;
  
  peak_to_peak = 2*imdiff->amplitude;
  for(j=0; j < imdiff->vpixels; j++) {
    for(i=0; i < imdiff->hpixels; i++) {
      imdiff->image[index] =
	(IMAGE_DATA_TYPE)(random() % peak_to_peak - imdiff->amplitude); 
      index++;
    }
  }
  return(return_value);
}






