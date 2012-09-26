/* LINTDFIM.C - Integrate diffuse features in a diffraction image and
                output to a file.
   
   Author: Mike Wall  
   Date: 5/28/94
   Version: 1.0
   
*/

#include<mwmask.h>

int lintdfim(DIFFIMAGE *imdiff)
{
  size_t
    i,
    j,
    index;

  long
    sum,
    average_count;

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
    sum = 0;
    average_count = 0;
    for (i=0; i<imdiff->mask_count; i++) {
      r = imdiff->pos.r+imdiff->mask[i].r;
      c = imdiff->pos.c+imdiff->mask[i].c;
      if (!((r < 0) || (r > imdiff->vpixels) || (c < 0) ||       
	    (c > imdiff->hpixels))) {
	index = r*imdiff->hpixels + c;
	if (imdiff->image[index] != imdiff->ignore_tag) {
	  average_count++;
	  sum += imdiff->image[index];
	}
      }
    }
    if (average_count != 0) {
      imdiff->feature[j].average_value = sum/average_count;
      imdiff->feature[j].peak_value = (long)imdiff->mask_count*
	(long)imdiff->feature[j].average_value;
    } else {
      return_value = 1;
      sprintf(imdiff->error_msg,"LINTDFIM: No integratable intensity "
	      "in feature number %ld.\n",j);
    }
  }  
  return(return_value);
}


