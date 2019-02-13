/* LSCALELT.C - Find the best scale factor relating two input lattices.
   
   Author: Mike Wall
   Date: 3/21/95
   Version: 1.
   
   */

#include<mwmask.h>

int lscalelt(LAT3D *lat1, LAT3D *lat2)
{
  size_t
    ct = 0,
    i,
    j,
    k,
    r,
    index = 0;

  int
    return_value = 0;
  
  struct ijkcoords rvec;
  
  float
    xx = 0,
    xy = 0,
    avg_xx = 0,
    avg_xy = 0,
    rscale;

  struct xyzcoords
    rfloat;

  rscale = (lat1->xscale*lat1->xscale + lat1->yscale*lat1->yscale +
		 lat1->zscale*lat1->zscale);
  ct = 0;
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
	if ((lat1->lattice[index] != lat1->mask_tag) &&
	    (lat2->lattice[index] != lat1->mask_tag) &&
	    (r >= lat1->inner_radius) &&
	    (r <= lat1->outer_radius)) {
	  xx = lat1->lattice[index]*lat1->lattice[index];
	  xy = lat1->lattice[index]*lat2->lattice[index];
/*printf("%f %f\n",lat1->lattice[index],lat2->lattice[index]);/***/
	    if (ct == 0) {/***/
	      avg_xx = xx;
	      avg_xy = xy;
	      ct = 1;/***/
	    } else {
	      avg_xx = (ct*avg_xx + xx)/(float)(ct + 1);
	      avg_xy = (ct*avg_xy + xy)/(float)(ct + 1);
	      ct++;
	    }/***/
	}
	index++;
      }
    }
  }
  lat1->rfile[0] = avg_xx/avg_xy;
  CloseShop:
  return(return_value);
}





