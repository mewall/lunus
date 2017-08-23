/* LCCRIM.C - Calculate the Pearson correlation coefficient between two images in 
 * different resolution shells.

Author: Veronica Pillar (modified from Mike Wall's codes)
Version:1.0
Date: 4/13/15

Input argument is two diffraction images.  Output is sent to stdout.

NB: each chunk is defined as located where its center is, even if so many pixels are
out of range or in the module gaps that the center of the usable pixels is elsewhere.
I am assuming that this will not significantly affect calculations.

*/

#include<mwmask.h>

int lccrim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2)
{
    int 
	return_value = 0;

    size_t
	bindex;

    double
	pcc,
	reslim,
	theta;

    double
	*avg1,
	*avg2,
	*nbins,
	*sum_11,
	*sum_12,
	*sum_22;

   // double
//	**bins_all;

    avg1 = (double*)calloc(25, sizeof(double)); //25 bins of radial width 50 pix
    avg2 = (double*)calloc(25, sizeof(double)); //25 bins of radial width 50 pix
    nbins = (double*)calloc(25, sizeof(double)); //25 bins of radial width 50 pix
    sum_11 = (double*)calloc(25, sizeof(double)); //25 bins of radial width 50 pix
    sum_12 = (double*)calloc(25, sizeof(double)); //25 bins of radial width 50 pix
    sum_22 = (double*)calloc(25, sizeof(double)); //25 bins of radial width 50 pix

    //bins_all = (double**)calloc(6, sizeof(double*));
    double *bins_all[6] = {avg1, avg2, nbins, sum_11, sum_12, sum_22};

    void loop_over_chunks(DIFFIMAGE*,DIFFIMAGE*,double**,void(double, double, size_t, double**));
    void calc_avgs(double, double, size_t, double**);
    void calc_cc(double, double, size_t, double**);
    
    // Loop over chunks: first pass
    loop_over_chunks(imdiff1, imdiff2, bins_all, &calc_avgs); 

    //Calculate averages and re-initialize nbins
    for (bindex = 0; bindex < 25; bindex++) {
	avg1[bindex] /= nbins[bindex];
	avg2[bindex] /= nbins[bindex];
	nbins[bindex] = 0;
    }

    // Loop over chunks: second pass
    loop_over_chunks(imdiff1, imdiff2, bins_all, &calc_cc); 

    //now every bin has the summed values

    printf("Res. limit (A)\tCorr. coeff.\n");

    for (bindex = 0; bindex < 25; bindex++) {
	pcc = sum_12[bindex]/(sqrt(sum_11[bindex]*sum_22[bindex]));
	theta = 0.5*atan((bindex+1)*50*imdiff1->pixel_size_mm/imdiff1->distance_mm);
	reslim = imdiff1->wavelength/(2*sin(theta));
	printf("%0.2f\t\t%0.2f\n", reslim, pcc);
    }

    /*
     * Free memory:
     */

    free((double*)avg1);
    free((double*)avg2);
    free((double*)nbins);
    free((double*)sum_11);
    free((double*)sum_12);
    free((double*)sum_22);

    return(return_value);
}

void loop_over_chunks(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2, double **bins_all, void (*func)(double, double, size_t, double**)) {

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
		    func(val1, val2, bindex, bins_all);
		    bins_all[2][bindex]++;
		}
	    }

	    // finished with this chunk
	}
    }

}

void calc_avgs(double val1, double val2, size_t bindex, double** bins_all) {

    bins_all[0][bindex] += val1;
    bins_all[1][bindex] += val2;

}

void calc_cc(double val1, double val2, size_t bindex, double** bins_all) {

    bins_all[4][bindex] += (val1 - bins_all[0][bindex]) * (val2 - bins_all[1][bindex]);
    bins_all[3][bindex] += (val1 - bins_all[0][bindex]) * (val1 - bins_all[0][bindex]);
    bins_all[5][bindex] += (val2 - bins_all[1][bindex]) * (val2 - bins_all[1][bindex]);

}
