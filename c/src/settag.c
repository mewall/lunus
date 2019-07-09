/* SETTAG.C - Add set a tag to a given value in .img header.
   
   Author: Mike Wall
   Date: 4/12/2013
   Version: 1.
   
   "settag <image in> <image out> <tag name> <tag value>" 

   Input is input image, output image, tag name, and tag value.  Output is input image with tag modified in header

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

  IMAGE_DATA_TYPE
	image_value;

  char
    *buf;

  char *tag,*tagval;
  
  tag = (char *)malloc(1000);
  tagval = (char *)malloc(1000);


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
		  strcpy(tagval,argv[4]);
	case 4:
	  strcpy(tag,argv[3]);
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
			printf("\n Usage: settag <image in> "
			       "<image out> <tag name> <tag value> \n\n");
			exit(0);
	}
 
/*
 * Initialize diffraction image:
 */
    
  if ((imdiff = linitim(1)) == NULL) {
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
 * Change the header:
 */

   lsettag(imdiff->header,tag,tagval);

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

