/* MULSCIM.C - Multiply an image by a scalar value.
   
   Author: Mike Wall
   Date: 10/25/2016
   Version: 1.
   
   "mulscim <input image> <output image> <scalar value>" 

   Input diffraction image and scalar value.  Output is image multiplied by scalar.

   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*imagein,
	*imageout;
  
  size_t
    i;

  DIFFIMAGE 
    *imdiff;

  float scalar;

/*
 * Read information from input line:
 */
	switch(argc) {
	case 4:
	  scalar = (float)atof(argv[3]);
		case 3:
			if (strcmp(argv[2], "-") == 0) {
				imageout = stdout;
			}
			else {
			 if ( (imageout = fopen(argv[2],"wb")) == NULL ) {
				printf("Can't open %s.",argv[2]);
				exit(0);
			 }
			}
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
			printf("\n Usage: mulscim <input image> "
				"<output image> <scalar value>\n\n");
			exit(0);
	}
/*
 * Initialize diffraction images:
 */

	if (((imdiff = linitim()) == NULL)) {
    perror("Couldn't initialize diffraction images.\n\n");
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
 * Set scalar value:
 */

	imdiff->amplitude = scalar;

  if (lmulscim(imdiff) != 0) {
    perror(imdiff->error_msg);
    goto CloseShop;
  }


/*
 * Write the output image:
 */

  imdiff->outfile = imageout;
  if(lwriteim(imdiff) != 0) {
    perror(imdiff->error_msg);
    goto CloseShop;
  }

CloseShop:
  
/*
 * Free allocated memory:
 */

  lfreeim(imdiff);

/*
 * Close files:
 */
  
  fclose(imagein);
  fclose(imageout);
}

