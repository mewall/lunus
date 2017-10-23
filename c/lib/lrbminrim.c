/* LRBMINRIM.C - Calculate the minimum intensity as a function of radius for an
   input image, with rolling-box averaging first to smooth the result.

Author: Veronica Pillar   
Date: 1/2/14
Version: 1.

*/

#include<mwmask.h>

int lrbminrim(DIFFIMAGE *imdiff, int width)
{
  size_t
    radius,
    index = 0,
    ind;

  int
    n,
    i,
    j;

  RCCOORDS_DATA
    r,
    c;

  float
    sum;

  struct xycoords rvec;

  imdiff->rfile_length = 0;
  for(r = 0; r < imdiff->vpixels; r++) {
    for(c = 0; c < imdiff->hpixels; c++) {
      rvec.y = (XYZCOORDS_DATA)(r - imdiff->origin.r);
      rvec.x = (XYZCOORDS_DATA)(c - imdiff->origin.c);
      radius = (size_t)sqrtf(rvec.y*rvec.y + rvec.x*rvec.x);
      if (radius > imdiff->rfile_length) imdiff->rfile_length = radius;
      if ((imdiff->image[index] != imdiff->overload_tag) && 
	  (imdiff->image[index] != imdiff->ignore_tag)) {

	/*
	 * Rolling-box averaging:
	 */
	
	sum = 0;
	n = 0;
	for (i = r-width; i <= r+width; i++) {
	  if ((i < 0) || (i >= imdiff->vpixels))
	    continue;
	  for (j = c-width; j <= c+width; j++) {
	    if ((j < 0) || (j >= imdiff->hpixels))
	      continue;
	    ind = i*imdiff->hpixels + j;

	    if ((imdiff->image[ind] != imdiff->overload_tag ) && 
		(imdiff->image[ind] != imdiff->ignore_tag)) {
	      sum += imdiff->image[ind];
	      n++;
	    }
	  }
	}
	sum = sum/(float)n;

	/*
	 * Identify the minimum:
	 */

	if ((sum > 0) && ((imdiff->rfile[radius] > (RFILE_DATA_TYPE)sum) || (imdiff->rfile[radius] == 0))) {
	  imdiff->rfile[radius] = (RFILE_DATA_TYPE)sum;
	}
      }
      index++;
    }
  }

  //for testing purposes: print r-file
  /* 
     for (i = 700; i < 710; i++) {
     printf("%f\n", imdiff->rfile[i]);
     }*/
}
