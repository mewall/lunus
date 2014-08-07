/* LSYMLIB.C - Library of functions important for symmetry operations.
   
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

struct ijkcoords lijkmjk(struct ijkcoords vec)
{
  struct ijkcoords return_value;

  return_value.i = -vec.i;
  return_value.j = vec.j;
  return_value.k = vec.k;

  return(return_value);
}
struct ijkcoords lijkmki(struct ijkcoords vec)
{
  struct ijkcoords return_value;

  return_value.i = vec.i;
  return_value.j = -vec.j;
  return_value.k = vec.k;

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









