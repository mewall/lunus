/* LEXPLT.C - Create an exponential decay lattice.
   
   Author: Mike Wall
   Date: 3/21/95
   Version: 1.
   
   */

#include<mwmask.h>

int lexplt(LAT3D *lat)
{
  size_t
    i,
    j,
    k,
    r,
    index = 0;

  int
    return_value = 0;
  
  struct ijkcoords rvec;
  
  float
    rf,
    rscale;

  struct xyzcoords
    rfloat;

  /*
   * Allocate counting array:
   */


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
	rf = sqrtf(rfloat.x*rfloat.x + rfloat.y*rfloat.y + rfloat.z*rfloat.z);
	//	r = (size_t)(sqrtf((rfloat.x*rfloat.x + rfloat.y*rfloat.y + 
	//		       rfloat.z*rfloat.z) / rscale)+.5);
	//	if (r > lat->rfile_length) lat->rfile_length = r;
	if (lat->lattice[index] != lat->mask_tag) {
	  lat->lattice[index] =	expf(-rf*lat->rfile[0]); 
	}
	index++;
      }
    }
  }
  CloseShop:
  return(return_value);
}





