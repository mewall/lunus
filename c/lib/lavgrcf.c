/* LAVGRCF.C - Calculate the average intensity vs. radius for an
		input image correction factor.
   
   Author: Mike Wall   
   Date: 12/31/2020
   Version: 1.
   
   */

#include<mwmask.h>

int lavgrcf(DIFFIMAGE *imdiff_in)
{
  size_t
    i,
    r,
    c,
    *n,
    radius,
    index = 0;

  struct xycoords rvec;

  RFILE_DATA_TYPE *rf;

  XYZCOORDS_DATA ssq,rr;

  float cos_two_theta;

  float rvec_mag;

  int pidx;

  DIFFIMAGE *imdiff;

  if (imdiff_in->slist == NULL) lslistim(imdiff_in);

  n = (size_t *)calloc(MAX_RFILE_LENGTH, sizeof(size_t));
  rf = (RFILE_DATA_TYPE *)calloc(MAX_RFILE_LENGTH,sizeof(RFILE_DATA_TYPE));
  imdiff_in->rfile_length = 0;


  for (pidx = 0; pidx < imdiff_in->num_panels; pidx++) {

    imdiff = &imdiff_in[pidx];
    index = 0;

    struct xyzcoords s;

    for(r = 0; r < imdiff->vpixels; r++) {
      for(c = 0; c < imdiff->hpixels; c++) {
	s = imdiff->slist[index];
	ssq = ldotvec(s,s);
	cos_two_theta = 1. - ssq * imdiff->wavelength * imdiff->wavelength / 2.;
	rr = imdiff->distance_mm / cos_two_theta; 
	rvec.x = imdiff->wavelength * s.x * rr;
	rvec.y = imdiff->wavelength * s.y * rr;
	rvec_mag = sqrtf(rvec.x*rvec.x + rvec.y*rvec.y);
	radius = (size_t)(rvec_mag/imdiff->pixel_size_mm+.5);
	if ((imdiff->image[index] != imdiff->overload_tag) &&
	    (imdiff->image[index] != imdiff->ignore_tag)) {
	  if (radius >= imdiff_in->rfile_length) 
	    imdiff_in->rfile_length = radius+1;
	  rf[radius] += (RFILE_DATA_TYPE)(imdiff->correction[index]);
	  n[radius]++;
	}
	index++;
      }
    }
  }
  for(i=0;i<imdiff_in->rfile_length;i++) {
    if (n[i] > 0) {
      imdiff_in->rfile[i] = rf[i]/(RFILE_DATA_TYPE)n[i];
#ifdef DEBUG
      if (i>100 && i<=110) printf("lavgrim rf[%d] = %g,",i,imdiff_in->rfile[i]);
#endif
    } else {
      imdiff_in->rfile[i] = 0.0;
    }
  }

  free(n);
  free(rf);
}
