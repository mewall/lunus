/* LSUBRFLT.C - Subtract a radial intensity distribution from each pixel
		in a 3D lattice.
   
   Author: Mike Wall
   Date: 3/28/93
   Version: 1.
   
   */

#include<mwmask.h>

int lsubrfsvlt(LAT3D *lat)
{
  size_t
    i,
    j,
    k,
    r,
    lat_index = 0;
  
  float
    s,
    rf,
    rscale;

  struct xyzcoords
    rfloat;
  
  struct ijkcoords index,rvec;
  

  // Use a spline to obtain the rfile index from scattering vectors

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
      tau[n]=(float)lat->rfile_svecs[i];
      c[4*n] = (float)i;
      //      printf("spline:%g %g\n",tau[n],c[4*n]);
      n++;
    }
  }

  lspline(tau,c,&n,&ibcbeg,&ibcend);

  lat->index.i=lat->origin.i+1;
  lat->index.j=lat->origin.j+1;
  lat->index.k=lat->origin.k+1;
  rscale=lssqrFromIndex(lat);

  lat_index = 0;
  l = (int)lat->rfile_length-2;
  for(index.k = 0; index.k < lat->zvoxels; index.k++) {
    for(index.j = 0; index.j < lat->yvoxels; index.j++) {
      for (index.i = 0; index.i < lat->xvoxels; index.i++) {
	lat->index = index;
	s = sqrtf(lssqrFromIndex(lat));
	if (s>= tau[0] && s <= tau[l]) {
	  rf = lspleval(tau,c,&l,&kk,&s,&jd);
	  //	  printf("%g %g,",s,rf);
	  r = (size_t)floorf(rf);
	  if ((r < lat->rfile_length) && 
	      (lat->lattice[lat_index] != lat->mask_tag)) {
	    lat->lattice[lat_index] -= lat->rfile[r];
	  } else {
	    lat->lattice[lat_index] = lat->mask_tag;
	  }
	} else {
	    lat->lattice[lat_index] = lat->mask_tag;
	}
	  //	    lat->lattice[lat_index] -= (lat->rfile[r] + 
	  //				  (rf - (float)r)*(lat->rfile[r+1] - 
	  //						   lat->rfile[r]));
	    //	  } else {
	    //	    lat->lattice[lat_index] = lat->mask_tag;
	    //	  }
	lat_index++;
      }
    }
  }

}


