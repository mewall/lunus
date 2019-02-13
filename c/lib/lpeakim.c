/* LPEAKIM - Create an image with various size and strength peaks.
   
   Author: Mike Wall
   Date: 4/6/95
   Version: 1.
   
   */

#include<mwmask.h>

int lpeakim(DIFFIMAGE *imdiff)
{
  size_t
    i,
    j,
    index = 0;
  
  int 
    return_value = 0;
  
  float
    image_value,
    d_squared,
    width,
    normalization,
    amplitude;
  
  struct rccoords
    peak_count,
    peak_index;

  peak_count.r = imdiff->vpixels / imdiff->pitch;
  peak_count.c = imdiff->hpixels / imdiff->pitch;
  for(j=0; j < imdiff->vpixels; j++) {
    peak_index.r = j / imdiff->pitch;
    width = 0.1 + (float)((float)imdiff->pitch/5. - 0.1)*
      ((float)peak_index.r/
       (float)peak_count.r);
    d_squared = ((float)j-(float)peak_index.r*(float)(imdiff->pitch) -
		 (float)(imdiff->pitch/2.))* 
		  ((float)j-(float)peak_index.r*(float)(imdiff->pitch) -
		 (float)(imdiff->pitch/2.)) +
		   ((float)i-(float)peak_index.c*(float)imdiff->pitch -
		    (float)imdiff->pitch/2.)*
		      ((float)i-(float)peak_index.c*(float)imdiff->pitch -
		    (float)imdiff->pitch/2.);
    normalization = 1./sqrt(2*PI)/width;
    for(i=0; i < imdiff->hpixels; i++) {
      peak_index.c = i / imdiff->pitch;
      d_squared = ((float)j-(float)peak_index.r*(float)(imdiff->pitch) -
		   (float)(imdiff->pitch/2.))* 
		     ((float)j-(float)peak_index.r*(float)(imdiff->pitch) -
		      (float)(imdiff->pitch/2.)) +
			((float)i-(float)peak_index.c*(float)imdiff->pitch -
			 (float)imdiff->pitch/2.)*
			   ((float)i-(float)peak_index.c*(float)imdiff->pitch -
			    (float)imdiff->pitch/2.);
      amplitude = (float)imdiff->amplitude*((float)peak_index.c /
					    (float)peak_count.c);
      image_value = (amplitude*normalization*
			    (expf(-d_squared/2./width/width)));
      if ((image_value < 32767) && (image_value > -32768)) {
	imdiff->image[index] =
	  (IMAGE_DATA_TYPE)image_value;
      } 
      else {
	imdiff->image[index] = imdiff->ignore_tag;
      }
      index++;
    }
  }
  return(return_value);
}

