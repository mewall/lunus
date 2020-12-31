/* LMULCFIM.C - Multiply correction factors by an image pixel-by-pixel

   Author: Mike Wall
   Version:1.0
   Date:6/6/2017
			

*/

#include<mwmask.h>

int lmulcfim(DIFFIMAGE *imdiff_in)
{
  size_t
    i,j,n,m,ct,r,c,index;

  int
    return_value = 0;

  float sum;

  int pidx;

  DIFFIMAGE *imdiff;

  for (pidx = 0; pidx < imdiff_in->num_panels; pidx++) {
    imdiff = &imdiff_in[pidx];
    index = 0;

    float *image_float;
    image_float = (float *)calloc(imdiff->image_length,sizeof(float));

    if (imdiff->slist == NULL) lslistim(imdiff);

    struct xyzcoords s;
    for (j = 0;j<imdiff->vpixels;j++) {
      for (i = 0;i<imdiff->hpixels;i++) {
	index = j*imdiff->hpixels+i;
	if ((imdiff->image[index] != imdiff->overload_tag) &&
	    (imdiff->image[index] != imdiff->ignore_tag)) {
	  //	      imdiff->image[index] = (imdiff->image[index]-imdiff->value_offset)*imdiff->correction[index]+imdiff->value_offset;
	  /*
	  if (imdiff->image[index] != 0) {
	    imdiff->image[index] = (imdiff->image[index]-imdiff->value_offset)*imdiff->correction[index]+imdiff->value_offset;
	  } else {
	    size_t ct = 0;
	    sum=0.0;
	    for (n=-1;n<=1;n++) {
	      r = j + n;
	      for (m=-1;m<=1;m++) {
		c = i + m;
		if (!((r < 0) || (r > imdiff->vpixels) || (c < 0) ||
		      (c > imdiff->hpixels))) {
		  size_t imd_index;
		  imd_index = index + n*imdiff->hpixels + m;
		  if ((imdiff->image[imd_index] != imdiff->overload_tag) &&
		      (imdiff->image[imd_index] != imdiff->ignore_tag)) {
		    sum += (float)(imdiff->image[imd_index]-imdiff->value_offset);
		    ct++;
		  }
		}
	      }
	    }
	    if (ct>0) {
	      imdiff->image[index] = (IMAGE_DATA_TYPE)(sum*imdiff->correction[index]/(float)ct) + imdiff->value_offset;
	    }
	  }
	  if (imdiff->image[index] == 0) {imdiff->image[index] = imdiff->ignore_tag;}
	  */
	  image_float[index] = ((float)(imdiff->image[index]-imdiff->value_offset) - imdiff->correction_offset)*imdiff->correction[index];
	  
	}
      }
    } 
      IMAGE_DATA_TYPE max=-32766,min=32766;
      for (i = 0;i<imdiff->image_length;i++) {
	if (image_float[i] >= (float)MAX_IMAGE_DATA_VALUE) {
	  imdiff->image[i] = imdiff->ignore_tag;
	  ct++;
	} else {
	  if ((IMAGE_DATA_TYPE)image_float[i] > max) max = (IMAGE_DATA_TYPE)image_float[i];
	  if ((IMAGE_DATA_TYPE)image_float[i] < min) min = (IMAGE_DATA_TYPE)image_float[i];
	}
      }
      if (min<0) {
	imdiff->value_offset = -min;
      } else {
	imdiff->value_offset = 0;
      }
      for (i = 0;i<imdiff->image_length;i++) {
	if (image_float[i] < (float)MAX_IMAGE_DATA_VALUE) {
	  imdiff->image[i] = (IMAGE_DATA_TYPE)image_float[i] + imdiff->value_offset;
	}
      }

  free(image_float);
  }

  return(return_value);

}
