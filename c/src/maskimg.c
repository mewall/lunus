/* MASKIMG.C - Create a pixel mask based on input file coordinates.
   
   Author: Mike Wall   Modified by:Fredrik Osterberg, Veronica Pillar
   Date: 1/11/93
   Version: 1.
   
   "maskimg <input file> <image in> <image out> <inner radius> <outer radius>"

   Input is ascii coordinates file.  Output is 16-bit 
   image of specified size (1024 x 1024 default). You may specify the inner and outer radii (inclusive) of the annular mask on the command line.

   Date: 3/12/14
   Version: 3.
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

  long   
	i,
	ii;

  DIFFIMAGE 
	*imdiff;

/*
 * Set input line defaults:
 */
	
	imagein = stdin;
	imageout = stdout;
	i = 0;
	ii = 0;

/*
 * Read information from input line:
 */
	switch(argc) {
		case 6:
			i = atol(argv[4]);
			ii = atol(argv[5]);

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
			printf("\n Usage: maskimg <input file> "
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
	imdiff->mask_inner_radius = i;
	imdiff->mask_outer_radius = ii;
	lgetanls(imdiff);  
	if (imdiff->mask_count == 0) {
		printf("\nNo points in mask generated.\n");
		goto CloseShop;
	}
/*
 * Read in peaks from input file:
 */
	imdiff->infile = infile;
	lgetpks(imdiff);
/*
 * Generate overload list:
 */
	//lgetovld(imdiff);
  /*
   *  Step through the peak table and punch out holes in the image.
   */
//printf("\nNumber of peaks: %ld\n\n",(long)imdiff->peak_count); 
	for(i=0;i<imdiff->peak_count;i++) {
		imdiff->pos.c = (short)imdiff->peak[i].y;
		imdiff->pos.r = (short)imdiff->peak[i].x;
		lpunch(imdiff);
	}
/*	for(i=0;i<imdiff->overload_count;i++) {
		imdiff->pos = imdiff->overload[i];
		lpunch(imdiff);
	}
*/
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

