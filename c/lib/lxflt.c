/* LXFLT.C - Transform a lattice according to input line instructions.
   
   Author: Mike Wall
   Date: 3/24/95
   Version: 1.
   
   */

#include<mwmask.h>

int lxf1lt(LAT3D *lat)
{
  size_t
    ct,
    lat_index1 = 0,
    lat_index2 = 0;
  
  int
    return_value = 0;

  struct ijkcoords 
    index1,
    index2,
    rvec1,
    rvec2;

  LATTICE_DATA_TYPE
    *lattice;

  /*
   * Allocate lattice:
   */

  lattice = (LATTICE_DATA_TYPE *)calloc(lat->lattice_length,
					sizeof(LATTICE_DATA_TYPE));

  if (!lattice) {
    sprintf(lat->error_msg,"\nLSYM1LT:  Couldn't allocate arrays.\n\n");
    return_value = 1;
    goto CloseShop;
  }

  for(index1.k = 0; index1.k < lat->zvoxels; index1.k++) {
    for(index1.j = 0; index1.j < lat->yvoxels; index1.j++) {
      for (index1.i = 0; index1.i < lat->xvoxels; index1.i++) {
	rvec1.i = index1.i - lat->origin.i;
	rvec1.j = index1.j - lat->origin.j;
	rvec1.k = index1.k - lat->origin.k;
	rvec2.i = rvec1.i;
	rvec2.j = -rvec1.j;
	rvec2.k = rvec1.k;
	index2.i = rvec2.i + lat->origin.i;
	index2.j = rvec2.j + lat->origin.j;
	index2.k = rvec2.k + lat->origin.k;
	lat_index2 = index2.k*lat->xyvoxels + index2.j*lat->xvoxels + index2.i;
	lattice[lat_index1] = lat->lattice[lat_index2];
	lat_index1++;
      }
    }
  }

  /*
   * Copy lattice to input lattice:
   */

  for(lat_index1 = 0; lat_index1 < lat->lattice_length; lat_index1++)
    {
      lat->lattice[lat_index1] = lattice[lat_index1];
    }
  CloseShop:
  free((LATTICE_DATA_TYPE *)lattice);
  return(return_value);
}

