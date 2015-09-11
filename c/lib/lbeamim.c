/* LBEAMIM.C - Create a target pattern around the beam in a diffraction image.
   
   Author: Mike Wall
   Date: 2/17/2014
   Version: 1.
   
   */

#include<mwmask.h>

int lbeamim(DIFFIMAGE *imdiff)
{
	size_t
		r,
		c,
		radius,
		index = 0;

	struct xycoords rvec;

	int
		return_value;

// Create a pattern of concenric rings around the beam position

	for(r = 0; r < imdiff->vpixels; r++) {
	  rvec.y = r*imdiff->pixel_size_mm - imdiff->beam_mm.y;
	  for(c = 0; c < imdiff->hpixels; c++) {
	      rvec.x = c*imdiff->pixel_size_mm - imdiff->beam_mm.x;
	      radius = (size_t)(sqrtf(rvec.x*rvec.x + rvec.y*rvec.y)/imdiff->pixel_size_mm+.5);
		if (radius % 100 == 0) {
			imdiff->image[index] = imdiff->ignore_tag;
		} 
	    index++;
	  }
	}
// Create a cross pattern centered on the beam position

	c = imdiff->beam_mm.x/imdiff->pixel_size_mm+.5;
	for (r = 0; r < imdiff->vpixels; r++) {
		index = r*imdiff->hpixels+c;
		imdiff->image[index] = imdiff->ignore_tag;
	}
	r = imdiff->beam_mm.y/imdiff->pixel_size_mm+.5;
	for (c = 0; c < imdiff->hpixels; c++) {
		index = r*imdiff->hpixels+c;
		imdiff->image[index] = imdiff->ignore_tag;
	}
	return(return_value);
}


