/* LMULSCLT.C - Multiply each voxel in a lattice by a scalar.
   
   Author: Mike Wall
   Date: 3/21/95
   Version: 1.
   
   */

#include<mwmask.h>

int lmulsclt(LAT3D *lat)
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
	  lat->lattice[index] *= lat->rfile[0];
	}
	index++;
      }
    }
  }
}


