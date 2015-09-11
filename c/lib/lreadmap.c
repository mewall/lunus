/* LREADMAP.C - Read a CCP4 map from a file into a lattice data type.

   Author: Mike Wall
   Date: 2/6/2014
   Version: 1.

*/

#include<mwmask.h>

int lreadmap(CCP4MAP *map)
{

  size_t
    num_read;

  void 
    *header;

  float 
    *header_float;

  int32_t
    *header_long;

  int
    return_value = 0;

  /*
   * Read in CCP4 map header info:
   */
  map->header = (void *)realloc(map->header,1024); // 256 4-byte words
  header_float = (float *)map->header;
  header_long = (int32_t *)map->header;
  num_read = fread(map->header, sizeof(void), 1024, map->infile);
  map->nc = (long) header_long[0];
  map->nr = (long) header_long[1];
  map->ns = (long) header_long[2];
  map->mode = (long) header_long[3];
  map->ncstart = (long) header_long[4];
  map->nrstart = (long) header_long[5];
  map->nsstart = (long) header_long[6];
  map->nx = (long) header_long[7];
  map->ny = (long) header_long[8];
  map->nz = (long) header_long[9];
  map->xlen = (float) header_float[10];
  map->ylen = (float) header_float[11];
  map->zlen = (float) header_float[12];
  map->alpha = (float) header_float[13];
  map->beta = (float) header_float[14];
  map->gamma = (float) header_float[15];
  map->mapc = (long) header_long[16];
  map->mapr = (long) header_long[17];
  map->maps = (long) header_long[18];
  map->amin = (float) header_float[19];
  map->amax = (float) header_float[20];
  map->amean = (float) header_float[21];
  map->ispg = (long) header_long[22];
  map->nsymbt = (long) header_long[23];
  map->machst = (long) header_long[53];
  map->section_length = map->nc*map->nr;
  map->map_length = map->section_length*map->ns;
  map->arms = (float)header_float[54];

  /*
   * Read symmetry record:
   */

  if (map->nsymbt>0) {
    map->symrec_buf = (void *)realloc(map->symrec_buf,map->nsymbt*sizeof(void));
    num_read = fread(map->symrec_buf, sizeof(void), map->nsymbt, map->infile);
    map->symrec = (char *)map->symrec_buf;
  }


  /*
   * Read the map data:
   */
  
  map->data_buf = (void *)realloc(map->data_buf,map->map_length*
					     sizeof(MAP_DATA_TYPE));
  if (!map->data_buf) {
    printf("\nCouldn't allocate CCP4 map data.\n\n");
    return_value = 1;
    goto CloseShop;
  }
  
  num_read = fread(map->data_buf, sizeof(void),sizeof(MAP_DATA_TYPE)* 
		   map->map_length,map->infile);

  /*
  int i;
  for (i=0;i<lat->lattice_length;i++) {
    if (lat->lattice[i] != lat->mask_tag && lat->lattice[i] < 0) {
      printf("%d,%f\n",i,lat->lattice[i]);
    }
  }
		   */
  if (num_read != map->map_length*sizeof(MAP_DATA_TYPE)) {
    printf("/nCouldn't read all of the map from input file.\n\n");
    return_value = 2;
    goto CloseShop;
  }

  map->data = (MAP_DATA_TYPE *)map->data_buf;

  // Print some useful info from the header
#ifdef DEBUG
  printf("map->machst = %d\n",map->machst);
  printf("NC = %d\n",map->nc);
  printf("NR = %d\n",map->nr);
  printf("NS = %d\n",map->ns);
  // printf("NX = %d\n",map->nx);
  //  printf("NY = %d\n",map->ny);
  //  printf("NZ = %d\n",map->nz);
  printf("NCstart, NRstart, NSstart = %d, %d, %d\n",map->ncstart,map->nrstart,map->nsstart);
  printf("MODE = %d\n",map->mode);
  printf("XLEN = %f\n",map->xlen);
  printf("YLEN = %f\n",map->ylen);
  printf("ZLEN = %f\n",map->zlen);
  printf("MAPC = %d\n",map->mapc);
  printf("MAPR = %d\n",map->mapr);
  printf("MAPS = %d\n",map->maps);
  printf("NSYMBT = %d\n",map->nsymbt);
  printf("Symmetry record:\n\n%s\n",map->symrec);
  printf("First three data values: %f, %f, %f\n",map->data[0],map->data[1],map->data[2]);
  printf("RMSD = %f\n",map->arms);
#endif
  CloseShop:
  return(return_value);
}
