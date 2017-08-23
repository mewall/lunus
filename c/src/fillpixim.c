/* FILLPIXIM.C - Remove Bragg peak pixels by index from a list, replacing with the average value of the pixels surrounding each Bragg peak.
   
   Author: Veronica Pillar (modified from Mike Wall's rmpkim.c)
   Date: 5/6/15
   Version: 1.
   
   "fillpixim <pixel file> <image in> <image out>"

   Input is ascii pixel index list and diffraction image.  Output is 16-bit 
   image.

   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*infile,
	*imagein,
	*imageout;
  
  char
    error_msg[LINESIZE];

  size_t
    num_wrote,
    num_read;

  double
      testvar;

  long   
	i,
	ii;

  RCCOORDS_DATA
	radius;

  DIFFIMAGE 
	*imdiff;

/*
 * Set input line defaults:
 */
	
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
			if ( (infile = fopen(argv[1],"r")) == NULL ) {
				printf("Can't open %s.",argv[1]);
				exit(0);
			}
			break;
		default:
			printf("\n Usage: fillpixim <pixel file> "
				"<image in> <image out> \n\n");
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
   * Generate mask:
   */

	radius = 0; // for simple pixel punching; mask should be one pixel
	imdiff->mask_inner_radius = radius;
	imdiff->mask_outer_radius = radius;
	lgetanls(imdiff);  
	if (imdiff->mask_count == 0) {
		printf("\nNo points in mask generated.\n");
		goto CloseShop;
	}

/*
 * Read in peaks from input file and punch & fill them:
 */
	imdiff->infile = infile;
	lfillpixim(imdiff);

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
  
  fclose(infile);
  fclose(imagein);
  fclose(imageout);
  
}

