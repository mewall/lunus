/* LCULLCONELT.C - Mask out voxels within a cone about an axis.
   
   Author: Mike Wall
   Date: 9/28/2016
   Version: 1.
   
   */

#include<mwmask.h>

void lcullconelt(LAT3D *lat)
{

  size_t lat_index;

  float y,x;

  struct xyzcoords s;

  struct ijkcoords index;

  // mask the lattice
  lat_index = 0;
  for(index.k = 0; index.k < lat->zvoxels; index.k++) {
    for(index.j = 0; index.j < lat->yvoxels; index.j++) {
      for (index.i = 0; index.i < lat->xvoxels; index.i++) {
	lat->index = index;
	
	s = lsFromIndex(lat);
	if (lat->axis == 2) {
	  y = sqrtf(s.x*s.x+s.z*s.z);
	  x = fabs(s.y);
	}
	if (atan2f(y,x)<PI/4.) lat->lattice[lat_index] = lat->mask_tag;
	lat_index++;
      }
    }
  }
}

