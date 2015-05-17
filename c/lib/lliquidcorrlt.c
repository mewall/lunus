/* LLIQUIDCORRLT.C - Generate a liquid-like motions correlation lattice.
   
   Author: Mike Wall
   Date: 2/28/95
   Version: 1.
   
   */

#include<mwmask.h>

int lliquidcorrlt(LAT3D *lat)
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

  float sum=0;
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
	rsqr = (rfloat.x*rfloat.x + rfloat.y*rfloat.y + 
		       rfloat.z*rfloat.z);
	lat->lattice[lat_index] = (LATTICE_DATA_TYPE)
	  4.*PI*lat->width*lat->width*lat->width/(1.+lat->width*lat->width*2.*PI*2.*PI*rsqr);
	sum += lat->lattice[lat_index];
	  // Clarage et al 1992 model:	  
	  //	  	  rsqr*2.*PI*2.*PI*lat->width*lat->width*expf(-rsqr*2.*PI*2.*PI*lat->width*lat->width);
	// Modfied clarage model:
	//(1.0 - expf(-rsqr*2.*PI*2.*PI*lat->width*lat->width))*expf(-rsqr*2.*PI*2.*PI*lat->width*lat->width);
	  // Wall et al 1997 model:
	//(1.0 - expf(-rsqr*2.*PI*2.*PI*lat->width*lat->width));
	lat_index++;
      }
    }
  }

  int i;
  for (i=0;i<lat->lattice_length;i++) {
    lat->lattice[i] /= sum;
  }

  CloseShop:
  return(return_value);
}


