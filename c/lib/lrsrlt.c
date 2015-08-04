/* LRSRLT.C - Calculate the real-space R-factor between two charge density lattices.
   
   Author: Mike Wall
   Date: 7/7/2015
   Version: 1.
   
   */

#include<mwmask.h>

float lrsrlt(LAT3D *lat1, LAT3D *lat2)
{
  size_t
    i,
    j,
    k,
    ct=0,
    index = 0;

  float
    return_value,num=0,denom = 0,x=0,y=0,xx=0,xy=0,w;
  
  ct=0;index=0;
  for(k = 0; k < lat1->zvoxels; k++) {
    for(j = 0; j < lat1->yvoxels; j++) {
      for (i = 0; i < lat1->xvoxels; i++) {
	if ((lat1->lattice[index] != lat1->mask_tag) &&
	    (lat2->lattice[index] != lat1->mask_tag)) {
	  x += lat1->lattice[index];
	  y += lat2->lattice[index];
	  ct++;
	}
	index++;
      }
    }
  }

  x /= (float)ct;
  y /= (float)ct;
  //w = x/y;

  ct=0;index=0;
  for(k = 0; k < lat1->zvoxels; k++) {
    for(j = 0; j < lat1->yvoxels; j++) {
      for (i = 0; i < lat1->xvoxels; i++) {
	if ((lat1->lattice[index] != lat1->mask_tag) &&
	    (lat2->lattice[index] != lat1->mask_tag)) {
	  xx += (lat1->lattice[index]-x)*(lat1->lattice[index]-x);
	  xy += (lat1->lattice[index]-x)*(lat2->lattice[index]-y);
	  ct++;
	}
	index++;
      }
    }
  }

  w = xx/xy;

  ct=0;index=0;
  for(k = 0; k < lat1->zvoxels; k++) {
    for(j = 0; j < lat1->yvoxels; j++) {
      for (i = 0; i < lat1->xvoxels; i++) {
	if ((lat1->lattice[index] != lat1->mask_tag) &&
	    (lat2->lattice[index] != lat1->mask_tag)) {
	  num += fabs(lat1->lattice[index]-x-w*(lat2->lattice[index]-y));
	  denom += fabs(lat1->lattice[index]-x+w*(lat2->lattice[index]-y));
	  ct++;
	}
	index++;
      }
    }
  }

  return_value = (float)num/denom;

  CloseShop:
  return(return_value);
}





