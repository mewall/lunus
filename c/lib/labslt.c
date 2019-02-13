/* LABSLT.C - Take tha absolute value of each voxel in a lattice.
   
   Author: Mike Wall
   Date: 5/12/15
   Version: 1.
   
   */

#include<mwmask.h>

int labslt(LAT3D *lat)
{
  size_t
    i,
    j,
    k,
    r,
    index = 0;
  
  for(k = 0; k < lat->zvoxels; k++) {
    for(j = 0; j < lat->yvoxels; j++) {
      for (i = 0; i < lat->xvoxels; i++) {
	if (lat->lattice[index] != lat->mask_tag) {
	  lat->lattice[index] = fabs(lat->lattice[index]);
	}
	index++;
      }
    }
  }
}


