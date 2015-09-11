/* LSHIMLT.C - Generate an image of a reduced angular distribution of
              intensity in a specified spherical shell of a lattice.
   
   Author: Mike Wall
   Date: 3/23/95
   Version: 1.
   
   */

#include<mwmask.h>

int lshimlt(LAT3D *lat)
{
  size_t
    ct,
    ii,
    jj,
    kk,
    i,
    j,
    r,
    lattice_index,
    index = 0;

  int
    return_value = 0;
  
  struct ijkcoords rvec;
  
  struct rccoords shim_origin;

  float
    theta_scale,
    phi_scale,
    theta,
    phi,
    rscale;

  struct xyzcoords
    rfloat;

  theta_scale = 2*PI/(float)lat->shim_hsize;
  phi_scale = PI/(float)lat->shim_vsize;
  shim_origin.r = lat->shim_vsize/2 - 1;
  shim_origin.c = lat->shim_hsize/2 - 1;
  rscale = sqrtf(lat->xscale*lat->xscale + lat->yscale*lat->yscale +
		 lat->zscale*lat->zscale);
  index = 0;
  for(j = 0; j < lat->shim_vsize; j++) {
    phi = ((float)j - (float)shim_origin.r) * phi_scale;
    for (i = 0; i < lat->shim_hsize; i++) {
      ct = 0;
      for(r = lat->inner_radius;r <= lat->outer_radius; r++) {
/*
 * Divide by 4 in x (8/7/95):
 */
/*
 * Remove divide by 4 (7/9/14):
 */

//	theta = ((float)i/4. - (float)shim_origin.c) * theta_scale;
	theta = ((float)i - (float)shim_origin.c) * theta_scale;
	rfloat.x = (float)r*rscale*cosf(theta)*cosf(phi);
	rfloat.y = (float)r*rscale*sinf(theta)*cosf(phi);
	rfloat.z = (float)r*rscale*sinf(phi);
	if (rfloat.x >=0) {
	  rvec.i = (IJKCOORDS_DATA)(rfloat.x / lat->xscale + .5);
	} else {
	  rvec.i = (IJKCOORDS_DATA)(rfloat.x / lat->xscale - .5);
	}
	if (rfloat.y >=0) {
	  rvec.j = (IJKCOORDS_DATA)(rfloat.y / lat->yscale + .5);
	} else {
	  rvec.j = (IJKCOORDS_DATA)(rfloat.y / lat->yscale - .5);
	}
	if (rfloat.z >=0) {
	  rvec.k = (IJKCOORDS_DATA)(rfloat.z / lat->zscale + .5);
	} else {
	  rvec.k = (IJKCOORDS_DATA)(rfloat.z / lat->zscale - .5);
	}
	ii = rvec.i + lat->origin.i;
	jj = rvec.j + lat->origin.j;
	kk = rvec.k + lat->origin.k;
	if ((ii >= 0) && (ii < lat->xvoxels) && 
	    (jj >= 0) && (jj < lat->yvoxels) &&
	    (kk >= 0) && (kk < lat->zvoxels)) {
	  lattice_index = kk*lat->xyvoxels + jj*lat->xvoxels + ii;
	  /*printf("%d,%d,%d -- %d\n",ii,jj,kk,(SHIM_DATA_TYPE)lat->lattice[lattice_index]);/***/
	  if (ct == 0) {
	    lat->shim[index] = (SHIM_DATA_TYPE)lat->lattice[lattice_index];
	    ct = 1;
	  } else {
	    lat->shim[index] = (SHIM_DATA_TYPE)((ct*(float)lat->shim[index] +
						 lat->lattice[lattice_index])/(float)(ct+1));
	    ct++;
	  }
	}
      }
      index++;
    }
  }

  CloseShop:
  return(return_value);
}





