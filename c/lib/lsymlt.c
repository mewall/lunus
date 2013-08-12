/* LSYMLT.C - Symmetrize a lattice according to input line instructions.
   
   Author: Mike Wall
   Date: 2/28/95
         revised 5/10/2013 (add P222 group for PSII)
   Version: 1.
   
   */

#include<mwmask.h>

/*
 * Subtract two vectors:
 */

struct ijkcoords lijksub(struct ijkcoords vec1, struct ijkcoords vec2)
{
  struct ijkcoords return_value;
  
  return_value.i = vec1.i - vec2.i;
  return_value.j = vec1.j - vec2.j;
  return_value.k = vec1.k - vec2.k;
  
  return(return_value);
}

/*
 * Add two vectors:
 */

struct ijkcoords lijksum(struct ijkcoords vec1, struct ijkcoords vec2)
{
  struct ijkcoords return_value;

  return_value.i = vec1.i + vec2.i;
  return_value.j = vec1.j + vec2.j;
  return_value.k = vec1.k + vec2.k;

  return(return_value);
}

/*
 * Rotate a vector about k-direction:
 */

struct ijkcoords lijkrotk(struct ijkcoords vec, float cos_theta, 
			  float sin_theta)
{
  struct ijkcoords return_value;

  return_value.i = vec.i * cos_theta - vec.j * sin_theta;
  return_value.j = vec.i * sin_theta + vec.j * cos_theta;
  return_value.k = vec.k;

  return(return_value);
}

/*
 * Invert a vector through the origin:
 */

struct ijkcoords lijkinv(struct ijkcoords vec)
{
  struct ijkcoords return_value;

  return_value.i = -vec.i;
  return_value.j = -vec.j;
  return_value.k = -vec.k;

  return(return_value);
}

/*
 * Reflect a vector through the ij-plane:
 */

struct ijkcoords lijkmij(struct ijkcoords vec)
{
  struct ijkcoords return_value;

  return_value.i = vec.i;
  return_value.j = vec.j;
  return_value.k = -vec.k;

  return(return_value);
}

/*
 * Symmetry operations:
 */

int lP1(LAT3D *lat)
{
  int return_value = 0;

  /*
   * Generate rotations:
   */

  /*
   * Generate reflections:
   */

  /*
   * Generate Friedel mates:
   */

  lat->symvec[1] = lijkinv(lat->symvec[0]);

  lat->symop_count = 2;
}

int lP41(LAT3D *lat)
{
  int return_value = 0;

  /*
   * Generate rotations:
   */

  lat->symvec[1] = lijkrotk(lat->symvec[0],0,1);
  lat->symvec[2] = lijkrotk(lat->symvec[1],0,1);
  lat->symvec[3] = lijkrotk(lat->symvec[2],0,1);

  /*
   * Generate reflections:
   */

  lat->symvec[4] = lijkmij(lat->symvec[0]);
  lat->symvec[5] = lijkmij(lat->symvec[1]);
  lat->symvec[6] = lijkmij(lat->symvec[2]);
  lat->symvec[7] = lijkmij(lat->symvec[3]);

  /*
   * Generate Friedel mates:
   */

  lat->symvec[8] = lijkinv(lat->symvec[0]);
  lat->symvec[9] = lijkinv(lat->symvec[1]);
  lat->symvec[10] = lijkinv(lat->symvec[2]);
  lat->symvec[11] = lijkinv(lat->symvec[3]);
  lat->symvec[12] = lijkinv(lat->symvec[4]);
  lat->symvec[13] = lijkinv(lat->symvec[5]);
  lat->symvec[14] = lijkinv(lat->symvec[6]);
  lat->symvec[15] = lijkinv(lat->symvec[7]);

  lat->symop_count = 16;
}

int lP222(LAT3D *lat)
{
  int return_value = 0;

  /*
   * Generate group:
   */

  lat->symvec[1].i = +lat->symvec[0].i;
  lat->symvec[1].j = -lat->symvec[0].j;
  lat->symvec[1].k = -lat->symvec[0].k;

  lat->symvec[2].i = -lat->symvec[0].i;
  lat->symvec[2].j = +lat->symvec[0].j;
  lat->symvec[2].k = -lat->symvec[0].k;

  lat->symvec[3].i = -lat->symvec[0].i;
  lat->symvec[3].j = -lat->symvec[0].j;
  lat->symvec[3].k = +lat->symvec[0].k;

  /*
   * Generate reflections:
   */

  /*
   * Generate Friedel mates:
   */

  lat->symvec[4] = lijkinv(lat->symvec[0]);
  lat->symvec[5] = lijkinv(lat->symvec[1]);
  lat->symvec[6] = lijkinv(lat->symvec[2]);
  lat->symvec[7] = lijkinv(lat->symvec[3]);

  lat->symop_count = 8;
}

int lsymlt(LAT3D *lat)
{
  size_t
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
	    lattice[lat_index[0]] = ((float)ct*lattice[lat_index[0]] +
				   lat->lattice[lat_index[op_index]])/
				     (float)(ct + 1);
	    ct++;
	  }
	}
	if (ct == 0) lattice[lat_index[0]] = lat->mask_tag;
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








