/* LLOWRIM.C - Calculate the Xth lowest intensity as a function of radius for an
   input image, with rolling-box averaging first to smooth the result.

Author: Veronica Pillar   
Date: 1/2/14
Version: 1.

*/

#include<mwmask.h>

int llowrim(DIFFIMAGE *imdiff, int X, int width)
{
  size_t
    radius,
    index = 0,
    ind;

  int
    n,
    i,
    j,
    k;

  RCCOORDS_DATA
    r,
    c;

  float
    sum;

  struct xycoords rvec;

  RFILE_DATA_TYPE
    **mins;

  RFILE_DATA_TYPE
    temp;

  /*
   * Allocate minimum arrays:
   */

  mins = (RFILE_DATA_TYPE**)calloc(MAX_RFILE_LENGTH, sizeof(RFILE_DATA_TYPE*));
  for (r = 0; r < MAX_RFILE_LENGTH; r++) {
    mins[r] = (RFILE_DATA_TYPE*)calloc(X, sizeof(RFILE_DATA_TYPE));
  }

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
	 * Identify the lowest X:
	 */

	if (sum > 0) {
	  
	  i = 0;
	  while (mins[radius][i] > 0) {
	    i++;
	    if (i == X)
	      break;
	  }
	  if (i < X) {
	    mins[radius][i] = sum;
	  }
	  else {
	    k = 0;
	    for (j = 0; j < X; j++) {
	      if (mins[radius][j] > sum) {
		if (mins[radius][j] > mins[radius][k])
		  k = j;
	      }
	    }
	    if (mins[radius][k] > sum) {
	      mins[radius][k] = sum;
	    }
	  }
	}
      }
      index++;
    }
  }


  /*
   * Set r-file values:
   */

  for (i = 0; i < imdiff->rfile_length; i++) {
    temp = 0;
    for (j = 0; j < X; j++) {
      if (mins[i][j] > temp)
	temp = mins[i][j];
    }
    imdiff->rfile[i] = temp;
  }

  //for testing purposes: print r-file
  /* 
     for (i = 700; i < 710; i++) {
     printf("%f\n", imdiff->rfile[i]);
     }*/
}
