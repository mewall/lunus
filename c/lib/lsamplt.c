/* LSAMPLT.C - Downsample a lattice.
   
   Author: Mike Wall
   Date: 1/18/2017
   Version: 1.
   
   */

#include<mwmask.h>

int lsamplt(LAT3D *lat)
{
  size_t
    i,
    j,
    k,
    dmap_index,
    map_index = 0,
    lat_index = 0;

  long dfac;

  long 
    c,
    r,
    s;

  long
    return_value = 0;

  long dlen,nrc;

  LATTICE_DATA_TYPE
    *ddata;

  dfac = (long)lat->decimation_factor;

  printf("Sampling lattice using factor %ld\n",dfac);

  if (!((lat->xvoxels % dfac) == 0 && (lat->yvoxels % dfac) == 0 && (lat->zvoxels % dfac) == 0)) {
    printf("\nLSAMPLT: Map dimensions not a multiple of %d\n\n",dfac);
    return_value = 4;
    goto CloseShop;
  }

  /*
   * Allocate sampled lattice
   */

  dlen = lat->lattice_length/dfac/dfac/dfac;

  ddata = (LATTICE_DATA_TYPE *)calloc((size_t)dlen,sizeof(LATTICE_DATA_TYPE));

  /*
   * Calculate the sampled data
   */  


  int io=1;
  int jo=1;
  int ko=1;

  size_t dlat_index;

  printf("Sampling...\n");

  for (k = 0; k < lat->zvoxels; k++) {
    if ((k+ko)%dfac == 0) {
      for (j = 0; j < lat->yvoxels; j++) {
	if ((j+jo)%dfac == 0) {
	  for (i = 0; i < lat->xvoxels; i++) {
	    if ((i+io)%dfac == 0) {
	      dlat_index = (((k-ko)*lat->xyvoxels/dfac+(j-jo)*lat->xvoxels)/dfac+i-io)/dfac;
	      lat_index = k*lat->xyvoxels+j*lat->xvoxels+i;
	      ddata[dlat_index] = lat->lattice[lat_index];
	    }
	  }
	} 
      }
    }
  }

  printf("Done.\n");

  // Replace original lattice with sampled lattice
  
  lat->lattice_length=dlen;
  lat->lattice=(LATTICE_DATA_TYPE *)realloc(lat->lattice,sizeof(LATTICE_DATA_TYPE)*dlen);
  memcpy((void *)lat->lattice,(void *)ddata,dlen*sizeof(LATTICE_DATA_TYPE));
  lat->xvoxels /= dfac;
  lat->yvoxels /= dfac;
  lat->zvoxels /= dfac;
  lat->xyvoxels /= dfac*dfac;
  lat->xscale *= dfac;
  lat->yscale *= dfac;
  lat->zscale *= dfac;

  lat->origin.i = (IJKCOORDS_DATA)(lat->xvoxels/2 - 1);
  lat->origin.j = (IJKCOORDS_DATA)(lat->yvoxels/2 - 1);
  lat->origin.k = (IJKCOORDS_DATA)(lat->zvoxels/2 - 1);

  lat->xbound.min = -lat->origin.i*lat->xscale;
  lat->ybound.min = -lat->origin.j*lat->yscale;
  lat->zbound.min = -lat->origin.k*lat->zscale;

  lat->xbound.max = lat->xbound.min + ((float)lat->xvoxels-1)*lat->xscale;
  lat->ybound.max = lat->ybound.min + ((float)lat->yvoxels-1)*lat->yscale;
  lat->zbound.max = lat->zbound.min + ((float)lat->zvoxels-1)*lat->zscale;
     
  CloseShop:
  return(return_value);
}





