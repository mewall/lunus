/* LSUMRESIM.C - Calculate the total intensity within given resolution limits 
 * for an input image. Subtracts out 1000 per pixel before printing!!

Author: Veronica Pillar
Date: 6/13/14
Version: 1.

*/

#include<mwmask.h>

int lsumresim(DIFFIMAGE *imdiff, double minres, double maxres)
{
  size_t
    num,
    index = 0;

  int
    sum;

  RCCOORDS_DATA
    r,
    c;

  struct xycoords rvec;

  double
    minrad,
    maxrad,
    radius;

  minrad = imdiff->distance_mm*tan(2*asin(imdiff->wavelength/(2*minres)));
  maxrad = imdiff->distance_mm*tan(2*asin(imdiff->wavelength/(2*maxres)));
  minrad = minrad/imdiff->pixel_size_mm;
  maxrad = maxrad/imdiff->pixel_size_mm;

  sum = 0;
  num = 0;
  for(r = 0; r < imdiff->vpixels; r++) {
    rvec.y = (XYZCOORDS_DATA)(r - imdiff->origin.r);
    for(c = 0; c < imdiff->hpixels; c++) {
      rvec.x = (XYZCOORDS_DATA)(c - imdiff->origin.c);
      radius = (double)sqrt(rvec.y*rvec.y + rvec.x*rvec.x);
      if ((radius >= minrad) && (radius <= maxrad)) {
	if ((imdiff->image[index] != imdiff->overload_tag) &&
	    (imdiff->image[index] != imdiff->ignore_tag)) {
	  sum += (imdiff->image[index]-1000);
	}
      }
      index++;
    }
  }
  printf("%d\n", sum);
}
