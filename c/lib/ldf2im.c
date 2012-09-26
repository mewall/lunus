/* LDF2IM.C - Generate an image based upon a diffuse features file.
                
   Author: Mike Wall  
   Date: 5/30/94
   Version: 1.0
   
*/

#include<mwmask.h>

int ldf2im(DIFFIMAGE *imdiff)
{
  size_t
    i,
    j,
    index;

  int
    return_value = 0;

  RCCOORDS_DATA
    r,
    c;
  
  for (j=0; j<imdiff->feature_count; j++) {
    imdiff->pos.r = imdiff->feature[j].pixel_pos.r;
    imdiff->pos.c = imdiff->feature[j].pixel_pos.c;
    imdiff->mask_outer_radius = (short)imdiff->feature[j].radius;
    imdiff->mask_inner_radius = 0;
    lgetanls(imdiff);
    for (i=0; i<imdiff->mask_count; i++) {
      r = imdiff->pos.r+imdiff->mask[i].r;
      c = imdiff->pos.c+imdiff->mask[i].c;
      if (!((r < 0) || (r > imdiff->vpixels) || (c < 0) ||       
	    (c > imdiff->hpixels))) {
	index = r*imdiff->hpixels + c;
/*	if (imdiff->image[index] != imdiff->ignore_tag) {*/
	  imdiff->image[index] =
	    (IMAGE_DATA_TYPE)imdiff->feature[j].average_value; 
/*	}*/
      }
    }
  }  
  return(return_value);
}


