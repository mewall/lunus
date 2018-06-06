/* LWRITEHKL.C - Write a lattice to a .hkl file.

   Author: Mike Wall
   Date: 2/4/14
   Version: 1.

*/

#include<mwmask.h>

int lwritehkl(LAT3D *lat)
{

  int
    return_value = 0;

/*
 * Write lattice to output file:
 */

  int index = 0,ct=0;
  int i,j,k,hh,kk,ll;
  int hmax,kmax,lmax;
 
  /*
  if (lat->origin.i >= lat->xvoxels/2) {
    hmax = lat->xvoxels-lat->origin.i;
  } else {
    hmax = lat->origin.i;
  }
  if (lat->origin.j >= lat->yvoxels/2) {
    kmax = lat->yvoxels-lat->origin.j;
  } else {
    kmax = lat->origin.j;
  }
  if (lat->origin.k >= lat->zvoxels/2) {
    lmax = lat->zvoxels-lat->origin.k;
  } else {
    lmax = lat->origin.k;
  }
  */

  hmax = lat->xvoxels-lat->origin.i;
  kmax = lat->yvoxels-lat->origin.j;
  lmax = lat->zvoxels-lat->origin.k;

  for (i=0;i<lat->xvoxels;i++) {
    hh = i - lat->origin.i;
    for (j=0;j<lat->yvoxels;j++) {
      kk = j - lat->origin.j;
      for (k=0;k<lat->zvoxels;k++) {
	ll = k - lat->origin.k;
	index = k*lat->yvoxels*lat->xvoxels+j*lat->xvoxels+i;
	if (index > lat->lattice_length) {
	  printf("\nLWRITEHKL: Index out of bounds. Value = %d\n\n",index);
	  return_value = 1;
	  goto CloseShop;
	}
	if (abs(hh)<=hmax && abs(kk)<=kmax && abs(ll) <= lmax) {
	  if (lat->lattice[index] != lat->mask_tag) {
	    fprintf(lat->outfile, "%4d %4d %4d   %10.2f\n",hh,kk,ll,lat->lattice[index]);
	  }
	}
	ct++;
      }
    }
  }
  

  if (ct != lat->lattice_length) {
    printf("/nCouldn't write all of the lattice to output file.\n\n");
    return_value = 1;
    goto CloseShop;
  }

  CloseShop:

  return(return_value);
}



