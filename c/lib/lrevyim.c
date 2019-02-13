/* LREVYIM.C - Reverse the y-axis in a diffraction image
   
   Author: Mike Wall
   Date: 2/17/2014
   Version: 1.
   
*/

#include<mwmask.h>

int lrevyim(DIFFIMAGE *imdiff)
{
  size_t
    r,
    c,
    radius,
    index = 0,
    index2;

  struct xycoords rvec;
  
  IMAGE_DATA_TYPE *buf;

  int
    return_value;

  // Create  a buffer for the image

  buf = (IMAGE_DATA_TYPE *)malloc(sizeof(IMAGE_DATA_TYPE)*imdiff->image_length);

  // Copy the image into the buffer

  memcpy((void *)buf,(void *)imdiff->image,sizeof(IMAGE_DATA_TYPE)*imdiff->image_length);

  // Write the reverse image into the original image buffer

  index = 0;
  for(r = 0; r < imdiff->vpixels; r++) {
    for(c = 0; c < imdiff->hpixels; c++) {
      index2 = (imdiff->vpixels-r-1)*imdiff->hpixels+c;
      imdiff->image[index]=buf[index2];
      index++;
    }
  }
  return(return_value);
}

