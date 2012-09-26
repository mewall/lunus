/* LSUBRFLT.C - Subtract a radial intensity distribution from each pixel
		in a 3D lattice.
   
   Author: Mike Wall
   Date: 3/28/93
   Version: 1.
   
   */

#include<mwmask.h>

int lsubrflt(LAT3D *lat)
{
  size_t
    i,
    j,
    k,
    r,
    index = 0;
  
  float
    rf,
    rscale;

  struct xyzcoords
    rfloat;
  
  struct ijkcoords rvec;
  
  rscale = (lat->xscale*lat->xscale + lat->yscale*lat->yscale +
		 lat->zscale*lat->zscale);
  for(k = 0; k < lat->zvoxels; k++) {
    for(j = 0; j < lat->yvoxels; j++) {
      for (i = 0; i < lat->xvoxels; i++) {
	rvec.i = i - lat->origin.i;
	rvec.j = j - lat->origin.j;
	rvec.k = k - lat->origin.k;
	rfloat.x = lat->xscale * rvec.i;
	rfloat.y = lat->yscale * rvec.j;
	rfloat.z = lat->zscale * rvec.k;
	rf = sqrtf((rfloat.x*rfloat.x + rfloat.y*rfloat.y + 
		       rfloat.z*rfloat.z) / rscale);
	r = (size_t)rf;
	if ((r < lat->rfile_length) && 
	    (lat->lattice[index] != lat->mask_tag)) {
	  lat->lattice[index] -= (lat->rfile[r] + 
				  (rf - (float)r)*(lat->rfile[r+1] - 
						   lat->rfile[r]));
	}
	index++;
      }
    }
  }
}


