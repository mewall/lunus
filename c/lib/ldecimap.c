/* LDECIMAP.C - Decimate a map.
   
   Author: Mike Wall
   Date: 5/4/2015
   Version: 1.
   
   */

#include<mwmask.h>

long ldecimap(CCP4MAP *map)
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

  MAP_DATA_TYPE
    *ddata;

  /* 
   * Check map properties:
   */

  // Only works if map mode = 2 (float data type)

  if (map->mode != 2) {
    printf("\nLDECIMAP: Unrecognized CCP4 map MODE = %d\n\n",map->mode);
    return_value = 3;
    goto CloseShop;
  }

  dfac = (long)map->decimation_factor;

  printf("Decimating map using factor %ld\n",dfac);

  if (!((map->nr % dfac) == 0 && (map->ns % dfac) == 0 && (map->nc % dfac) == 0)) {
    printf("\nLDECIMAP: Map dimensions not a multiple of %d\n\n",dfac);
    return_value = 4;
    goto CloseShop;
  }

  /*
   * Allocate decimated map
   */

  dlen = map->map_length/dfac/dfac/dfac;

  ddata = (MAP_DATA_TYPE *)calloc((size_t)dlen,sizeof(MAP_DATA_TYPE));

  /*
   * Calculate the decimated data
   */  

  nrc=map->nr*map->nc;
  for (s = 0; s < map->ns; s=s+dfac) {
    for (r = 0; r < map->nr; r=r+dfac) {
      for (c = 0; c < map->nc; c=c+dfac) {
	long ss,rr,cc;
	dmap_index = ((s*nrc/dfac+r*map->nc)/dfac+c)/dfac;
	for (ss=s-dfac+1;ss<=s;ss++) {
	  long sidx = (ss+map->ns)%map->ns;
	  printf("sidx = %d\n",sidx);
	  for (rr=r-dfac+1;rr<=r;rr++) {
	    long ridx = (rr+map->nr)%map->nr;
	    for (cc=c-dfac+1;cc<=c;cc++) {
	      long cidx = (cc+map->nc)%map->nc;
	      map_index = sidx*nrc+ridx*map->nc+cidx;
	      ddata[dmap_index]+=map->data[map_index];
	    }
	  }
	} 
      }
    }
  }

  // Replace original map with decimated map
  
  map->map_length=dlen;
  map->data=realloc(map->data,sizeof(MAP_DATA_TYPE)*dlen);
  memcpy((void *)map->data,(void *)ddata,dlen*sizeof(MAP_DATA_TYPE));
  map->nr /= dfac;
  map->ns /= dfac;
  map->nc /= dfac;
  map->nx /= dfac;
  map->ny /= dfac;
  map->nz /= dfac;
     
  CloseShop:
  return(return_value);
}





