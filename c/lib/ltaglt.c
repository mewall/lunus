/* LTAGLT.C - Convert all voxels of specified value in a lattice to an 
             ignore_tag.
   
   Author: Mike Wall
   Date: 5/5/95
   Version: 1.
   
   */

#include<mwmask.h>

int ltaglt(LAT3D *lat)
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
	if (lat->lattice[index] == lat->rfile[0]) {
	  lat->lattice[index] = lat->rfile[1];
	}
	index++;
      }
    }
  }
}


