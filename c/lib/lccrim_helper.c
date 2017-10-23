/* LCCRIM_HELPER.C - Calculate the sums necessary for a Pearson correlation 
 * coefficient between two images in different resolution shells.

Author: Veronica Pillar (modified from Mike Wall's codes)
Version:1.0
Date: 5/26/15

Input argument is two diffraction images and the 6 arrays of sums. The arrays
get updated by this method.

NB: each chunk is defined as located where its center is, even if so many pixels
are out of range or in the module gaps that the center of the usable pixels is 
elsewhere. I am assuming that this will not significantly affect calculations.

*/

#include<mwmask.h>

int lccrim_helper(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2, double **sums_all)
{
    int 
	return_value = 0;

    double
	*n,
	*sum_x,
	*sum_y,
	*sum_xx,
	*sum_yy,
	*sum_xy;

    RCCOORDS_DATA
	r1,
	r2,
	c1,
	c2,
	rbox,
	cbox;

    struct rccoords
	rvec;

    size_t
	bindex,
	index1 = 0,
	index2 = 0,
	nchunk1,
	nchunk2,
	vchunk1,
	vchunk2,
	box_size = imdiff1->mode_width;

    double
	val1,
	val2,
	radius,
	rdist,
	cdist;

    n = sums_all[0];
    sum_x = sums_all[1];
    sum_y = sums_all[2];
    sum_xx = sums_all[3];
    sum_yy = sums_all[4];
    sum_xy = sums_all[5];

    for(r1 = 0; r1 < imdiff1->vpixels; r1 += box_size) {
	rvec.r = r1 - imdiff1->origin.r;
	r2 = (rvec.r + imdiff2->origin.r);
	for(c1 = 0; c1 < imdiff1->hpixels; c1 += box_size) {
	    rvec.c = c1 - imdiff1->origin.c;
	    c2 = (rvec.c + imdiff2->origin.c);

	    // Initialize chunk variables
	    vchunk1 = 0;
	    vchunk2 = 0;
	    nchunk1 = 0;
	    nchunk2 = 0;

	    // Loop over pixels within chunk
	    for(rbox = 0; rbox < box_size; rbox++) {
		for (cbox = 0; cbox < box_size; cbox++) {
		    if ((r1 + rbox > 0) && (r1 + rbox < imdiff1->vpixels) &&
			    (c1 + cbox > 0) && (c1 + cbox < imdiff1->hpixels)) {
			index1 = (r1 + rbox)*imdiff1->hpixels + (c1 + cbox);
			if ((imdiff1->image[index1] != imdiff1->overload_tag) &&
				(imdiff1->image[index1] != imdiff1->ignore_tag)) { 

			    // add the index1 pixel into the average
			    vchunk1 += imdiff1->image[index1];
			    nchunk1++;
			}
		    }

		    if ((r2 + rbox > 0) && (r2 + rbox < imdiff2->vpixels) &&
			    (c2 + cbox > 0) && (c2 + cbox < imdiff2->hpixels)) {
			index2 = (r2 + rbox)*imdiff2->hpixels + (c2 + cbox);
			if ((imdiff2->image[index2] != imdiff2->overload_tag) &&
				(imdiff2->image[index2] != imdiff2->ignore_tag)) {

			    // add the index2 pixel into the average
			    vchunk2 += imdiff2->image[index2];
			    nchunk2++;

			}
		    }
		}
	    }

	    // Calculate radius and appropriate values using chunk averages.
	    if ((nchunk1 > 0) && (nchunk2 > 0)) {
		val1 = (double)vchunk1/(double)nchunk1;
		val2 = (double)vchunk2/(double)nchunk2;
		rdist = (rvec.r + (double)box_size/2.0 - 0.5);
		cdist = (rvec.c + (double)box_size/2.0 - 0.5);
		radius = sqrt(rdist*rdist + cdist*cdist);
		bindex = (int)floor(radius/50); //which bin this radius goes in
		if (bindex < 25) {
		    n[bindex]++;
		    sum_x[bindex] += val1;
		    sum_y[bindex] += val2;
		    sum_xx[bindex] += val1*val1;
		    sum_yy[bindex] += val2*val2;
		    sum_xy[bindex] += val1*val2;
		}
	    }

	    // finished with this chunk
	}
    }

    return(return_value);
}

