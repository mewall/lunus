/* SMOOTHIM.C - Smooth an image.
   
   Author: Veronica Pillar
   Date: 4/22/14
   Version: 1.
   
   "smoothim <image in> <image out> <box half-width>"

   Input is TIFF image and box size. Output is smoothed TIFF image.

   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*imagein,
	*imageout;
  
  DIFFIMAGE 
	*imdiff;

  size_t
    width;

/*
 * Set input line defaults:
 */
	
	imagein = stdin;
	imageout = stdout;
	width = 0;

/*
 * Read information from input line:
 */
	switch(argc) {
		case 4:
			width = atoi(argv[3]);
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
			printf("\n Usage: smoothim "
				"<image in> <image out> <box half-width>\n\n");
			exit(0);
	}
  
/*
 * Initialize diffraction image:
 */
  
  if ((imdiff = linitim()) == NULL) {
    perror("Couldn't initialize diffraction image.\n\n");
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
 * Smooth image:
 */

  lsmoothim(imdiff, width);

/*
 * Write the output image:
 */

  imdiff->outfile = imageout;
  if(lwriteim(imdiff) != 0) {
    perror(imdiff->error_msg);
    goto CloseShop;
  }

CloseShop:
  
  lfreeim(imdiff);

/*
 * Close files:
 */
  
  fclose(imagein);
  fclose(imageout);
  
}

