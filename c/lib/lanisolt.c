/* LANISOLT.C - Extract the anisotropic component of a lattice.
   
   Author: Mike Wall
   Date: 9/27/2016
   Version: 1.
   
   */

#include<mwmask.h>

void lanisolt(LAT3D *lat)
{

  size_t *ct,r,lat_index;

  float rf,rscale;

  struct ijkcoords index;

  ct = (size_t *)calloc(MAX_RFILE_LENGTH,sizeof(size_t));
  if (!ct) {
    sprintf(lat->error_msg,"\nLLMLT: Couldn't allocate counting "
	    "array ct.\n\n");
    //    return_value = 1;
    //    goto CloseShop;
  }

  lat->rfile_length = 0;
  lat->index.i=lat->origin.i+1;
  lat->index.j=lat->origin.j+1;
  lat->index.k=lat->origin.k+1;
  rscale=lssqrFromIndex(lat);
  //  rscale = (lat->xscale*lat->xscale + lat->yscale*lat->yscale +
  //	    lat->zscale*lat->zscale);
  // calculate isotropic component
  lat_index = 0;
  for(index.k = 0; index.k < lat->zvoxels; index.k++) {
    for(index.j = 0; index.j < lat->yvoxels; index.j++) {
      for (index.i = 0; index.i < lat->xvoxels; index.i++) {
	lat->index = index;
	
	r = (size_t)(sqrtf(lssqrFromIndex(lat) / rscale)+.5);
	if (r > lat->rfile_length) lat->rfile_length = r;
	if (lat->lattice[lat_index] != lat->mask_tag) {
	  //	  if (lat->lattice[index]<0) printf("%d,%f\n",(int)index,lat->lattice[index]);
	  if (ct[r] == 0) {
	    lat->rfile[r] = lat->lattice[lat_index];
	    ct[r] = 1;
	  } else {
	    lat->rfile[r] = ((float)ct[r]*lat->rfile[r] +
			     lat->lattice[lat_index])/(float)(ct[r]+1);
	    ct[r]++;
	  }
	}
	lat_index++;
      }
    }
  }

  // subtract isotropic component

  //  size_t maxr=40;
  lat_index = 0;
  for(index.k = 0; index.k < lat->zvoxels; index.k++) {
    for(index.j = 0; index.j < lat->yvoxels; index.j++) {
      for (index.i = 0; index.i < lat->xvoxels; index.i++) {
	lat->index = index;
	rf = sqrtf(lssqrFromIndex(lat) / rscale);
	r = (size_t)rf;
	if ((r < lat->rfile_length) && 
	    (lat->lattice[lat_index] != lat->mask_tag)) {
	  //    lat->lattice[index] -= lat->rfile[r];
	  //	  if (r <= maxr) {
	    lat->lattice[lat_index] -= (lat->rfile[r] + 
				  (rf - (float)r)*(lat->rfile[r+1] - 
						   lat->rfile[r]));
	    //	  } else {
	    //	    lat->lattice[lat_index] = lat->mask_tag;
	    //	  }
	}

	lat_index++;
      }
    }
  }
  free((size_t *)ct);
}

