/* LCORRLT.C - Calculate the correlation coefficient between two lattices.
   
   Author: Mike Wall
   Date: 1/22/2013
   Version: 1.
   
   */

#include<mwmask.h>

float lcorrlt(LAT3D *lat1, LAT3D *lat2)
{
  size_t
    i,
    j,
    k,
    ct=0,ctdat=0,
    index = 0;

  float
    avg1 = 0,
    avg2 = 0,
    corr = 0;

  // Calculate means  

  for(k = 0; k < lat1->zvoxels; k++) {
    for(j = 0; j < lat1->yvoxels; j++) {
      for (i = 0; i < lat1->xvoxels; i++) {
        if (lat1->lattice[index] != lat1->mask_tag) ctdat++;
	if ((lat1->lattice[index] != lat1->mask_tag) &&
	    (lat2->lattice[index] != lat1->mask_tag)) {
	  //	    (lat2->lattice[index] != lat1->mask_tag) && (lat1->lattice[index] >= lat1->threshold)) {
	  avg1 += lat1->lattice[index];
	  avg2 += lat2->lattice[index];
	  ct++;
	}
	index++;
      }
    }
  }

  avg1 /= (float)ct;
  avg2 /= (float)ct;

#ifdef DEBUG

  printf("%d %d\n",ctdat,ct);

#endif

  float 
    s11=0.0f,
    s22=0.0f,
    s12=0.0f;

  index=0;
  for(k = 0; k < lat1->zvoxels; k++) {
    for(j = 0; j < lat1->yvoxels; j++) {
      for (i = 0; i < lat1->xvoxels; i++) {
	if ((lat1->lattice[index] != lat1->mask_tag) &&
	    (lat2->lattice[index] != lat1->mask_tag)) {
	  //	    (lat2->lattice[index] != lat2->mask_tag) && (lat1->lattice[index] >= lat1->threshold)) {
	  float d1 = (lat1->lattice[index]-avg1);
	  float d2 = (lat2->lattice[index]-avg2);
	  s11 += d1*d1;
	  s22 += d2*d2;
	  s12 += d1*d2;
	}
	index++;
      }
    }
  }


  corr = s12/sqrtf(s11)/sqrtf(s22);

  CloseShop:
  return(corr);
}





