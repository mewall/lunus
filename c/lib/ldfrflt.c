/* LDFRFLT.C - Calculate an R-factor rfile for two input lattices.
   
   Author: Mike Wall
   Date: 7/9/95
   Version: 1.
   
   */

#include<mwmask.h>

int ldfrflt(LAT3D *lat1, LAT3D *lat2)
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
    rscale,
    diffn;

  struct xyzcoords
    rfloat;

  /*
   * Allocate counting array:
   */

  ct = (size_t *)calloc(MAX_RFILE_LENGTH,sizeof(size_t));
  if (!ct) {
    sprintf(lat1->error_msg,"\nLAVGRLT: Couldn't allocate counting "
	    "array ct.\n\n");
    return_value = 1;
    goto CloseShop;
  }

  rscale = (lat1->xscale*lat1->xscale + lat1->yscale*lat1->yscale +
		 lat1->zscale*lat1->zscale);
  lat1->rfile_length = 0;
  for(k = 0; k < lat1->zvoxels; k++) {
    for(j = 0; j < lat1->yvoxels; j++) {
      for (i = 0; i < lat1->xvoxels; i++) {
	rvec.i = i - lat1->origin.i;
	rvec.j = j - lat1->origin.j;
	rvec.k = k - lat1->origin.k;
	rfloat.x = lat1->xscale * rvec.i;
	rfloat.y = lat1->yscale * rvec.j;
	rfloat.z = lat1->zscale * rvec.k;
	r = (size_t)(sqrtf((rfloat.x*rfloat.x + rfloat.y*rfloat.y + 
		       rfloat.z*rfloat.z) / rscale)+.5);
	if (r > lat1->rfile_length) lat1->rfile_length = r;
	if ((lat1->lattice[index] != lat1->mask_tag) &&
	    (lat2->lattice[index] != lat1->mask_tag)) {
	  diffn = fabsf(lat1->lattice[index] -
		   lat2->lattice[index]) / (fabsf(lat1->lattice[index])
					    +fabsf(lat2->lattice[index]));
/*printf("%f %f\n",lat1->lattice[index],lat2->lattice[index]);/***/
	  if (ct[r] == 0) {
	    lat1->rfile[r] = diffn;
	    ct[r] = 1;
	  } else {
	    lat1->rfile[r] = (ct[r]*lat1->rfile[r] +
				 diffn)/(float)(ct[r]+1);
	    ct[r]++;
	  }
	}
	index++;
      }
    }
  }
  free((size_t *)ct);
  CloseShop:
  return(return_value);
}





