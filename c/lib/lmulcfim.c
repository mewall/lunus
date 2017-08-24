/* LMULCFIM.C - Multiply correction factors by an image pixel-by-pixel

   Author: Mike Wall
   Version:1.0
   Date:6/6/2017
			

*/

#include<mwmask.h>

int lmulcfim(DIFFIMAGE *imdiff)
{
	size_t
	  i,j,n,m,ct,r,c,index;

	int 
	  sum,
	  return_value = 0;

	for (j = 0;j<imdiff->vpixels;j++) {
	  for (i = 0;i<imdiff->hpixels;i++) {
	    index = j*imdiff->hpixels+i;
	    if ((imdiff->image[index] != imdiff->overload_tag) &&
		(imdiff->image[index] != imdiff->ignore_tag)) {
	      if (imdiff->image[index] != 0) {
		imdiff->image[index] *= imdiff->correction[index];
	      } else {
		size_t ct = 0;
		IMAGE_DATA_TYPE sum=0;
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
			sum += imdiff->image[imd_index];
			ct++;
		      }
		    }
		  }
		}
		if (ct>0) {
		  imdiff->image[index] = (IMAGE_DATA_TYPE)((float)sum*imdiff->correction[index]/(float)ct);
		}
	      }
	    }
	  }
	}

	return(return_value);

}
