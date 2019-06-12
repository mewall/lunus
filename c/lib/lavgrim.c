/* LAVGRIM.C - Calculate the average intensity vs. radius for an
		input image.
   
   Author: Mike Wall   
   Date: 4/3/93
   Version: 1.
   
   */

#include<mwmask.h>

int lavgrim(DIFFIMAGE *imdiff_in)
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

  int pidx;

  DIFFIMAGE *imdiff;

  n = (size_t *)calloc(MAX_RFILE_LENGTH, sizeof(size_t));
  rf = (RFILE_DATA_TYPE *)calloc(MAX_RFILE_LENGTH,sizeof(RFILE_DATA_TYPE));
  imdiff_in->rfile_length = 0;

  for (pidx = 0; pidx < imdiff_in->num_panels; pidx++) {

    imdiff = &imdiff_in[pidx];
    index = 0;

    if (imdiff->slist == NULL) lslistim(imdiff);

    struct xyzcoords s;

    for(r = 0; r < imdiff->vpixels; r++) {
      for(c = 0; c < imdiff->hpixels; c++) {
	s = imdiff->slist[index];
	ssq = ldotvec(s,s);
	cos_two_theta = 1. - ssq * imdiff->wavelength * imdiff->wavelength / 2.;
	rr = imdiff->distance_mm / cos_two_theta; 
	rvec.x = imdiff->wavelength * s.x * rr;
	rvec.y = imdiff->wavelength * s.y * rr;
	//	printf("rvec = (%f,%f)\n",rvec.x,rvec.y);
	radius = (size_t)(sqrtf(rvec.x*rvec.x + rvec.y*rvec.y)/imdiff->pixel_size_mm+.5);
	if ((imdiff->image[index] != imdiff->overload_tag) &&
	    (imdiff->image[index] != imdiff->ignore_tag)) {
	  if (radius >= imdiff_in->rfile_length) 
	    imdiff_in->rfile_length = radius+1;
	  rf[radius] += (RFILE_DATA_TYPE)(float)(imdiff->image[index]-imdiff->value_offset);
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
      if (i>100 && i<=110) printf("lavgrim rf[%d] = %g,",i,rf[i]);
#endif
    } else {
      imdiff_in->rfile[i] = 0.0;
    }
  }
  free(n);
  free(rf);
}
