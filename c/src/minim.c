/* MINIM.C - Calculate the minimum intensity for an input image.
   
   Author: Mike Wall   
   Date: 9/14/2017
   Version: 1.
   
   "minim <input image>"

   Input is diffraction image.  Output is the minimum pixel value in the image, discounting mask pixels. 

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
			printf("\n Usage: minim <input image>\n\n");
			exit(0);
	}
  

/*
 * Initialize diffraction image:
 */

  if ((imdiff = linitim()) == NULL) {
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
   * Calculate minimum:
   */
  
	lminim(imdiff);  

/*
 * Write the result:
 */

  printf("%d\n",imdiff->min_pixel_value);

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

