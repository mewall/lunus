/* LSUMRIM.C - Calculate the average intensity vs. radius for an
   input image.

Author: Veronica Pillar, based on Mike Wall's lavgrim.c
Date: 2/22/16
Version: 1.

*/

#include<mwmask.h>

int lsumrim(DIFFIMAGE *imdiff)
{
    size_t
	*n,
	radius,
	index = 0;

    RCCOORDS_DATA
	r,
	c;

    struct xycoords rvec;

    //n = (size_t *)calloc(MAX_RFILE_LENGTH, sizeof(size_t));
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
		if ((imdiff->image[index] > imdiff->value_offset) && 
			(imdiff->image[index] < 60000)) {
		imdiff->rfile[radius] = (RFILE_DATA_TYPE)
		    ((float)(imdiff->image[index]-imdiff->value_offset)+ 
		     (float)imdiff->rfile[radius]);
		}
		//if (imdiff->image[index] > 60000) {
		//    printf("large pixel at %d, %d,",r,c);
		//}
		//n[radius]++;
	    }
	    index++;
	}
    }
    //free((size_t *)n);
}
