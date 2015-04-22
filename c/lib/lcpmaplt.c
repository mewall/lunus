/* LCPMAPLT.C - Copy a map to a lattice
   
   Author: Mike Wall
   Date: 2/13/2014
   Version: 1.
   
   */

#include<mwmask.h>

int lcpmaplt(CCP4MAP *map, LAT3D *lat)
{
  size_t
    i,
    j,
    k,
    c,
    r,
    s,
    map_index = 0,
    lat_index = 0;

  int
    return_value = 0;

  /* 
   * Check map properties:
   */

  // Only works if map mode = 2 (float data type)

  if (map->mode != 2) {
    printf("\nUnrecognized CCP4 map MODE = %d\n\n",map->mode);
    return_value = 3;
    goto CloseShop;
  }

  /*
   * Set lattice header info
   */

  lat->xvoxels = map->nr; // MAPR = 1
  lat->yvoxels = map->ns; // MAPS = 2
  lat->zvoxels = map->nc; // MAPC = 3
  lat->xyvoxels = lat->xvoxels*lat->yvoxels;
  lat->lattice_length = lat->xyvoxels*lat->zvoxels;
  // Reallocate lattice
  if (lat->lattice) {free(lat->lattice);}
  lat->lattice = (LATTICE_DATA_TYPE *)calloc(lat->lattice_length,
					     sizeof(LATTICE_DATA_TYPE)); 
  if (!lat->lattice) {
    printf("\nFailed to allocate lattice\n\n");
    return_value = 6;
    goto CloseShop;
  }
  lat->xscale = map->xlen/(float)lat->xvoxels;
  lat->yscale = map->ylen/(float)lat->yvoxels;
  lat->zscale = map->zlen/(float)lat->zvoxels;
  lat->origin.i = lat->origin.j = lat->origin.k = 0;
  lat->xbound.min = - lat->origin.i*lat->xscale;               
  lat->xbound.max = (lat->xvoxels - lat->origin.i - 1)*lat->xscale;
  lat->ybound.min = - lat->origin.j*lat->yscale; 
  lat->ybound.max = (lat->yvoxels - lat->origin.j - 1)*lat->yscale;
  lat->zbound.min = - lat->origin.k*lat->zscale; 
  lat->zbound.max = (lat->zvoxels - lat->origin.k - 1)*lat->zscale;

  /*
   * Copy data
   */  

  if (map->mapc == 3 || map->mapr == 1 || map->maps == 2) {
    for (s = 0; s < map->ns; s++) {
      j = s;
      for (r = 0; r < map->nr; r++) {
	i = r;
	for (c = 0; c < map->nc; c++) {
	  k = c;
	  lat_index = k*lat->xyvoxels+j*lat->xvoxels+i;
	  lat->lattice[lat_index] = map->data[map_index];
	  map_index++;
	}
      }
    } 
  } else if (map->mapc == 3 || map->mapr == 2 || map->maps == 1) {
    for (s = 0; s < map->ns; s++) {
      i = s;
      for(r = 0; r < map->nr; r++) {
	j = r;
	for (c = 0; c < map->nc; c++) {
	  k = c;
	  lat_index = k*lat->xyvoxels+j*lat->xvoxels+i;
	  lat->lattice[lat_index] = map->data[map_index];
	  map_index++;
	}
      }
    } 
  } else {
    printf("\nUnrecognized CCP4 map x,y,z definitions (MAPC,MAPR,MAPS) = (%d,%d,%d)\n\n",map->mapc,map->mapr,map->maps);
    return_value = 4;
    exit(return_value);
  }

  if (map_index != map->map_length) {
    printf("\nNumber of elements copied not equal to map length\n\n");
    return_value = 5;
    goto CloseShop;
  }

  CloseShop:
  return(return_value);
}





