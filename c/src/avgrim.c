/* AVGRIM.C - Calculate the average intensity vs. radius for an
		input image, smoothing rfile if desired.
   
   Author: Mike Wall   
   Date: 4/3/93
   Version: 1.
   
   "avgrim <input image> <output rfile> <rolling box half-width>"

   Input is TIFF TV6 image and pixel origin.  Output is a list of
        values of I(r).

   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*imagein,
	*outfile;
  
  char
    error_msg[LINESIZE];

  size_t
    num_wrote,
    num_read,
    width;

  DIFFIMAGE 
	*imdiff;

  struct rccoords origin;

/*
 * Set input line defaults:
 */
	
	imagein = stdin;
	outfile = stdout;
	width = 0;

/*
 * Read information from input line:
 */
	switch(argc) {
		case 4: width = atoi(argv[3]);
		case 3:
			if ( (outfile = fopen(argv[2],"wb")) == NULL ) {
				printf("Can't open %s.",argv[2]);
				exit(0);
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
			printf("\n Usage: avgrim <input image> "
				"<output rfile> <rolling box half-width>\n\n");
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
   * Generate rfile:
   */
  
	lavgrim(imdiff);  

  /*
   * Smooth rfile:
   */
      if (width > 0)
      lsmoothrf(imdiff, width);

/*
 * Write the output rfile:
 */

   imdiff->outfile = outfile;
   if(lwriterf(imdiff) != 0) {
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
  fclose(outfile);
  
}

