/* LRMPKIM.C - Remove peaks by replacing them with background pixel values..
   
   Author: Mike Wall
   Date: 5/13/93
   Version: 1.
   
   "rmpkim <input file> <image in> <image out> <hsize> <vsize>"

   Input is ascii coordinates file.  Output is 16-bit 
   image of specified size (1024 x 1024 default).

   */

#include<mwmask.h>

int lrmpkim(DIFFIMAGE *imdiff)
{
	int return_value = 0;
	long i,
		sum = 0,
		count = 0;
	short
		r,
		c;
						

	for (i=0; i<imdiff->mask_count; i++) {
		r = imdiff->pos.r+imdiff->mask[i].r;
		c = imdiff->pos.c+imdiff->mask[i].c;
		if (!((r < 0) || (r > imdiff->vpixels) || (c < 0) ||       
			(c > imdiff->hpixels))) {
			sum += imdiff->image[r*imdiff->hpixels+c];
			count++;
		};
	}
	imdiff->mask_inner_radius = 0;
	lgetanls(imdiff);
	imdiff->punch_tag = (IMAGE_DATA_TYPE)sum/count;
	lpunch(imdiff);
	imdiff->mask_inner_radius = imdiff->mask_outer_radius;
	lgetanls(imdiff);
	return(return_value);
}
