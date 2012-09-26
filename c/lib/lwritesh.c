/* LWRITESH.C - Write a shell image to a file.

   Author: Mike Wall
   Date: 9/27/95
   Version: 1.

*/

#include<mwmask.h>

int lwritesh(LAT3D *lat)
{

  size_t
    num_wrote;

  int
    return_value = 0;

/*
 * Reverse byte order if neccessary:
 */

  if (DOS_BYTE_ORDER!=0) {
        lchbyte(lat->shim, sizeof(SHIM_DATA_TYPE), lat->shim_length);
  }

/*
 * Write shell image to output file:
 */

  num_wrote = fwrite(lat->shim, sizeof(SHIM_DATA_TYPE), 
			lat->shim_length, lat->outfile);

  if (num_wrote != lat->shim_length) {
    printf("/nCouldn't write all of the shell image to output file.\n\n");
    return_value = 1;
    goto CloseShop;
  }

  CloseShop:

  return(return_value);
}



