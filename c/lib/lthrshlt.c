/* LTHRSHLT.C - Convert all voxels with values outside a specified range in a lattice to an 
             ignore_tag.
   
   Author: Mike Wall
   Date: 8/11/2014
   Version: 1.
   
   */

#include<mwmask.h>

int lthrshlt(LAT3D *lat)
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
	if (lat->lattice[index]!=lat->mask_tag && (lat->lattice[index]<lat->valuebound.min || lat->lattice[index]>lat->valuebound.max)) {
	  lat->lattice[index] = lat->mask_tag;
	}
	index++;
      }
    }
  }
}


