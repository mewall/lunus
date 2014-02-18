/* AVGPOLIM.C - Calculate the average intensity vs. polar angle for an
		input image.
   
   Author: Mike Wall   
   Date: 2/18/2014
   Version: 1.
   
   "avgpolim <input image> <output rfile> <inner radius> <outer radius>"

   Input is image and inner and outer radius of an annulus of interest.  Output is a list of average pixel values vs. polar angle.

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

  DIFFIMAGE 
	*imdiff;

  struct rccoords origin;

  short inner_radius,outer_radius;

/*
 * Set input line defaults:
 */
	
	imagein = stdin;
	outfile = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
	case 5:
	  outer_radius = atoi(argv[4]);
	case 4:
	  inner_radius = atoi(argv[3]);
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
				"<output rfile> <x origin> <y origin>\n\n");
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
  
	//	printf("Radial averaging image of size %d,%d\n",imdiff->hpixels,imdiff->vpixels); 
	imdiff->mask_inner_radius = inner_radius;
	imdiff->mask_outer_radius = outer_radius;
	lavgrim(imdiff);  

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

