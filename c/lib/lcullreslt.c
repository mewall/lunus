/* LCULLRESLT.C - Throw away part of a lattice.
   
   Author: Mike Wall
   Date: 2/1/2017
   Version: 1.
   
   */

#include<mwmask.h>

int lcullreslt(LAT3D *lat)
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
    res;

  struct xyzcoords
    rfloat;

  /*
   * Allocate counting array:
   */

  //  rscale = (lat->xscale*lat->xscale + lat->yscale*lat->yscale +
  //		 lat->zscale*lat->zscale);
  for(k = 0; k < lat->zvoxels; k++) {
    for(j = 0; j < lat->yvoxels; j++) {
      for (i = 0; i < lat->xvoxels; i++) {
	lat->index.i = i;
	lat->index.j = j;
	lat->index.k = k;
	/* rvec.i = i - lat->origin.i; */
	/* rvec.j = j - lat->origin.j; */
	/* rvec.k = k - lat->origin.k; */
	/* rfloat.x = lat->xscale * rvec.i; */
	/* rfloat.y = lat->yscale * rvec.j; */
	/* rfloat.z = lat->zscale * rvec.k; */
	/* r = (size_t)(sqrtf((rfloat.x*rfloat.x + rfloat.y*rfloat.y +  */
	/* 	       rfloat.z*rfloat.z) / rscale)+.5); */
	//	r = (size_t)(sqrtf(lssqrFromIndex(lat)/rscale)+0.5);
	res = 1./sqrtf(lssqrFromIndex(lat));
	if ((res < lat->resolution.min) || (res > lat->resolution.max)) {
	  lat->lattice[index] = lat->mask_tag;
	}
	index++;
      }
    }
  }
  CloseShop:
  return(return_value);
}





