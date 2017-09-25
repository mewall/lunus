/* LSUBRFLT.C - Subtract a radial intensity distribution from each pixel
		in a 3D lattice.
   
   Author: Mike Wall
   Date: 3/28/93
   Version: 1.
   
   */

#include<mwmask.h>

int lsubrflt(LAT3D *lat)
{
  size_t
    i,
    j,
    k,
    r,
    index = 0;
  
  float
    rf,
    rscale;

  struct xyzcoords
    rfloat;
  
  struct ijkcoords rvec;
  

  // First, compute the spline from the rfile

  float *tau, *c;
  int n,ibcbeg,ibcend;
  int l,jd,kk;
  kk = 4;
  jd = 0;

  tau = (float *)malloc(sizeof(float)*lat->rfile_length);
  c = (float *)malloc(sizeof(float)*4*lat->rfile_length);
  //  n = (int)lat->rfile_length;
  ibcbeg = 0;
  ibcend = 0;

  // 


  // populate variables for spline

  n=0;

  for (i=0;i<lat->rfile_length;i++) {
    if (lat->rfile[i] != lat->mask_tag) {
      tau[n]=(float)i;
      c[4*n] = lat->rfile[i];
      n++;
    }
  }

  lspline(tau,c,&n,&ibcbeg,&ibcend);

  rscale = (lat->xscale*lat->xscale + lat->yscale*lat->yscale +
		 lat->zscale*lat->zscale);
  for(k = 0; k < lat->zvoxels; k++) {
    for(j = 0; j < lat->yvoxels; j++) {
      for (i = 0; i < lat->xvoxels; i++) {
	rvec.i = i - lat->origin.i;
	rvec.j = j - lat->origin.j;
	rvec.k = k - lat->origin.k;
	rfloat.x = lat->xscale * rvec.i;
	rfloat.y = lat->yscale * rvec.j;
	rfloat.z = lat->zscale * rvec.k;
	rf = sqrtf((rfloat.x*rfloat.x + rfloat.y*rfloat.y + 
		       rfloat.z*rfloat.z) / rscale);
	//	r = (size_t)(rf+.5);
	r = (size_t)(rf);
	if ((r < lat->rfile_length) && 
	    (lat->lattice[index] != lat->mask_tag)) {
	  //    lat->lattice[index] -= lat->rfile[r];
	  l = (int)r + 1;
	  if (rf>= tau[0] && rf <= tau[n-2]) {
	    lat->lattice[index] -= lspleval(tau,c,&l,&kk,&rf,&jd);
	  } else {
	    lat->lattice[index] = lat->mask_tag;
	  }
	  //	  lat->lattice[index] -= (lat->rfile[r] + 
	  //				  (rf - (float)r)*(lat->rfile[r+1] - 
	  //						   lat->rfile[r]));
	}
	index++;
      }
    }
  }
}


