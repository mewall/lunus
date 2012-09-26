/* LCULLLT.C - Throw away part of a lattice.
   
   Author: Mike Wall
   Date: 4/26/95
   Version: 1.
   
   */

#include<mwmask.h>

int lculllt(LAT3D *lat)
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
    rscale;

  struct xyzcoords
    rfloat;

  /*
   * Allocate counting array:
   */

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
	r = (size_t)(sqrtf((rfloat.x*rfloat.x + rfloat.y*rfloat.y + 
		       rfloat.z*rfloat.z) / rscale)+.5);
	if ((r < lat->inner_radius) || (r > lat->outer_radius)) {
	  lat->lattice[index] = lat->mask_tag;
	}
	index++;
      }
    }
  }
  CloseShop:
  return(return_value);
}





