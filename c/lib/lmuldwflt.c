/* LMULDWFLT.C - Multiply a lattice by an anisotropic Debye-Waller factor.
   
   Author: Mike Wall
   Date: 7/27/15
   Version: 1.
   
   */

#include<mwmask.h>

int lmuldwflt(LAT3D *lat)
{
  
  int return_value=0;

  long
    i,j,k;

  float
    rscale,
    factor,
    denom,
    sum;

  struct ijkcoords rvec;

  size_t lat_index=0;
  for(k = 0; k < lat->zvoxels; k++) {
    for(j = 0; j < lat->yvoxels; j++) {
      for (i = 0; i < lat->xvoxels; i++) {
	struct xyzcoords rdist,U_times_rdist;
	rdist.x = lat->xscale * (i-lat->origin.i);
	rdist.y = lat->yscale * (j-lat->origin.j);
	rdist.z = lat->zscale * (k-lat->origin.k);
	U_times_rdist = lmatvecmul(lat->anisoU,rdist);
	float rsqr;
	rsqr = ldotvec(rdist,U_times_rdist);
	if (lat->lattice[lat_index] != lat->mask_tag) {
	  lat->lattice[lat_index] *= (LATTICE_DATA_TYPE)expf(-2.*PI*2.*PI*rsqr/2.);
	}
	lat_index++;
      }
    }
  }

  CloseShop:
  return(return_value);
}


