/* LMINRLT.C - Calculate the minimum voxel value vs. radius for an 
              input lattice
   
   Author: Mike Wall
   Date: 2/23/2017
   Version: 1.
   
   */

#include<mwmask.h>

int lminrlt(LAT3D *lat)
{
  size_t
    *ct,
    i,
    j,
    k,
    r,
    index = 0;

  int
    return_value = 0;
  
  struct ijkcoords rvec;
  
  float
    rscale;

  struct xyzcoords
    rfloat;

  /*
   * Allocate counting array:
   */

  ct = (size_t *)calloc(MAX_RFILE_LENGTH,sizeof(size_t));
  if (!ct) {
    sprintf(lat->error_msg,"\nLAVGRLT: Couldn't allocate counting "
	    "array ct.\n\n");
    return_value = 1;
    goto CloseShop;
  }

  rscale = (lat->xscale*lat->xscale + lat->yscale*lat->yscale +
		 lat->zscale*lat->zscale);
  lat->rfile_length = 0;
  for(k = 0; k < lat->zvoxels; k++) {
    for(j = 0; j < lat->yvoxels; j++) {
      for (i = 0; i < lat->xvoxels; i++) {
	rvec.i = i - lat->origin.i;
	rvec.j = j - lat->origin.j;
	rvec.k = k - lat->origin.k;
	rfloat.x = lat->xscale * rvec.i;
	rfloat.y = lat->yscale * rvec.j;
	rfloat.z = lat->zscale * rvec.k;
	r = (size_t)(sqrtf((rfloat.x*rfloat.x + rfloat.y*rfloat.y + 
		       rfloat.z*rfloat.z) / rscale)+.5);
	if (lat->lattice[index] != lat->mask_tag) {
	  //	  if (lat->lattice[index]<0) printf("%d,%f\n",(int)index,lat->lattice[index]);
	  if (r >= lat->rfile_length) lat->rfile_length = r;
	  if (ct[r] == 0) {
	    lat->rfile[r] = lat->lattice[index];
	    ct[r] = 1;
	  } else {
	    if (lat->rfile[r] > lat->lattice[index]) {
	      lat->rfile[r] = lat->lattice[index];
	    }
	  }
	}
	index++;
      }
    }
  }
  // If there are no counts at a given radius, mask the value

  for (r=0;r<lat->rfile_length;r++) {
    if (ct[r]==0) {
      lat->rfile[r]=lat->mask_tag;
    }
  }

  free((size_t *)ct);
  CloseShop:
  return(return_value);
}





