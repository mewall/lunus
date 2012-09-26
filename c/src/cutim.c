/* CUTIM.C - "Cut" a rectangle out of an image by marking pixels 
     with ignore tags.
   
   Author: Mike Wall  
   Date: 1/13/95
   Version: 1.
   
   "cutim <image in> <x_lower_bound> <x_upper_bound> <y_lower_bound> 
		<y_upper_bound> <image out>

   Input is diffraction image.  Output is 16-bit 
     image of specified size (1024 x 1024 default), with pixels inside bounds
     marked to be ignored.
   
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*imagein,
	*imageout;
  
  char
    error_msg[LINESIZE];

  long   
	i,
	ii;

  DIFFIMAGE 
	*imdiff;

  struct rccoords
	upper_bound,
	lower_bound;

/*
 * Set input line defaults:
 */
	upper_bound.r = DEFAULT_WINDOW_UPPER;
	upper_bound.c = DEFAULT_WINDOW_UPPER;
	lower_bound.c = DEFAULT_WINDOW_LOWER;
	lower_bound.c = DEFAULT_WINDOW_LOWER;
	imagein = stdin;
	imageout = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
		case 7:
			if (strcmp(argv[6], "-") == 0) {
				imageout = stdout;
			}
			else {
			 if ( (imageout = fopen(argv[6],"wb")) == NULL ) {
				printf("Can't open %s.",argv[6]);
				exit(0);
			 }
			}
		case 6:
			upper_bound.r = (RCCOORDS_DATA)atol(argv[5]);
		case 5:
			lower_bound.r = (RCCOORDS_DATA)atol(argv[4]);
		case 4:
			upper_bound.c = (RCCOORDS_DATA)atol(argv[3]);
		case 3:
			lower_bound.c = (RCCOORDS_DATA)atol(argv[2]);
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
			printf("\n Usage: cutim <image in> <x_lower_bound> "
			       "<x_upper_bound> <y_lower_bound> "
			       "<y_upper_bound> <image out> \n\n");
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
 * Set window parameters:
 */

  imdiff->window_upper = upper_bound;
  imdiff->window_lower = lower_bound;

/*
 * Mark pixels inside window as ignored:
 */

  lcutim(imdiff);

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


