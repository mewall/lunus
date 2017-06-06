/* MULCFIM.C - Multiply correction factors by an image pixel-by-pixel.
   
   Author: Mike Wall
   Date: 6/6/2017
   Version: 1.
   
   "mulcfim <input correction factor> <input image> <output image>" 

   Input correction factors and image. Output the product, pixel by pixels.

   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*cfin,
	*imagein,
	*imageout;
  
  size_t
    i,num_read;

  DIFFIMAGE 
    *imdiff;


/*
 * Set input line defaults:
 */
	
	cfin = stdin;
	imagein = stdin;
	imageout = stdout;
/*
 * Read information from input line:
 */
	switch(argc) {
		case 4:
			if (strcmp(argv[3], "-") == 0) {
				imageout = stdout;
			}
			else {
			 if ( (imageout = fopen(argv[3],"wb")) == NULL ) {
				printf("Can't open %s.",argv[3]);
				exit(0);
			 }
			}
		case 3:
			if (strcmp(argv[2], "-") == 0) {
				imagein = stdin;
			}
			else {
			 if ( (imagein = fopen(argv[2],"rb")) == NULL ) {
				printf("Can't open %s.",argv[2]);
				exit(0);
			 }
			}
		case 2:
			if (strcmp(argv[1], "-") == 0) {
				cfin = stdin;
			}
			else {
			 if ( (cfin = fopen(argv[1],"rb")) == NULL ) {
				printf("Can't open %s.",argv[1]);
				exit(0);
			 }
			}
			break;
		default:
			printf("\n Usage: mulim <input image 1> "
				"<x origin 1> <y origin 1> <input image 2> "
				"<x origin 2> <y origin 2> "
				"<output image>\n\n");
			exit(0);
	}
/*
 * Initialize diffraction images:
 */

  if ((imdiff = linitim()) == NULL) {
    perror("Couldn't initialize diffraction images.\n\n");
    exit(0);
  }

// Allocate correction factor

  imdiff->correction = (float *)malloc(imdiff->image_length*sizeof(float));

 
 /*
  * Read diffraction image:
  */

  imdiff->infile = imagein;
  if (lreadim(imdiff) != 0) {
    perror(imdiff->error_msg);
    goto CloseShop;
  }

  // Read correction factor

  num_read = fread(imdiff->correction, sizeof(float), imdiff->image_length,
		 cfin);

  if (lmulcfim(imdiff) != 0) {
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
  fclose(cfin);
  fclose(imageout);
}

