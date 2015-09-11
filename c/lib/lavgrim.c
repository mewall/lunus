/* LAVGRIM.C - Calculate the average intensity vs. radius for an
   input image.

Author: Mike Wall   
Date: 4/3/93
Version: 1.

Edited by Veronica Pillar
Date: 4/21/14
Version: 2.

*/

#include<mwmask.h>

int lavgrim(DIFFIMAGE *imdiff)
{
<<<<<<< HEAD
  size_t
    *n,
    radius,
    index = 0;

  RCCOORDS_DATA
    r,
    c;

  struct xycoords rvec;

  //temporary edit!!
  //float angle;

  n = (size_t *)calloc(MAX_RFILE_LENGTH, sizeof(size_t));
  imdiff->rfile_length = 0;
  for(r = 0; r < imdiff->vpixels; r++) {
    //rvec.y = r*imdiff->pixel_size_mm - imdiff->beam_mm.y;
    rvec.y = (XYZCOORDS_DATA)(r - imdiff->origin.r);
    for(c = 0; c < imdiff->hpixels; c++) {
      //rvec.x = c*imdiff->pixel_size_mm - imdiff->beam_mm.x;
      rvec.x = (XYZCOORDS_DATA)(c - imdiff->origin.c);
      //radius = (size_t)(sqrtf(rvec.x*rvec.x + rvec.y*rvec.y)/imdiff->pixel_size_mm+.5);
      radius = (size_t)sqrtf(rvec.y*rvec.y + rvec.x*rvec.x);
      if (radius > imdiff->rfile_length) 
	imdiff->rfile_length = radius;
      if (radius > MAX_RFILE_LENGTH)
	  printf("alert! alert!\n");

      //temporary edit!! for vertical signal
      /*
      angle = atan(rvec.y/rvec.x);
      if ((angle < -1.833) || (angle > 1.833)) {
	index++;
	continue;
      }
      if ((angle > -1.309) && (angle < 1.309)) {
	index++;
	continue;
      }
      */

      //temporary edit!! for horizontal signal
     /* 
      angle = atan(rvec.y/rvec.x);
      if ((angle > -2.880) && (angle < -0.262)) {
	index++;
	continue;
      }
      if ((angle > 0.262) && (angle < 2.880)) {
	index++;
	continue;
      }
      
*/

      if ((imdiff->image[index] != imdiff->overload_tag) &&
	  (imdiff->image[index] != imdiff->ignore_tag)) {
	imdiff->rfile[radius] = (RFILE_DATA_TYPE)
	  ((float)(imdiff->image[index]-imdiff->value_offset)+ 
	   (float)n[radius]*(float)imdiff->rfile[radius]) /
	  (RFILE_DATA_TYPE)(n[radius] + 1);
	n[radius]++;
      }
      index++;
    }
  }
  free((size_t *)n);
=======
	size_t
		r,
		c,
		*n,
		radius,
		index = 0;

	struct xycoords rvec;

	n = (size_t *)calloc(MAX_RFILE_LENGTH, sizeof(size_t));
	imdiff->rfile_length = 0;
	for(r = 0; r < imdiff->vpixels; r++) {
	  rvec.y = r*imdiff->pixel_size_mm - imdiff->beam_mm.y;
	  for(c = 0; c < imdiff->hpixels; c++) {
	      rvec.x = c*imdiff->pixel_size_mm - imdiff->beam_mm.x;
	      radius = (size_t)(sqrtf(rvec.x*rvec.x + rvec.y*rvec.y)/imdiff->pixel_size_mm+.5);
	      if ((imdiff->image[index] != imdiff->overload_tag) &&
		  (imdiff->image[index] != imdiff->ignore_tag)) {
	        if (radius > imdiff->rfile_length) 
			imdiff->rfile_length = radius;
		imdiff->rfile[radius] = (RFILE_DATA_TYPE)
		  ((float)(imdiff->image[index]-imdiff->value_offset)+ 
				(float)n[radius]*(float)imdiff->rfile[radius]) /
				(RFILE_DATA_TYPE)(n[radius] + 1);
		n[radius]++;
	      }
	    index++;
	  }
	}
>>>>>>> v0.1a
}
