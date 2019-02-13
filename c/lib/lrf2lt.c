/* LRF2LT.C - Generate a lattice from an rfile.
   
   Author: Mike Wall
   Date: 3/3/95
   Version: 1.
   
   */

#include<mwmask.h>

int lrf2lt(LAT3D *lat)
{
  size_t
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
	r = (size_t)sqrtf((rfloat.x*rfloat.x + rfloat.y*rfloat.y + 
		       rfloat.z*rfloat.z) / rscale);
	if ((lat->lattice[index] != lat->mask_tag) &&
	     (r < lat->rfile_length)){
	  lat->lattice[index] = lat->rfile[r];
	} else {
	  lat->lattice[index] = lat->mask_tag;
	}
	index++;
      }
    }
  }
  CloseShop:
  return(return_value);
}





