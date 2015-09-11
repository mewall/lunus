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

  struct ijkcoords s;

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
  int tagged=0;
  index = 0;
  for(k = 0; k < lat2->zvoxels; k++) {
    for(j = 0; j < lat2->yvoxels; j++) {
      for (i = 0; i < lat2->xvoxels; i++) {
	pos.x = lat2->xbound.min + ((float)i)*lat2->xscale;
	pos.y = lat2->ybound.min + ((float)j)*lat2->yscale;
	pos.z = lat2->zbound.min + ((float)k)*lat2->zscale;
	tagged=0;
	for (m = 0; m<map1->natoms;m++) {
	  disp.x = pos.x - map1->xyzlist[m].x;
	  disp.y = pos.y - map1->xyzlist[m].y;
	  disp.z = pos.z - map1->xyzlist[m].z;
	  struct xyzcoords d;
	  for (s.k=-1;s.k<=1;s.k++) {
	    d.z = disp.z + ((float)s.k)*map1->zlen;
	    for (s.j=-1;s.j<=1;s.j++) {
	      d.y = disp.y + ((float)s.j)*map1->ylen;
	      for (s.i=-1;s.i<=1;s.i++) {
		d.x = disp.x + ((float)s.i)*map1->xlen;
		dist = sqrtf(d.x*d.x+d.y*d.y+d.z*d.z);
		if (dist<map1->arad) {
		  lat1_back->lattice[index] = lat2->mask_tag;
		  tagged=1;
		} 
	      }
	    }
	  }
	} // m
	if (tagged == 1) {
	  nfore++;
	} else {
	  lat1_fore->lattice[index] = lat2->mask_tag;
	  nback++;
	}
	index++;
      } // i
    } // j
  } // k

  lat1_fore->threshold = -32768.f;
  lat1_back->threshold = -32768.f;

    // Foreground (atoms)

  map1->cc_fore = lcorrlt(lat1_fore,lat2);
  map1->rsr_fore = lrsrlt(lat1_fore,lat2);

  // Background (valence)

  map1->cc_back = lcorrlt(lat1_back,lat2);
  map1->rsr_back = lrsrlt(lat1_back,lat2);

#ifdef DEBUG

  printf("#foreground,#background= %ld %ld\n",nfore,nback);

#endif

  CloseShop:
  return(0);
}





