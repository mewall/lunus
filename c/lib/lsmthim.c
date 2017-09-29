/* LSMTHIM.C - Smooth an image.
   
   Author: Mike Wall
   Date: 4/11/93
   Version: 1.
   
   */

#include<mwmask.h>
int lsmthim(DIFFIMAGE *imdiff) 
{

	WEIGHTS_DATA_TYPE
		*image,
		*weights_sum;

	RCCOORDS_DATA 
		half_height,
		half_width,
		n, 
		m, 
		i, 
		j, 
		r, 
		c; 
	size_t 
		imd_index = 0,
		weights_index = 0,
		index = 0; 

	int
		return_value = 0;

/* 
 * Allocate working image: 
 */ 

  weights_sum = (WEIGHTS_DATA_TYPE *)calloc(imdiff->image_length,
			sizeof(WEIGHTS_DATA_TYPE));
  image = (WEIGHTS_DATA_TYPE *)calloc(imdiff->image_length, 
	sizeof(WEIGHTS_DATA_TYPE)); 

  if (!image || !weights_sum) {
    sprintf(imdiff->error_msg,"\nLSMTHIM:  Couldn't allocate arrays.\n\n");
    return_value = 1;
    goto CloseShop;
  }

  half_height = imdiff->weights_height / 2;
  half_width = imdiff->weights_width / 2;
  printf("%d,%d\n",(int)half_height,(int)half_width);
  for(n=-half_height; n<=half_height; n++) {
    for(m=-half_width; m<=half_width; m++) {
      index = 0;
      for (j=0; j<imdiff->vpixels; j++) {
	r = j + n;
        for(i=0; i<imdiff->hpixels; i++) {
          c = i + m;
          if (!((r < 0) || (r > imdiff->vpixels) || (c < 0) ||       
             (c > imdiff->hpixels) ||
	     (imdiff->image[index] == imdiff->overload_tag) ||
	     (imdiff->image[index] == imdiff->ignore_tag))) {
            imd_index = index + n*imdiff->hpixels + m;
	    if ((imdiff->image[imd_index] != imdiff->overload_tag) &&
	        (imdiff->image[imd_index] != imdiff->ignore_tag)) {
              image[index] += (WEIGHTS_DATA_TYPE)imdiff->image[imd_index] * 
		imdiff->weights[weights_index]; 
              weights_sum[index] += imdiff->weights[weights_index];
            }
          }
	  index++;
        }
      }
      weights_index++;
    }
  }
  for(index=0;index < imdiff->image_length; index++) {
    if (weights_sum[index] != 0) { 
      imdiff->image[index] = (IMAGE_DATA_TYPE)(image[index]/weights_sum[index]);
    }
  }
  free((WEIGHTS_DATA_TYPE *)weights_sum);
  free((WEIGHTS_DATA_TYPE *)image);
CloseShop:
  return(return_value);
}
