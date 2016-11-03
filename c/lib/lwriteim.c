/* LWRITEIM.C - Write a diffraction image to a file.

   Author: Mike Wall
   Date: 9/27/95
   Version: 1.

*/

#include<mwmask.h>

int lwriteim(DIFFIMAGE *imdiff)
{
	size_t
		num_wrote;

	int
		return_value = 0;  


/*
 * Reverse byte order if neccessary:
 */

  if (DOS_BYTE_ORDER!=0 || imdiff->big_endian!=0) {
        lchbyte(imdiff->image, sizeof(IMAGE_DATA_TYPE), imdiff->image_length);
  }

/*
 * Write image header
 */


  num_wrote = fwrite(imdiff->header, sizeof(char), imdiff->header_length,
                        imdiff->outfile);
  if (num_wrote != imdiff->header_length) {
  	return_value = 1;
	sprintf(imdiff->error_msg,"\nCouldn't write all of header.\n\n");
  }
  if (ferror(imdiff->outfile) != 0) {
  	return_value = 2;
	sprintf(imdiff->error_msg,"\nError while writing header.\n\n");
  }

/*
 * Write image:
 */

  num_wrote = fwrite(imdiff->image, sizeof(IMAGE_DATA_TYPE), 
		imdiff->image_length, imdiff->outfile);

  if (num_wrote != imdiff->image_length) {
	return_value = 3;
	sprintf(imdiff->error_msg,"\nCouldn't write all of image.\n\n");
  }
  if (ferror(imdiff->outfile) != 0) {
	return_value = 4;
	sprintf(imdiff->error_msg,"\nError while writing image\n\n");
  }


  return(return_value);
}
