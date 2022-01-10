/* LSUBRFIM.C - Subtract I(r) from a diffraction image.
   
   Author: Mike Wall
   Date: 4/4/94
   Version: 1.
   
   */

#include<mwmask.h>

int lsubrfim(DIFFIMAGE *imdiff_in)
{
	size_t
		r,
		c,
		radius,
		index = 0;

	int i;

	struct xycoords rvec;

	int pidx;

	DIFFIMAGE *imdiff;

	int
		return_value;
	
	XYZCOORDS_DATA
	  ssq,
	  rr,
	  cos_two_theta;
	
  if (imdiff_in->slist == NULL) lslistim(imdiff_in);

	
  // First, compute the spline from the rfile

  imdiff = imdiff_in;
  
  float *tau, *cv;
  int n,ibcbeg,ibcend;
  int l,jd,kk;
  kk = 4;
  jd = 0;

  tau = (float *)malloc(sizeof(float)*imdiff->rfile_length);
  cv = (float *)malloc(sizeof(float)*4*imdiff->rfile_length);
  //  n = (int)lat->rfile_length;
  ibcbeg = 0;
  ibcend = 0;

  // populate variables for spline

  n=0;

  for (i=0;i<imdiff->rfile_length;i++) {
    if (imdiff->rfile[i] != imdiff->overload_tag && imdiff->rfile[i] != imdiff->ignore_tag && imdiff->rfile[i] != imdiff->mask_tag) {
      tau[n]=(float)i;
      cv[4*n] = imdiff->rfile[i];
      n++;
    }
  }

  lspline(tau,cv,&n,&ibcbeg,&ibcend);

  // Subtract the rfile from each image panel
  
  for (pidx = 0; pidx < imdiff_in->num_panels; pidx++) {

    imdiff = &imdiff_in[pidx];
    index = 0;
    
    struct xyzcoords s;

    float radiusf;
    
    for(r = 0; r < imdiff->vpixels; r++) {
      for(c = 0; c < imdiff->hpixels; c++) {
	s = imdiff->slist[index];
	ssq = ldotvec(s,s);
	cos_two_theta = 1. - ssq * imdiff->wavelength * imdiff->wavelength / 2.;
	rr = imdiff->distance_mm / cos_two_theta; 
	rvec.x = imdiff->wavelength * s.x * rr;
	rvec.y = imdiff->wavelength * s.y * rr;
	radiusf = sqrtf(rvec.x*rvec.x + rvec.y*rvec.y)/imdiff->pixel_size_mm;
	radius = (size_t)(radiusf);

	l = (int)radius + 1;

	index = r*imdiff->hpixels+c;

	if ((imdiff->image[index] != imdiff->overload_tag) &&
	    (imdiff->image[index] != imdiff->ignore_tag) &&
	    (imdiff->image[index] != imdiff->mask_tag)) {
	  if (radiusf>= tau[0] && radiusf <= tau[n-2]) {
	    imdiff->image[index] -= (IMAGE_DATA_TYPE)lspleval(tau,cv,&l,&kk,&radiusf,&jd);
	  } else {
	    imdiff->image[index] = imdiff->ignore_tag;
	  }} else {
	    imdiff->image[index] = imdiff->ignore_tag;
		}		  

	      /*	      if (radius < imdiff->rfile_length) {
		index = r*imdiff->hpixels+c;
	        if ((imdiff->image[index] != imdiff->overload_tag) &&
		    (imdiff->image[index] != imdiff->ignore_tag) &&
		    (imdiff->image[index] != imdiff->mask_tag)) {
		    imdiff->image[index] -= (IMAGE_DATA_TYPE)imdiff->rfile[radius];
	        }
		}
	      else {
		return_value = 1;
		sprintf(imdiff->error_msg,"\nAttempt to read past end "
			"of rfile.\n\n");
			}*/
	    index++;
	  }
	}
  }
  return(return_value);
}


