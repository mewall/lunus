/* LSYMLIB.C - Library of functions important for symmetry operations.
   
   Author: Mike Wall
   Date: 2/28/95
         revised 5/10/2013 (add P222 group for PSII)
   Modified by Alex Wolff to generalize to all Laue classes. Added 6/16/2017 MEW
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
 * Rotate a vector about k-axis:
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
 * Reflect a vector in a mirror that runs in the ij-plane:
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
 * Reflect a vector in a mirror that runs in the jk-plane:
 */

struct ijkcoords lijkmjk(struct ijkcoords vec)
{
  struct ijkcoords return_value;

  return_value.i = -vec.i;
  return_value.j = vec.j;
  return_value.k = vec.k;

  return(return_value);
}

/*
 * Reflect a vector in a mirror that runs in the ki-plane:
 */

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

int lLaue1(LAT3D *lat)
{
  int return_value = 0;

  /*
   * Generate Friedel mates:
   */

  lat->symvec[1] = lijkinv(lat->symvec[0]);

  lat->symop_count = 2;
}


int lLaue2(LAT3D *lat)
{
  int return_value = 0;

  /*
   * Generate group:
   */

  lat->symvec[1].i = +lat->symvec[0].i;
  lat->symvec[1].j = -lat->symvec[0].j;
  lat->symvec[1].k = +lat->symvec[0].k;

  /*
   * Generate Friedel mates:
   */

  lat->symvec[2] = lijkinv(lat->symvec[0]);
  lat->symvec[3] = lijkinv(lat->symvec[1]);

  lat->symop_count = 4;
}


int lLaue3(LAT3D *lat)
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
   * Generate Friedel mates:
   */

  lat->symvec[4] = lijkinv(lat->symvec[0]);
  lat->symvec[5] = lijkinv(lat->symvec[1]);
  lat->symvec[6] = lijkinv(lat->symvec[2]);
  lat->symvec[7] = lijkinv(lat->symvec[3]);

  lat->symop_count = 8;
}

int lLaue4(LAT3D *lat)
{
  int return_value = 0;

  /*
   * Generate rotations:
   */

  lat->symvec[1].i = -lat->symvec[0].i;
  lat->symvec[1].j = -lat->symvec[0].j;
  lat->symvec[1].k = +lat->symvec[0].k;

  lat->symvec[2].i = -lat->symvec[0].j;
  lat->symvec[2].j = +lat->symvec[0].i;
  lat->symvec[2].k = +lat->symvec[0].k;

  lat->symvec[3].i = +lat->symvec[0].j;
  lat->symvec[3].j = -lat->symvec[0].i;
  lat->symvec[3].k = +lat->symvec[0].k;

  /*
   * Generate Friedel mates:
   */

  lat->symvec[4] = lijkinv(lat->symvec[0]);
  lat->symvec[5] = lijkinv(lat->symvec[1]);
  lat->symvec[6] = lijkinv(lat->symvec[2]);
  lat->symvec[7] = lijkinv(lat->symvec[3]);

  lat->symop_count = 8;
}

int lLaue5(LAT3D *lat)
{
  int return_value = 0;

  /*
   * Generate rotations:
   */

  lat->symvec[1].i = -lat->symvec[0].i;
  lat->symvec[1].j = -lat->symvec[0].j;
  lat->symvec[1].k = +lat->symvec[0].k;

  lat->symvec[2].i = -lat->symvec[0].j;
  lat->symvec[2].j = +lat->symvec[0].i;
  lat->symvec[2].k = +lat->symvec[0].k;

  lat->symvec[3].i = +lat->symvec[0].j;
  lat->symvec[3].j = -lat->symvec[0].i;
  lat->symvec[3].k = +lat->symvec[0].k;

  lat->symvec[4].i = -lat->symvec[0].i;
  lat->symvec[4].j = +lat->symvec[0].j;
  lat->symvec[4].k = -lat->symvec[0].k;

  lat->symvec[5].i = +lat->symvec[0].i;
  lat->symvec[5].j = -lat->symvec[0].j;
  lat->symvec[5].k = -lat->symvec[0].k;

  lat->symvec[6].i = +lat->symvec[0].j;
  lat->symvec[6].j = +lat->symvec[0].i;
  lat->symvec[6].k = -lat->symvec[0].k;

  lat->symvec[7].i = -lat->symvec[0].j;
  lat->symvec[7].j = -lat->symvec[0].i;
  lat->symvec[7].k = -lat->symvec[0].k;

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

int lLaue6(LAT3D *lat)
{
  int return_value = 0;

  /*
   * Generate rotations:
   */

  lat->symvec[1].i = -lat->symvec[0].j;
  lat->symvec[1].j = +lat->symvec[0].i-lat->symvec[0].j;
  lat->symvec[1].k = +lat->symvec[0].k;

  lat->symvec[2].i = -lat->symvec[0].i+lat->symvec[0].j;
  lat->symvec[2].j = -lat->symvec[0].i;
  lat->symvec[2].k = +lat->symvec[0].k;

  /*
   * Generate Friedel mates:
   */

  lat->symvec[3] = lijkinv(lat->symvec[0]);
  lat->symvec[4] = lijkinv(lat->symvec[1]);
  lat->symvec[5] = lijkinv(lat->symvec[2]);

  lat->symop_count = 6;
}

int lLaue7(LAT3D *lat)
{
  int return_value = 0;

  /*
   * Generate rotations:
   */

  lat->symvec[1].i = -lat->symvec[0].j;
  lat->symvec[1].j = +lat->symvec[0].i-lat->symvec[0].j;
  lat->symvec[1].k = +lat->symvec[0].k;

  lat->symvec[2].i = -lat->symvec[0].i+lat->symvec[0].j;
  lat->symvec[2].j = -lat->symvec[0].i;
  lat->symvec[2].k = +lat->symvec[0].k;

  lat->symvec[3].i = -lat->symvec[0].j;
  lat->symvec[3].j = -lat->symvec[0].i;
  lat->symvec[3].k = -lat->symvec[0].k;

  lat->symvec[4].i = -lat->symvec[0].i+lat->symvec[0].j;
  lat->symvec[4].j = +lat->symvec[0].j;
  lat->symvec[4].k = -lat->symvec[0].k;

  lat->symvec[5].i = +lat->symvec[0].i;
  lat->symvec[5].j = +lat->symvec[0].i-lat->symvec[0].j;
  lat->symvec[5].k = -lat->symvec[0].k;

  /*
   * Generate Friedel mates:
   */

  lat->symvec[6] = lijkinv(lat->symvec[0]);
  lat->symvec[7] = lijkinv(lat->symvec[1]);
  lat->symvec[8] = lijkinv(lat->symvec[2]);
  lat->symvec[9] = lijkinv(lat->symvec[3]);
  lat->symvec[10] = lijkinv(lat->symvec[4]);
  lat->symvec[11] = lijkinv(lat->symvec[5]);


  lat->symop_count = 12;
}

int lLaue8(LAT3D *lat)
{
  int return_value = 0;

  /*
   * Generate rotations:
   */

  lat->symvec[1].i = -lat->symvec[0].j;
  lat->symvec[1].j = +lat->symvec[0].i-lat->symvec[0].j;
  lat->symvec[1].k = +lat->symvec[0].k;

  lat->symvec[2].i = -lat->symvec[0].i+lat->symvec[0].j;
  lat->symvec[2].j = -lat->symvec[0].i;
  lat->symvec[2].k = +lat->symvec[0].k;

  lat->symvec[3].i = -lat->symvec[0].i;
  lat->symvec[3].j = -lat->symvec[0].j;
  lat->symvec[3].k = +lat->symvec[0].k;

  lat->symvec[4].i = +lat->symvec[0].j;
  lat->symvec[4].j = -lat->symvec[0].i+lat->symvec[0].j;
  lat->symvec[4].k = +lat->symvec[0].k;

  lat->symvec[5].i = +lat->symvec[0].i-lat->symvec[0].j;
  lat->symvec[5].j = +lat->symvec[0].i;
  lat->symvec[5].k = +lat->symvec[0].k;

  /*
   * Generate Friedel mates:
   */

  lat->symvec[6] = lijkinv(lat->symvec[0]);
  lat->symvec[7] = lijkinv(lat->symvec[1]);
  lat->symvec[8] = lijkinv(lat->symvec[2]);
  lat->symvec[9] = lijkinv(lat->symvec[3]);
  lat->symvec[10] = lijkinv(lat->symvec[4]);
  lat->symvec[11] = lijkinv(lat->symvec[5]);


  lat->symop_count = 12;
}

int lLaue9(LAT3D *lat)
{
  int return_value = 0;

  /*
   * Generate rotations:
   */

  lat->symvec[1].i = -lat->symvec[0].j;
  lat->symvec[1].j = +lat->symvec[0].i-lat->symvec[0].j;
  lat->symvec[1].k = +lat->symvec[0].k;

  lat->symvec[2].i = -lat->symvec[0].i+lat->symvec[0].j;
  lat->symvec[2].j = -lat->symvec[0].i;
  lat->symvec[2].k = +lat->symvec[0].k;

  lat->symvec[3].i = -lat->symvec[0].i;
  lat->symvec[3].j = -lat->symvec[0].j;
  lat->symvec[3].k = +lat->symvec[0].k;

  lat->symvec[4].i = +lat->symvec[0].j;
  lat->symvec[4].j = -lat->symvec[0].i+lat->symvec[0].j;
  lat->symvec[4].k = +lat->symvec[0].k;

  lat->symvec[5].i = +lat->symvec[0].i-lat->symvec[0].j;
  lat->symvec[5].j = +lat->symvec[0].i;
  lat->symvec[5].k = +lat->symvec[0].k;

  lat->symvec[6].i = +lat->symvec[0].j;
  lat->symvec[6].j = +lat->symvec[0].i;
  lat->symvec[6].k = -lat->symvec[0].k;

  lat->symvec[7].i = +lat->symvec[0].i-lat->symvec[0].j;
  lat->symvec[7].j = -lat->symvec[0].j;
  lat->symvec[7].k = -lat->symvec[0].k;

  lat->symvec[8].i = -lat->symvec[0].i;
  lat->symvec[8].j = -lat->symvec[0].i+lat->symvec[0].j;
  lat->symvec[8].k = -lat->symvec[0].k;

  lat->symvec[9].i = -lat->symvec[0].j;
  lat->symvec[9].j = -lat->symvec[0].i;
  lat->symvec[9].k = -lat->symvec[0].k;

  lat->symvec[10].i = -lat->symvec[0].i+lat->symvec[0].j;
  lat->symvec[10].j = +lat->symvec[0].j;
  lat->symvec[10].k = -lat->symvec[0].k;

  lat->symvec[11].i = +lat->symvec[0].i;
  lat->symvec[11].j = +lat->symvec[0].i-lat->symvec[0].j;
  lat->symvec[11].k = -lat->symvec[0].k;

  /*
   * Generate Friedel mates:
   */

int i;

  for (i=0;i<12;i++) {
    lat->symvec[i+12]=lijkinv(lat->symvec[i]);
  }

  lat->symop_count = 24;
}

int lLaue10(LAT3D *lat)
{
  int return_value = 0;

  /*
   * Generate group:
   */

  lat->symvec[1].i = -lat->symvec[0].i;
  lat->symvec[1].j = -lat->symvec[0].j;
  lat->symvec[1].k = +lat->symvec[0].k;

  lat->symvec[2].i = -lat->symvec[0].i;
  lat->symvec[2].j = +lat->symvec[0].j;
  lat->symvec[2].k = -lat->symvec[0].k;

  lat->symvec[3].i = +lat->symvec[0].i;
  lat->symvec[3].j = -lat->symvec[0].j;
  lat->symvec[3].k = -lat->symvec[0].k;

  lat->symvec[4].i = +lat->symvec[0].k;
  lat->symvec[4].j = +lat->symvec[0].i;
  lat->symvec[4].k = +lat->symvec[0].j;

  lat->symvec[5].i = +lat->symvec[0].k;
  lat->symvec[5].j = -lat->symvec[0].i;
  lat->symvec[5].k = -lat->symvec[0].j;

  lat->symvec[6].i = -lat->symvec[0].k;
  lat->symvec[6].j = -lat->symvec[0].i;
  lat->symvec[6].k = +lat->symvec[0].j;

  lat->symvec[7].i = -lat->symvec[0].k;
  lat->symvec[7].j = +lat->symvec[0].i;
  lat->symvec[7].k = -lat->symvec[0].j;

  lat->symvec[8].i = +lat->symvec[0].j;
  lat->symvec[8].j = +lat->symvec[0].k;
  lat->symvec[8].k = +lat->symvec[0].i;

  lat->symvec[9].i = -lat->symvec[0].j;
  lat->symvec[9].j = +lat->symvec[0].k;
  lat->symvec[9].k = -lat->symvec[0].i;

  lat->symvec[10].i = +lat->symvec[0].j;
  lat->symvec[10].j = -lat->symvec[0].k;
  lat->symvec[10].k = -lat->symvec[0].i;

  lat->symvec[11].i = -lat->symvec[0].j;
  lat->symvec[11].j = -lat->symvec[0].k;
  lat->symvec[11].k = +lat->symvec[0].i;

  /*
   * Generate Friedel mates:
   */

  int i;

  for (i=0;i<12;i++) {
    lat->symvec[i+12]=lijkinv(lat->symvec[i]);
  }

  lat->symop_count = 24;
}

int lLaue11(LAT3D *lat)
{
  int return_value = 0;

  /*
   * Generate rotations:
   */

  lat->symvec[1].i = -lat->symvec[0].i;
  lat->symvec[1].j = -lat->symvec[0].j;
  lat->symvec[1].k = +lat->symvec[0].k;

  lat->symvec[2].i = -lat->symvec[0].i;
  lat->symvec[2].j = +lat->symvec[0].j;
  lat->symvec[2].k = -lat->symvec[0].k;

  lat->symvec[3].i = +lat->symvec[0].i;
  lat->symvec[3].j = -lat->symvec[0].j;
  lat->symvec[3].k = -lat->symvec[0].k;

  lat->symvec[4].i = +lat->symvec[0].k;
  lat->symvec[4].j = +lat->symvec[0].i;
  lat->symvec[4].k = +lat->symvec[0].j;

  lat->symvec[5].i = +lat->symvec[0].k;
  lat->symvec[5].j = -lat->symvec[0].i;
  lat->symvec[5].k = -lat->symvec[0].j;

  lat->symvec[6].i = -lat->symvec[0].k;
  lat->symvec[6].j = -lat->symvec[0].i;
  lat->symvec[6].k = +lat->symvec[0].j;

  lat->symvec[7].i = -lat->symvec[0].k;
  lat->symvec[7].j = +lat->symvec[0].i;
  lat->symvec[7].k = -lat->symvec[0].j;

  lat->symvec[8].i = +lat->symvec[0].j;
  lat->symvec[8].j = +lat->symvec[0].k;
  lat->symvec[8].k = +lat->symvec[0].i;

  lat->symvec[9].i = -lat->symvec[0].j;
  lat->symvec[9].j = +lat->symvec[0].k;
  lat->symvec[9].k = -lat->symvec[0].i;

  lat->symvec[10].i = +lat->symvec[0].j;
  lat->symvec[10].j = -lat->symvec[0].k;
  lat->symvec[10].k = -lat->symvec[0].i;

  lat->symvec[11].i = -lat->symvec[0].j;
  lat->symvec[11].j = -lat->symvec[0].k;
  lat->symvec[11].k = +lat->symvec[0].i;

  lat->symvec[12].i = +lat->symvec[0].j;
  lat->symvec[12].j = +lat->symvec[0].i;
  lat->symvec[12].k = -lat->symvec[0].k;

  lat->symvec[13].i = -lat->symvec[0].j;
  lat->symvec[13].j = -lat->symvec[0].i;
  lat->symvec[13].k = -lat->symvec[0].k;

  lat->symvec[14].i = -lat->symvec[0].j;
  lat->symvec[14].j = +lat->symvec[0].i;
  lat->symvec[14].k = -lat->symvec[0].k;

  lat->symvec[15].i = -lat->symvec[0].j;
  lat->symvec[15].j = +lat->symvec[0].i;
  lat->symvec[15].k = +lat->symvec[0].k;

  lat->symvec[16].i = +lat->symvec[0].i;
  lat->symvec[16].j = +lat->symvec[0].k;
  lat->symvec[16].k = -lat->symvec[0].j;

  lat->symvec[17].i = -lat->symvec[0].i;
  lat->symvec[17].j = +lat->symvec[0].k;
  lat->symvec[17].k = +lat->symvec[0].j;

  lat->symvec[18].i = -lat->symvec[0].i;
  lat->symvec[18].j = -lat->symvec[0].k;
  lat->symvec[18].k = -lat->symvec[0].j;

  lat->symvec[19].i = +lat->symvec[0].i;
  lat->symvec[19].j = -lat->symvec[0].k;
  lat->symvec[19].k = +lat->symvec[0].j;

  lat->symvec[20].i = +lat->symvec[0].k;
  lat->symvec[20].j = +lat->symvec[0].j;
  lat->symvec[20].k = -lat->symvec[0].i;

  lat->symvec[21].i = +lat->symvec[0].k;
  lat->symvec[21].j = -lat->symvec[0].j;
  lat->symvec[21].k = +lat->symvec[0].i;

  lat->symvec[22].i = -lat->symvec[0].k;
  lat->symvec[22].j = +lat->symvec[0].j;
  lat->symvec[22].k = +lat->symvec[0].i;

  lat->symvec[23].i = -lat->symvec[0].k;
  lat->symvec[23].j = -lat->symvec[0].j;
  lat->symvec[23].k = -lat->symvec[0].i;

  /*
   * Generate Friedel mates:
   */

  int i;

  for (i=0;i<24;i++) {
    lat->symvec[i+24]=lijkinv(lat->symvec[i]);
  }

  lat->symop_count = 48;
}









