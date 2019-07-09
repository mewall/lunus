/* AVGSQIM.C - Calculate the average squared pixel value for an input image.
   
   Author: Mike Wall   
   Date: 1/23/95
   Version: 1.
   
   "avgsqim <input image>"

   Input is TIFF TV6 image.  Output is a number which is the average
   squared pixel value in the image.

   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*imagein;
  
  char
    error_msg[LINESIZE];

  size_t
    num_wrote,
    num_read;

  DIFFIMAGE 
	*imdiff;

  struct rccoords origin;

/*
 * Set input line defaults:
 */
	
	imagein = stdin;

/*
 * Read information from input line:
 */
	switch(argc) {
		case 2:
			if (strcmp(argv[1], "-") == 0) {
				imagein = stdin;
			}
			else {
			 if ( (imagein = fopen(argv[1],"rb")) == NULL ) {
				printf("Can't open %s.",argv[1]);
				exit(0);
			 }
			}
			break;
		default:
			printf("\n Usage: avgsqim <input image>\n\n");
			exit(0);
	}
  

/*
 * Initialize diffraction image:
 */

  if ((imdiff = linitim(1)) == NULL) {
    perror("\nCouldn't initialize diffraction image.\n\n");
    exit(0);
  }

/*
 * Read diffraction image:
 */
	imdiff->infile = imagein;
	if (lreadim(imdiff) != 0) {
		perror(imdiff->error_msg);
		goto CloseShop;
	}


  /*
   * Calculate average:
   */
  
	lavgsqim(imdiff);  

/*
 * Write the result:
 */

  printf("%e\n",imdiff->avg_pixel_value);

CloseShop:
  
/*
 * Free allocated memory:
 */

  lfreeim(imdiff);

/*
 * Close files:
 */
  
  fclose(imagein);
  
}

