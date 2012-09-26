/* LREADIM.C - Read a diffraction image from a file.

   Author: Mike Wall
   Date: 9/27/95
   Version: 1.

*/

#include<mwmask.h>

int lreadim(DIFFIMAGE *imdiff)
{
	size_t
		num_read;

	int
		return_value = 0;  
  /*
   * Read diffraction image header
   */


  num_read = fread(imdiff->header, sizeof(char), imdiff->header_length,
                imdiff->infile);
  if (num_read != imdiff->header_length) {
  	return_value = 1;
	sprintf(imdiff->error_msg,"\nCouldn't read all of header.\n\n");
  }
  if (ferror(imdiff->infile) != 0) {
  	return_value = 2;
  	sprintf(imdiff->error_msg,"\nError while reading header.\n\n");
  }

  /*
   * Read image:
   */

  num_read = fread(imdiff->image, sizeof(IMAGE_DATA_TYPE),
                        imdiff->image_length, imdiff->infile);
  if (num_read != imdiff->image_length) {
	return_value = 3;
	sprintf(imdiff->error_msg,"\nCouldn't read all of image.\n\n");
  }
  if (ferror(imdiff->infile) != 0) {
    return_value = 4;
    sprintf(imdiff->error_msg,"\nError while reading image\n\n");
  }

/*
 * Reverse byte order if neccessary:
 */

  if (DOS_BYTE_ORDER!=0) {
        lchbyte(imdiff->image, sizeof(IMAGE_DATA_TYPE), imdiff->image_length);
  }

  return(return_value);
}
