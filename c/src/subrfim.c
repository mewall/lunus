/* SUBRFIM.C - Subtract I(r) from a diffraction image.
   
   Author: Mike Wall
   Date: 4/4/94
   Version: 1.
   
   "subrfim <input file> <image in> <image out> <scale>"

   Input is rfile and image.  Output is 16-bit 
   image with rfile, multipled by scale, subtracted.

   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*infile,
	*imagein,
	*imageout;
  
  size_t
    i,
    num_wrote,
    num_read;

  DIFFIMAGE 
	*imdiff;

  struct rccoords
	origin;

  float
    scale;

/*
 * Set input line defaults:
 */
	
	imagein = stdin;
	imageout = stdout;
	scale = 1;
/*
 * Read information from input line:
 */
	switch(argc) {
		case 5:
			scale = atof(argv[4]);

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
			if ( (infile = fopen(argv[1],"rb")) == NULL ) {
				printf("Can't open %s.",argv[1]);
				exit(0);
			}
			break;
		default:
			printf("\n Usage: subrfim <input file> "
				"<image in> <image out> <scale>\n\n");
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
  * Read in rfile:
  */

  imdiff->infile = infile;
  if (lreadrf(imdiff) != 0) {
    perror(imdiff->error_msg);
    goto CloseShop;
  }

/*
 * Scale rfile:
 */

   lscalerf(imdiff, scale);


/*
 * Subtract rfile from image:
 */

   lsubrfim(imdiff);

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
  
  fclose(infile);
  fclose(imagein);
  fclose(imageout);
  
}

