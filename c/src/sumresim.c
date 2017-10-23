/* SUMRESIM.C - Calcuate the total intensity between given resolution limits in an input image.
   
   Author: Veronica Pillar   
   Date: 6/13/14
   Version: 1.
   
   "sumresim <input image> <min resolution> <max resolution>"

   Input is TIFF TV6 image and resolution limits.  Output is a number printed to stdout. Note that the min resolution is the larger number (in Angstroms).

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

  double
    minres,
    maxres;

  DIFFIMAGE 
	*imdiff;

  struct rccoords origin;

/*
 * Set input line defaults:
 */
	
	imagein = stdin;
	outfile = stdout;
	minres = 1000;
	maxres = 0;

/*
 * Read information from input line:
 */
	switch(argc) {
		case 4: maxres = atof(argv[3]);
		case 3: minres = atof(argv[2]);
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
			printf("\n Usage: sumresim <input image> "
				"<min resolution> <max resolution>\n\n");
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
   * Calculate sum
   */
  
	lsumresim(imdiff, minres, maxres);  

/*
 * Value should be printed by the library function (bad coding I know)
 */

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

