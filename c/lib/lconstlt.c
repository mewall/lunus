/* LCONSTLT.C - Create a constant lattice, using an input lattice as a
                template. 
   
   Author: Mike Wall
   Date: 8/5/95
   Version: 1.
   
   */

#include<mwmask.h>

int lconstlt(LAT3D *lat)
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
	  lat->lattice[index] = lat->rfile[0];
	}
	index++;
      }
    }
  }
}


