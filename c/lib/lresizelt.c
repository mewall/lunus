/* LRESIZELT.C - Resize a lattice.
   
   Author: Mike Wall
   Date: 9/4/2015
   Version: 1.
   
   */

#include<mwmask.h>

int lresizelt(LAT3D *lat1, LAT3D *lat2)
{
  size_t
    i,
    j,
    k,
    index = 0;

  struct ijkcoords
    rvec;

  int
    return_value = 0;
  

  size_t index1;
  for(k = 0; k < lat2->zvoxels; k++) {
    for(j = 0; j < lat2->yvoxels; j++) {
      for (i = 0; i < lat2->xvoxels; i++) {
	rvec.i=i-lat2->origin.i+lat1->origin.i;
	rvec.j=j-lat2->origin.j+lat1->origin.j;
	rvec.k=k-lat2->origin.k+lat1->origin.k;
	if (rvec.i>0&&rvec.i<lat1->xvoxels&&rvec.j>0&&rvec.j<lat1->yvoxels&&rvec.k>0&&rvec.k<lat1->zvoxels) {
	  index1=rvec.k*lat1->xyvoxels+rvec.j*lat1->xvoxels+rvec.i;
	  lat2->lattice[index]=lat1->lattice[index1];
	} else {
	  lat2->lattice[index]=lat2->mask_tag;
	}
	index++;
      }
    }
  }

  CloseShop:
  return(return_value);
}





