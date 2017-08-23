/* GETRIM.C - Print all intensities at a certain radius, in arbitrary order.
   
   Author: Veronica Pillar   
   Date: 4/21/14
   Version: 1.
   
   "getrim <input image> <target radius>"

   Input is TIFF TV6 image and target radius. Prints to stdout.

   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*imagein;
  
  char
    error_msg[LINESIZE];

  size_t
    target;

  DIFFIMAGE 
	*imdiff;

  struct rccoords origin;

/*
 * Set input line defaults:
 */
	
	imagein = stdin;
	target = 0;

/*
 * Read information from input line:
 */
	switch(argc) {
		case 3:
			target = atoi(argv[2]);
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
			printf("\n Usage: getrim <input image> "
				"<target radius>\n\n");
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
   * Print values:
   */
  
	lgetrim(imdiff, target);  

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

