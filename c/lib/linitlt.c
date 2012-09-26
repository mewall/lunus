/* LINITLT.C - Initialize a lattice.

   Author: Mike Wall
   Date: 9/27/95
   Version: 1.

*/

#include<mwmask.h>

LAT3D *linitlt(void) 
{

  LAT3D *lat;

  size_t
    index;
  
  lat = (LAT3D *)malloc(sizeof(LAT3D));
  if (!lat) {
    printf("\nLINITLT:  Unable to allocate all memory (lat).\n");
    lat = NULL;
    goto CloseShop;
  }

  lat->wavelength = DEFAULT_WAVELENGTH;
  lat->inner_radius = DEFAULT_LAT_INNER_RADIUS;
  lat->outer_radius = DEFAULT_LAT_OUTER_RADIUS;
  lat->mask_tag = DEFAULT_LATTICE_MASK_TAG;
  lat->xvoxels = DEFAULT_XVOXELS;
  lat->yvoxels = DEFAULT_YVOXELS;
  lat->zvoxels = DEFAULT_ZVOXELS;
  lat->minrange.x = DEFAULT_MINRANGE;
  lat->minrange.y = DEFAULT_MINRANGE;
  lat->minrange.z = DEFAULT_MINRANGE;
  lat->origin.i = lat->origin.j = lat->origin.k = DEFAULT_LATTICE_ORIGIN;
  lat->cell.a = DEFAULT_CELL_A;
  lat->cell.b = DEFAULT_CELL_B;
  lat->cell.c = DEFAULT_CELL_C;
  lat->xscale = 1./lat->cell.a;
  lat->yscale = 1./lat->cell.b;
  lat->zscale = 1./lat->cell.c;
  lat->xbound.min = - lat->origin.i*lat->xscale;               
  lat->xbound.max = (lat->xvoxels - lat->origin.i - 1)*lat->xscale;
  lat->ybound.min = - lat->origin.j*lat->yscale; 
  lat->ybound.max = (lat->yvoxels - lat->origin.j - 1)*lat->yscale;
  lat->zbound.min = - lat->origin.k*lat->zscale; 
  lat->zbound.max = (lat->zvoxels - lat->origin.k - 1)*lat->zscale;
  lat->xyvoxels = lat->xvoxels * lat->yvoxels;
  lat->lattice_length = lat->xyvoxels*lat->zvoxels;
  lat->rfile_length = 0;
  lat->shim_hsize = lat->shim_vsize = DEFAULT_SHIM_SIZE;
  lat->shim_length = lat->shim_hsize * lat->shim_vsize;

  lat->rfile = (RFILE_DATA_TYPE *)calloc(MAX_RFILE_LENGTH, 
					sizeof(RFILE_DATA_TYPE));
  lat->lattice = (LATTICE_DATA_TYPE *)calloc(lat->lattice_length,
					     sizeof(LATTICE_DATA_TYPE)); 
  lat->shim = (SHIM_DATA_TYPE *)calloc(lat->shim_length,
				       sizeof(SHIM_DATA_TYPE));
  if (!lat->lattice || !lat->rfile || !lat->shim) {
    printf("\nLINITLT:  Unable to allocate all memory.\n");
    lat = NULL;
    goto CloseShop;
  }

  for (index = 0;index < lat->lattice_length;index++) {
    lat->lattice[index] = lat->mask_tag;
  }

  CloseShop:

  return(lat);

}












