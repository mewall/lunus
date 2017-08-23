/* LGETRIM.C - Print all intensity values for a given radius, in arbitrary order.

Author: Veronica Pillar
Date: 4/21/14
Version: 1.

*/

#include<mwmask.h>

int lgetrim(DIFFIMAGE *imdiff, size_t target)
{
  size_t
    radius,
    index = 0;

  RCCOORDS_DATA
    r,
    c;

  struct xycoords rvec;

  for(r = 0; r < imdiff->vpixels; r++) {
    rvec.y = (XYZCOORDS_DATA)(r - imdiff->origin.r);
    for(c = 0; c < imdiff->hpixels; c++) {
      rvec.x = (XYZCOORDS_DATA)(c - imdiff->origin.c);
      radius = (size_t)sqrtf(rvec.y*rvec.y + rvec.x*rvec.x);
      if (radius == target) {
	if ((imdiff->image[index] != imdiff->overload_tag) &&
	    (imdiff->image[index] != imdiff->ignore_tag)) {
	  printf("%f\n", (float)imdiff->image[index]);
	}
      }
      index++;
    }
  }
}
