/* LAVGPOLIM.C - Calculate the average intensity vs. polar angle for an
		input image.
   
   Author: Mike Wall   
   Date: 2/18/2014
   Version: 1.
   
   */

#include<mwmask.h>

int lavgrim(DIFFIMAGE *imdiff)
{
	size_t
		r,
		c,
		*n,
		radius,
		index = 0;

	short angle;

	struct xycoords rvec;

	n = (size_t *)calloc(MAX_RFILE_LENGTH, sizeof(size_t));
	imdiff->rfile_length = 360;
	for(r = 0; r < imdiff->vpixels; r++) {
	  rvec.y = r*imdiff->pixel_size_mm - imdiff->beam_mm.y;
	  for(c = 0; c < imdiff->hpixels; c++) {
	      rvec.x = c*imdiff->pixel_size_mm - imdiff->beam_mm.x;
	      radius = (size_t)(sqrtf(rvec.x*rvec.x + rvec.y*rvec.y)/imdiff->pixel_size_mm+.5);
	      if (radius >= imdiff->mask_inner_radius && radius <= imdiff->mask_outer_radius) {
	      angle = (short)(acosf(((float)rvec.x)/((float)radius*imdiff->pixel_size_mm))*180./PI);
	      if (rvec.y<0) angle += 180;
	      if ((imdiff->image[index] != imdiff->overload_tag) &&
		  (imdiff->image[index] != imdiff->ignore_tag)) {
		imdiff->rfile[angle] = (RFILE_DATA_TYPE)
		  ((float)(imdiff->image[index]-imdiff->value_offset)+ 
				(float)n[angle]*(float)imdiff->rfile[angle]) /
				(RFILE_DATA_TYPE)(n[angle] + 1);
		n[angle]++;
	      }
	      }
	    index++;
	  }
	}
	free((size_t *)n);
}
