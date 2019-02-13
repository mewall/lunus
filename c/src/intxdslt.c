/* INTXDSLT.C - Integrate diffuse features from an image using XDS information.
   
   Author: Mike Wall
   Date: 9/15/2015
   Version: 1.
   
   "thrshim <image in> <lower> <upper> <image out>" 

   Input is image and upper and lower thresholds.  Output is input image 
   with all pixel values outside of thresholds converted to an ingore_tag.

   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*imagein,
	*imageout;
  
  size_t
    i;

  DIFFIMAGE 
	*imdiff;

  LAT3D
    *lat;

  IMAGE_DATA_TYPE
	upper_threshold,
	lower_threshold;

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
			if (strcmp(argv[4], "-") == 0) {
				imageout = stdout;
			}
			else {
			 if ( (imageout = fopen(argv[4],"wb")) == NULL ) {
				printf("Can't open %s.",argv[4]);
				exit(0);
			 }
			}
                case 4:
			upper_threshold = (IMAGE_DATA_TYPE)atoi(argv[3]);
                case 3:
			lower_threshold = (IMAGE_DATA_TYPE)atoi(argv[2]);
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
			printf("\n Usage: thrshim <image in> <lower> <upper> "
			       "<image out>\n\n");
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
 * Initialize lattice:
 */
    
  if ((lat = linitlt()) == NULL) {
    perror("\nCouldn't initialize lattice.\n\n");
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
 * Integrate
 */

   lintxdslt(imdiff,lat);

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

