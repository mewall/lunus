/* LCFIM.C - Calculate correction factor for a diffraction image.
   
   Author: Mike Wall
   Date: 6/5/2017
   Version: 1.
   
   */

#include<mwmask.h>

int lcfim(DIFFIMAGE *imdiff_in)
{
  size_t 
    index = 0;
  
  RCCOORDS_DATA
    r,
    c;
  
  int 
    return_value = 0;
  
  
  float 
    scale,
    radius,
    radius_squared,
    distance_pixels,
    arctan_argument,
    two_theta,
    cos_two_theta,
    cos_sq_two_theta,
    sin_sq_two_theta,
    cos_two_rho,
    two_rho_offset,
    cos_incidence,
    maxcorr;

  XYZCOORDS_DATA ssq;

  int pidx;

  DIFFIMAGE *imdiff;

  struct xyzcoords normal_vec;

  for (pidx = 0; pidx < imdiff_in->num_panels; pidx++) {
    imdiff = &imdiff_in[pidx];
    index = 0;

    struct xyzcoords s;

    scale = imdiff->correction_factor_scale;
    two_rho_offset = 2.*PI/180.*imdiff->polarization_offset;
    for(r=0; r < imdiff->vpixels; r++) {
      imdiff->pos.r = r;
      for(c=0; c < imdiff->hpixels; c++) {
	imdiff->pos.c = c;
	s = lcalcsim(imdiff);
	ssq = ldotvec(s,s);
	cos_two_theta = 1. - ssq * imdiff->wavelength *imdiff->wavelength / 2.;
	cos_sq_two_theta = cos_two_theta * cos_two_theta;
	sin_sq_two_theta = 1. - cos_sq_two_theta;
	cos_two_rho = cosf(2.*acosf(s.x / sqrtf(s.x*s.x+s.y*s.y)) - two_rho_offset);
	// polarization
	imdiff->correction[index] = scale*(2. / (1. + cos_sq_two_theta -
						 imdiff->polarization *
						 cos_two_rho *
						 sin_sq_two_theta));
	// solid angle normalization

	cos_incidence = ldotvec(lmulscvec(imdiff->wavelength,laddvec(s,lmulscvec(1./imdiff->wavelength,imdiff->beam_vec))),imdiff->normal_vec);

	//	printf("cos_incidence(%d,%d)=%f\n",c,r,cos_incidence);

	imdiff->correction[index] /= cos_incidence*cos_incidence*cos_incidence;
	//	  if (imdiff->correction[index]>maxcorr) {
	//  maxcorr = imdiff->correction[index];
	//}
	//    	} else {
	//	  imdiff->correction[index] = scale;
	//	}
	index++;
      }
    }
  }
  //  printf("Maximum correction = %f\n",maxcorr);
  return(return_value);
}
