/* OVLDIM.C - Punch out overloads in an image.
   
   Author: Mike Wall   Modified by:Fredrik Osterberg
   Date: 3/14/95
   Version: 1.
   
   "ovldim <image in> <image out>"
   
   Input is image with overloads.  Output is image with overloads
   expanded.

*/

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*imagein,
	*imageout;
  
  char
    error_msg[LINESIZE];

  DIFFIMAGE 
	*imdiff;

  size_t
    i;

/*
 * Set input line defaults:
 */
	
	imagein = stdin;
	imageout = stdout;

/*
 * Read information from input line:
 */
  switch(argc) {
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
    printf("\n Usage: ovldim "
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
  
  imdiff->mask_inner_radius = 0;
  imdiff->mask_outer_radius = DEFAULT_OVERLOAD_RADIUS;

  lgetanls(imdiff);  
  if (imdiff->mask_count == 0) {
    printf("\nNo points in mask generated.\n");
    goto CloseShop;
  }
  
  /*
   * Generate overload list:
   */
  
  lgetovld(imdiff);
  
  printf("Overload count: %d\n",imdiff->overload_count);

  /*
   *  Step through the peak table and punch out holes in the image.
   */
  for(i=0;i<imdiff->overload_count;i++) {
    imdiff->pos = imdiff->overload[i];
    lpunch(imdiff);
  }
  
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

