/* LRFACLT.C - Calculate the R-factor between two lattices.
   
   Author: Mike Wall
   Date: 1/22/2013
   Version: 1.
   
   */

#include<mwmask.h>

float lrfaclt(LAT3D *lat1, LAT3D *lat2)
{
  size_t
    i,
    j,
    k,
    ct=0,
    index = 0;

  float
    return_value = 0;
  
  for(k = 0; k < lat1->zvoxels; k++) {
    for(j = 0; j < lat1->yvoxels; j++) {
      for (i = 0; i < lat1->xvoxels; i++) {
	if ((lat1->lattice[index] != lat1->mask_tag) &&
	    (lat2->lattice[index] != lat1->mask_tag)) {
	  return_value += fabs(sqrtf(fabs(lat1->lattice[index]))-sqrtf(fabs(lat2->lattice[index])))/sqrtf(fabs(lat1->lattice[index]));
	  ct++;
	}
	index++;
      }
    }
  }

  return_value /= (float)ct;

  CloseShop:
  return(return_value);
}





