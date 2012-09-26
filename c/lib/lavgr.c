/* LAVGR.C - Generate a table of average voxel values as a function of radius
		for an input 3D lattice.
   
   Author: Mike Wall
   Date: 3/28/93
   Version: 1.

*/

#include<mwmask.h>

int lavgr(LAT3D *lat)
{
	size_t
		*n,
		i,
		j,
		k,
		r,
		index = 0;

	struct ijkcoords rvec;

	n = (size_t *)calloc(MAX_RFILE_LENGTH, sizeof(size_t));
	lat->origin.i = (IJKCOORDS_DATA)((-lat->xbound.min / (lat->xbound.max - 
				lat->xbound.min)) * lat->xvoxels);
	lat->origin.j = (IJKCOORDS_DATA)((-lat->ybound.min / (lat->ybound.max - 
				lat->ybound.min)) * lat->yvoxels);
	lat->origin.k = (IJKCOORDS_DATA)((-lat->zbound.min / (lat->zbound.max - 
				lat->zbound.min)) * lat->zvoxels);
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
	      if (lat->lattice[index] != lat->mask_tag) {
		lat->rfile[r] = (lat->lattice[index] + n[r]*lat->rfile[r]) /
				(LATTICE_DATA_TYPE)(n[r] + 1);
		n[r]++;
	      }
	    index++;
	    }
	  }
	}
}
