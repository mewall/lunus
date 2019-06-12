/* ABSIM.C - Calculate the absolute value of an image, treating pixels as signed.
   
   Author: Mike Wall
   Date: 10/25/2016
   Version: 1.
   
   "absim <input image> <output image>" 

   Input diffraction image.  Output is image with absolute value pixels.

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
			printf("\n Usage: absim <input image> "
				"<output image>\n\n");
			exit(0);
	}
/*
 * Initialize diffraction images:
 */

	if (((imdiff = linitim(1)) == NULL)) {
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


  // Calculate absolute value

  if (labsim(imdiff) != 0) {
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

