/* LWRITEMAP.C - Write a CCP4 map

   Author: Mike Wall
   Date: 2/23/2014
   Version: 1.

*/

#include<mwmask.h>

int lwritemap(CCP4MAP *map)
{

  size_t
    num_wrote;

  void 
    *header;

  float 
    *header_float;

  int32_t
    *header_int;

  int
    return_value = 0;

  // Calculate map statistics

  int i;
  map->amin=0;
  map->amax=0;
  map->amean=0;
  map->arms=0;
  for (i = 0;i<map->map_length;i++) {
    if (map->data[i]<map->amin) map->amin=map->data[i];
    if (map->data[i]>map->amax) map->amax=map->data[i];
    map->amean += map->data[i];
    map->arms += map->data[i]*map->data[i];
  }
  map->amean /= (MAP_DATA_TYPE)map->map_length;
  map->arms = sqrtf(map->arms/(MAP_DATA_TYPE)map->map_length-map->amean*map->amean);

  /*
   * Write map header info:
   */
  //  if (map->header) {free(map->header);}
  map->header = (void *)calloc(1024,sizeof(void)); // 256 4-byte words
  header_float = (float *)map->header;
  header_int = (int32_t *)map->header;
  header_int[0] = map->nc;
  header_int[1] = map->nr;
  header_int[2] = map->ns;
  header_int[3] = map->mode;
  header_int[4] = map->ncstart;
  header_int[5] = map->nrstart;
  header_int[6] = map->nsstart;
  header_int[7] = map->nx;
  header_int[8] = map->ny;
  header_int[9] = map->nz;
  header_float[10] = map->xlen;
  header_float[11] = map->ylen;
  header_float[12] = map->zlen;
  header_float[13] = map->alpha;
  header_float[14] = map->beta;
  header_float[15] = map->gamma;
  header_int[16] = map->mapc;
  header_int[17] = map->mapr;
  header_int[18] = map->maps;
  header_float[19] = map->amin;
  header_float[20] = map->amax;
  header_float[21] = map->amean;
  header_int[22] = map->ispg;
  header_int[23] = map->nsymbt;
  sprintf((char *)&header_float[52],"MAP ");
  header_int[53] = map->machst;
  header_float[54] = map->arms;
  
  // Write the header:

  num_wrote = fwrite(map->header,sizeof(void),1024,map->outfile);

  /*
   * Write symmetry record:
   */

  if (map->nsymbt>0) {
    num_wrote = fwrite(map->symrec_buf, sizeof(void), map->nsymbt, map->outfile);
  }

  // Write the map data:

  num_wrote = fwrite(map->data_buf, sizeof(void),sizeof(MAP_DATA_TYPE)* 
		   map->map_length,map->outfile);

  CloseShop:
  return(return_value);
}
