/* LGAUSSLT.C - Generate a gaussian lattice.
   
   Author: Mike Wall
   Date: 2/28/95
   Version: 1.
   
   */

#include<mwmask.h>

int lgausslt(LAT3D *lat)
{
  size_t
    lat_index = 0;
  
  int
    return_value = 0;

  struct ijkcoords 
    rvec,
    index;

  float
    rscale,
    factor,
    rsqr;

  struct xyzcoords
    rfloat;

  factor = lat->peak / (sqrt(2*PI)*(2*PI)) /
    lat->width/lat->width/lat->width;
  rscale = (lat->xscale*lat->xscale + lat->yscale*lat->yscale +
		 lat->zscale*lat->zscale);
  for(index.k = 0; index.k < lat->zvoxels; index.k++) {
    for(index.j = 0; index.j < lat->yvoxels; index.j++) {
      for (index.i = 0; index.i < lat->xvoxels; index.i++) {
	rvec.i = index.i - lat->origin.i;
	rvec.j = index.j - lat->origin.j;
	rvec.k = index.k - lat->origin.k;
	rfloat.x = lat->xscale * rvec.i;
	rfloat.y = lat->yscale * rvec.j;
	rfloat.z = lat->zscale * rvec.k;
	rsqr = ((rfloat.x*rfloat.x + rfloat.y*rfloat.y + 
		       rfloat.z*rfloat.z) / rscale);
	lat->lattice[lat_index] = (LATTICE_DATA_TYPE)
	  factor*expf(-rsqr/(2.*lat->width*lat->width));
	lat_index++;
      }
    }
  }

  CloseShop:
  return(return_value);
}


