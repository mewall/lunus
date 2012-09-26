/* LSOLIDLT.C - Apply solid angle correction to lattice.
   
   Author: Mike Wall
   Date: 3/25/95
   Version: 1.
   
   */

#include<mwmask.h>

int lsolidlt(LAT3D *lat)
{
  size_t
    *ct,
    i,
    j,
    k,
    r,
    index = 0;

  int
    return_value = 0;
  
  struct ijkcoords rvec;
  
  float
    corr,
    rsq,
    lambdasq;

  struct xyzcoords
    rfloat;

  lambdasq = lat->wavelength;
  for(k = 0; k < lat->zvoxels; k++) {
    for(j = 0; j < lat->yvoxels; j++) {
      for (i = 0; i < lat->xvoxels; i++) {
	rvec.i = i - lat->origin.i;
	rvec.j = j - lat->origin.j;
	rvec.k = k - lat->origin.k;
	rfloat.x = lat->xscale * rvec.i;
	rfloat.y = lat->yscale * rvec.j;
	rfloat.z = lat->zscale * rvec.k;
	rsq = (rfloat.x*rfloat.x + rfloat.y*rfloat.y + rfloat.z*rfloat.z);
	if (lat->lattice[index] != lat->mask_tag) {
	  corr = (1-.5*rsq*lambdasq);
	  lat->lattice[index] /= corr*corr*corr;
	}
	index++;
      }
    }
  }
  CloseShop:
  return(return_value);
}





