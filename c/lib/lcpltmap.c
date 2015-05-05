/* LCPLTMAP.C - Copy a lattice to a ccp4 map
   
   Author: Mike Wall
   Date: 2/13/2014; 4/23/2015
   Version: 1.
   
   */

#include<mwmask.h>

int lcpltmap(LAT3D *lat,CCP4MAP *map)
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
   * Copy data
   */  

  if (map->map_length != lat->lattice_length) {
    perror("Map and lattice are different sizes\n\n");
    exit(0);
  }

  if (map->mapc == 3 && map->mapr == 1 && map->maps == 2) {
    for (s = 0; s < map->ns; s++) {
      j = s;
      for (r = 0; r < map->nr; r++) {
	i = r;
	for (c = 0; c < map->nc; c++) {
	  k = c;
	  lat_index = k*lat->xyvoxels+j*lat->xvoxels+i;
	  map->data[map_index] = lat->lattice[lat_index];
	  map_index++;
	}
      }
    } 
  } else if (map->mapc == 3 && map->mapr == 2 && map->maps == 1) {
    for (s = 0; s < map->ns; s++) {
      i = s;
      for(r = 0; r < map->nr; r++) {
	j = r;
	for (c = 0; c < map->nc; c++) {
	  k = c;
	  lat_index = k*lat->xyvoxels+j*lat->xvoxels+i;
	  map->data[map_index] = lat->lattice[lat_index];
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





