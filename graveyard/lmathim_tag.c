/* LMATHIM.C - Calculate aX + bY (a & b numbers, X & Y images), storing the result in image X.

    Author: Mike Wall (lsumim.c)
    Version:1.0
    Date:4/6/95

    Modified by: Veronica Pillar
    Version: 3.0
    Date: 1/4/16

    Input argument is two diffraction images.  Output sum is
    stored in imdiff1->image.

*/

#include<mwmask.h>

int lmathim(DIFFIMAGE *imdiff1, double a, DIFFIMAGE *imdiff2, double b)
{
    RCCOORDS_DATA
	r1,
	r2,
	c1,
	c2;

    size_t
	index,
	radius,
	index1 = 0,
	index2 = 0,
	tag;

    struct rccoords 
	rvec;

    int 
	return_value = 0;

    IMAGE_DATA_TYPE
	val1,
	val2,
	pedestal = imdiff1->value_offset;	

    if (imdiff2 == NULL) {
	for(index=0; index < imdiff1->image_length; index++) {
	    imdiff1->image[index] = (IMAGE_DATA_TYPE)(a*(double)imdiff1->image[index]);
	}
	return(return_value);
    }

    if (imdiff2->value_offset > pedestal)
	pedestal = imdiff2->value_offset;

    tag = imdiff1->tag;

    for(r1 = 0; r1 < imdiff1->vpixels; r1++) {
	rvec.r = r1 - imdiff1->origin.r;
	r2 = (rvec.r + imdiff2->origin.r);
	for(c1 = 0; c1 < imdiff1->hpixels; c1++) {
	    rvec.c = c1 - imdiff1->origin.c;
	    if ((imdiff1->image[index1] != imdiff1->overload_tag) &&
		    (imdiff1->image[index1] != imdiff1->ignore_tag)) {
		c2 = (rvec.c + imdiff2->origin.c);
		val1 = imdiff1->image[index1] - imdiff1->value_offset;
		if ((r2 > 0) && (r2 < imdiff2->vpixels) && (c2 > 0) && 
			(c2 < imdiff2->hpixels)) {
		    index2 = r2*imdiff2->hpixels + c2;
		    if ((imdiff2->image[index2] != imdiff2->overload_tag) &&
			(imdiff2->image[index2] != imdiff2->ignore_tag )) {
			val2 = imdiff2->image[index2] - imdiff2->value_offset;
			imdiff1->image[index1] = (IMAGE_DATA_TYPE)
			    (a*(double)val1 +
				b*(double)val2 + (double)pedestal);
		    }
		    else { // if image 2 is punched here
			if(tag) {
			    imdiff1->image[index1] = (IMAGE_DATA_TYPE)
				(2*a*(double)val1 + (double)pedestal);
			}
			else {
			    imdiff1->image[index1] = imdiff1->ignore_tag;
			}
		    }
		}
		else {
		    imdiff1->image[index1] = imdiff1->ignore_tag;
		    // we get here if image 1 is not punched, but image 2 is out of bounds. usually we'll never get here.
		    // let's still not write a value here, bc if we were keeping this area then why wouldn't you keep the area in image 2 but not image 1? but that would take a lot more coding to set up.
		}
	    }
	    else if(tag) { // if tag, we want to check for a value in image 2
		c2 = (rvec.c + imdiff2->origin.c);
		if ((r2 > 0) && (r2 < imdiff2->vpixels) && (c2 > 0) && 
			(c2 < imdiff2->hpixels)) {
		    index2 = r2*imdiff2->hpixels + c2;
		    if ((imdiff2->image[index2] != imdiff2->overload_tag) &&
			(imdiff2->image[index2] != imdiff2->ignore_tag )) {
			val2 = imdiff2->image[index2] - imdiff2->value_offset;
			imdiff1->image[index1] = (IMAGE_DATA_TYPE)
			    (2*b*(double)val2 + (double)pedestal);
		    }
		}
	    }
	    index1++;
	}
    }
    return(return_value);

}
