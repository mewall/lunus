/* LMINRLT.C - Generate a table of minimum voxel values as a function of radius
		for an input 3D lattice.
   
   Author: Mike Wall
   Date: 3/28/93
   Version: 1.
   
   */

#include<mwmask.h>

int lminrlt(LAT3D *lat)
{
	size_t
		i,
		j,
		k,
		r,
		index = 0;

	struct ijkcoords rvec;

	lat->rfile_length = 0;
	for(k = 0; k < lat->zvoxels; k++) {
	  for(j = 0; j < lat->yvoxels; j++) {
	    for (i = 0; i < lat->xvoxels; i++) {
	      rvec.i = i - lat->origin.i;
	      rvec.j = j - lat->origin.j;
	      rvec.k = k - lat->origin.k;
	      r = (size_t)sqrtf((float)(rvec.i*rvec.i + rvec.j*rvec.j + 
					rvec.k*rvec.k));
	      if (r > lat->rfile_length) lat->rfile_length = r;
	      if ((lat->lattice[index] != lat->mask_tag) && 
		((lat->rfile[r] >= lat->lattice[index]) || 
			(lat->rfile[r] == lat->mask_tag))) {
		lat->rfile[r] = lat->lattice[index];
	      }
	    index++;
	    }
	  }
	}
}
