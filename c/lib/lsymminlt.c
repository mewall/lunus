/* LSYMMINLT.C - Symmetrize a lattice using the minimum value among symmetry related points.
   
   Author: Mike Wall
   Date: 5/19/2017
   Version: 1.
   
   */

#include<mwmask.h>

int lsymminlt(LAT3D *lat)
{
  size_t
    i,
    max_op_count = 100,
    op_count,
    op_index,
    ct,
    *lat_index;
  
  int
    return_value = 0;

  struct ijkcoords 
    *index,
    *rvec;

  LATTICE_DATA_TYPE
    *lattice;

  /*
   * Allocate lattice:
   */

  lattice = (LATTICE_DATA_TYPE *)calloc(lat->lattice_length,
					sizeof(LATTICE_DATA_TYPE));
  lat_index = (size_t *)calloc(max_op_count, sizeof(size_t));
  index = (struct ijkcoords *)malloc(max_op_count*sizeof(struct
						     ijkcoords));
  rvec = (struct ijkcoords *)malloc(max_op_count*sizeof(struct
						     ijkcoords));
 

  if (!lattice || !lat_index || !index || !rvec) {
    sprintf(lat->error_msg,"\nLSYM1LT:  Couldn't allocate arrays.\n\n");
    return_value = 1;
    goto CloseShop;
  }

  for(i=0;i<lat->lattice_length;i++) {
    lattice[i] = lat->mask_tag;
  }

  lat->symvec = rvec;
  lat_index[0] = 0;
  for(index[0].k = 0; index[0].k < lat->zvoxels; index[0].k++) {
    for(index[0].j = 0; index[0].j < lat->yvoxels; index[0].j++) {
      for (index[0].i = 0; index[0].i < lat->xvoxels; index[0].i++) {
	rvec[0] = lijksub(index[0],lat->origin);
	switch(lat->symop_index) {
	case 0:
	  lP1(lat);
	  break;
	case 1:
	  lP41(lat);
	  break;
	case 2:
	  lP222(lat);
	  break;
	case 3:
	  lPm_minus_3(lat);
	  break;
	case 4:
	  lsg10(lat);
	  break;
	}
	op_count = lat->symop_count;
	ct = 0;
	for(op_index = 0;op_index < op_count; op_index++) {
	  index[op_index] = lijksum(rvec[op_index],lat->origin);
	  lat_index[op_index] = index[op_index].k*lat->xyvoxels +
	    index[op_index].j*lat->xvoxels + index[op_index].i;
	  if ((lat_index[op_index] >= 0) && 
	      (lat_index[op_index] < lat->lattice_length) &&
	      (lat->lattice[lat_index[op_index]] != lat->mask_tag)) {
	    if (lattice[lat_index[0]] == lat->mask_tag || lattice[lat_index[0]] > lat->lattice[lat_index[op_index]]) {
	      lattice[lat_index[0]] = lat->lattice[lat_index[op_index]];
	    }
	  }
	}
	lat_index[0]++;
      }
    }
  }

  /*
   * Copy lattice to input lattice:
   */

  for(lat_index[0] = 0; lat_index[0] < lat->lattice_length; lat_index[0]++)
    {
      lat->lattice[lat_index[0]] = lattice[lat_index[0]];
    }
  CloseShop:
  free((LATTICE_DATA_TYPE *)lattice);
  free((size_t *)lat_index);
  free((struct ijkcoords *)rvec);
  free((struct ijkcoords *)index);
  return(return_value);
}








