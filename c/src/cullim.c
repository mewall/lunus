/* CULLIM.C - Set all pixel values outside a specified radius range to a mask value
   
   Author: Mike Wall
   Date: 1/19/2016
   Version: 1.
   
   "cullim <image in> <image out> <inner radius> <outer radius>"

   Input is image and radius range. Output is image with pixels outside the range set to a mask value

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

  struct irange
	rfirange;

/*
 * Set input line defaults:
 */
	
	imagein = stdin;
	imageout = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
	        case 5:
		  rfirange.u = atoi(argv[4]);
		case 4:
		  rfirange.l = atoi(argv[3]);
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
			printf("\n Usage: cullim "
				"<image in> <image out> <r lower> "
				"<r upper>\n\n");
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
   * Set range
   */

  imdiff->rfirange = rfirange;

 /*
  * Read diffraction image:
  */

   imdiff->infile = imagein;
   if (lreadim(imdiff) != 0) {
     perror(imdiff->error_msg);
     goto CloseShop;
   }

/*
 * Cull image:
 */

   printf("Doing it...");
   lcullim(imdiff);
   printf("done\n");
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

