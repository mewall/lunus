/* LRMSDIM.C - Calculate the rmsd between two images in different resolution
 * shells.

Author: Veronica Pillar (modified from Mike Wall's codes)
Version:2.0
Date: 3/30/15

Input argument is two diffraction images.  Output is sent to stdout.

NB: each chunk is defined as located where its center is, even if so many pixels are
out of range or in the module gaps that the center of the usable pixels is elsewhere.
I am assuming that this will not significantly affect calculations.

*/

#include<mwmask.h>

int lrmsdim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2)
{
  RCCOORDS_DATA
    r1,
    r2,
    c1,
    c2,
    rbox,
    cbox;

  struct rccoords
    rvec;

  int 
    return_value = 0;

  size_t
    index,
    bindex,
    box_size = imdiff1->mode_width, // initialize to the value passed
    index1 = 0,
    index2 = 0,
    nchunk1,
    nchunk2,
    vchunk1,
    vchunk2;

  double
    radius,
    diff,
    rmsd,
    reslim,
    theta,
    rdist,
    cdist;

  double
    *bins,
    *nbins;

  bins = (double*)calloc(25, sizeof(double)); //25 bins of radial width 50 pix
  nbins = (double*)calloc(25, sizeof(double)); //25 bins of radial width 50 pix

  // Loop over chunks
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

      // Calculate diff and radius, using chunk averages.
      if ((nchunk1 > 0) && (nchunk2 > 0)) {
	diff = (double)vchunk1/(double)nchunk1 - (double)vchunk2/(double)nchunk2;
	rdist = (rvec.r + (double)box_size/2.0 - 0.5);
	cdist = (rvec.c + (double)box_size/2.0 - 0.5);
	radius = sqrt(rdist*rdist + cdist*cdist);
	bindex = (int)floor(radius/50); //which bin this radius goes in
	if (bindex < 25) {
	  bins[bindex] += diff*diff;
	  nbins[bindex]++;
	}
      }
    }
  }

  //now every bin has the sum of squared differences

  printf("Res. limit (A)\tRMSD\n");

  for (bindex = 0; bindex < 25; bindex++) {
    rmsd = sqrt(bins[bindex]/nbins[bindex]);
    theta = 0.5*atan((bindex+1)*50*imdiff1->pixel_size_mm/imdiff1->distance_mm);
    reslim = imdiff1->wavelength/(2*sin(theta));
    printf("%0.2f\t\t%0.2f\n", reslim, rmsd);
  }


  /*
   * Free memory:
   */

  free((double*)bins);
  free((double*)nbins);

EndPaper:

  return(return_value);
}
