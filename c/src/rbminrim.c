/* RBMINRIM.C - Calculate the minimum intensity as a function of radius for an
		input image, with rolling-box averaging to smooth the result.
   
   Author: Veronica Pillar
   Date: 12/31/13
   Version: 1.
   
   "rbminrim <input image> <output rfile> <image box half-width> <rfile box half-width>"

   Input is TIFF TV6 image and box half-widths.  Output is a list of values of I(r).

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
    num_read;

  int
    width,
    width2;

  DIFFIMAGE 
	*imdiff;

  struct rccoords origin;

/*
 * Set input line defaults:
 */
	
	imagein = stdin;
	outfile = stdout;
	width = 2;
	width2 = 2;

/*
 * Read information from input line:
 */

	switch(argc) {
		case 5:
			if ((width2 = atoi(argv[4])) < 0) {
			  printf("Box half-width must be nonnegative integer");
			  exit(0);
			}
		case 4:
			if ((width = atoi(argv[3])) < 0) {
			  printf("Box half-width must be nonnegative integer");
			  exit(0);
			}
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
			printf("\n Usage: rbminrim <input image> "
				"<output rfile> <image box half-width> <r-file box half-width>\n\n");
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
   * Generate rfile:
   */
  
	lrbminrim(imdiff, width);  

  /*
   * Smooth rfile:
   */
      if (width2 > 0)
	lsmoothrf(imdiff, width2);

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

