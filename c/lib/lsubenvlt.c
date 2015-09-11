/* LSUBENVLT.C - Calculate the voxel-by-voxel difference of two lattices, using the first lattice as a maximum envelope.
   
   Author: Mike Wall
   Date: 8/11/2014
   Version: 1.
   
   */

#include<mwmask.h>

int lsubenvlt(LAT3D *lat1, LAT3D *lat2)
{
  size_t
    i,
    j,
    k,
    index = 0;
  
  LATTICE_DATA_TYPE newval;

  int
    return_value = 0;
  
  for(k = 0; k < lat1->zvoxels; k++) {
    for(j = 0; j < lat1->yvoxels; j++) {
      for (i = 0; i < lat1->xvoxels; i++) {
	if ((lat1->lattice[index] != lat1->mask_tag) &&
	    (lat2->lattice[index] != lat1->mask_tag)) {
	  newval = lat1->lattice[index] - lat2->lattice[index];
	  if (newval<lat1->lattice[index]) {
	    lat1->lattice[index]=newval;
	  }
	}
	else {
	  lat1->lattice[index] = lat1->mask_tag;
	}
	index++;
      }
    }
  }
  CloseShop:
  return(return_value);
}





