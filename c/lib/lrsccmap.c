/* LRSCCMAP.C - Calculate the real-space correlation coefficient between two maps.
   
   Author: Mike Wall
   Date: 7/6/2015
   Version: 1.
   
   */

#include<mwmask.h>

int lrsccmap(CCP4MAP *map1, CCP4MAP *map2)
{
  size_t
    i,
    j,
    k,
    m,
    ct=0,ctdat=0,
    index = 0;

  LAT3D
    *lat1_fore,
    *lat1_back,
    *lat2;

  float
    avg1 = 0,
    avg2 = 0,
    corr = 0;

  /*
   * Initialize lattices:
   */
  
  if (((lat1_fore = linitlt()) == NULL) || ((lat1_back = linitlt()) == NULL) || ((lat2 = linitlt()) == NULL)) {
    perror("Couldn't initialize lattices.\n\n");
    exit(0);
  }

  // copy maps to lattices

  if (lcpmaplt(map1,lat1_fore) != 0) {
    perror("Couldn't copy map to lattice\n\n");
    exit(0);
  }
  if (lcpmaplt(map1,lat1_back) != 0) {
    perror("Couldn't copy map to lattice\n\n");
    exit(0);
  }
  if (lcpmaplt(map2,lat2) != 0) {
    perror("Couldn't copy map to lattice\n\n");
    exit(0);
  }


  // Mask the foreground and background lattices

  struct xyzcoords pos,disp;
  float dist;

  size_t nfore=0,nback=0;

  index = 0;
  for(k = 0; k < lat2->zvoxels; k++) {
    for(j = 0; j < lat2->yvoxels; j++) {
      for (i = 0; i < lat2->xvoxels; i++) {
	pos.x = lat2->xbound.min + ((float)i)*lat2->xscale;
	pos.y = lat2->ybound.min + ((float)j)*lat2->yscale;
	pos.z = lat2->zbound.min + ((float)k)*lat2->zscale;
	for (m = 0; m<map1->natoms;m++) {
	  disp.x = pos.x - map1->xyzlist[m].x;
	  disp.y = pos.y - map1->xyzlist[m].y;
	  disp.z = pos.z - map1->xyzlist[m].z;
	  dist = sqrtf(disp.x*disp.x+disp.y*disp.y+disp.z*disp.z);
	  if (dist<map1->arad) {
	    lat1_back->lattice[index] = lat2->mask_tag;
	    nfore++;
	  } else {
	    lat1_fore->lattice[index] = lat2->mask_tag;
	    nback++;
	  }
	} // m
      } // i
    } // j
  } // k

    // Foreground (atoms)

  map1->cc_fore = lcorrlt(lat1_fore,lat2);
  
  // Background (valence)

  map1->cc_back = lcorrlt(lat1_back,lat2);

  printf("#foreground,#background= %ld %ld\n",nfore,nback);

  CloseShop:
  return(0);
}





