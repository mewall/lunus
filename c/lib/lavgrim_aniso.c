/* LAVGRIM_ANISO.C - Calculate the average abs(intensity-pedestal) vs radius for an input image.

Edited by Veronica Pillar
Date: 2/28/16
Version: 1.

*/

#include<mwmask.h>

int lavgrim_aniso(DIFFIMAGE *imdiff)
{
  size_t
    *n,
    radius,
    index = 0;

  RCCOORDS_DATA
    r,
    c;

  struct xycoords rvec;

  n = (size_t *)calloc(MAX_RFILE_LENGTH, sizeof(size_t));
  imdiff->rfile_length = 0;
  for(r = 0; r < imdiff->vpixels; r++) {
    rvec.y = (XYZCOORDS_DATA)(r - imdiff->origin.r);
    for(c = 0; c < imdiff->hpixels; c++) {
      rvec.x = (XYZCOORDS_DATA)(c - imdiff->origin.c);
      radius = (size_t)sqrtf(rvec.y*rvec.y + rvec.x*rvec.x);
      if (radius > imdiff->rfile_length) 
	imdiff->rfile_length = radius;
      if (radius > MAX_RFILE_LENGTH)
	  printf("alert! alert!\n");

      if ((imdiff->image[index] != imdiff->overload_tag) &&
	  (imdiff->image[index] != imdiff->ignore_tag)) {
	imdiff->rfile[radius] = (RFILE_DATA_TYPE)
	  ((float)(abs(imdiff->image[index]-imdiff->value_offset))+ 
	   (float)n[radius]*(float)imdiff->rfile[radius]) /
	  (RFILE_DATA_TYPE)(n[radius] + 1);
	n[radius]++;
      }
      index++;
    }
  }
  free((size_t *)n);
}
