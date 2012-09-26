/* DF2IM.C - Generate an image based upon a diffuse features file.
                
   Author: Mike Wall  
   Date: 5/30/94
   Version: 1.0
   
   "df2im <diffuse features in> <template image in> <image out>"

   Input is ascii diffuse features file and input template image.  Output is
   diffuse features file.

*/

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *featurein,
    *imageout,
    *imagein;
  
  char
    error_msg[LINESIZE];
  
  DIFFIMAGE 
    *imdiff;
  
  /*
   * Set input line defaults:
   */
  
  featurein = stdin;
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
    if ( (featurein = fopen(argv[1],"r")) == NULL ) {
      printf("Can't open %s.",argv[1]);
      exit(0);
    }
    break;
  default:
    printf("\n Usage: df2im <diffuse features in> "
	   "<template image in> <image out> \n\n");
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
   * Read diffuse features file:
   */
  
  imdiff->infile = featurein;
  if (lreaddf(imdiff) != 0) {
    perror(imdiff->error_msg);
    goto CloseShop;
  }
  
  /*
   * Generate an image from diffuse features:
   */

  if (ldf2im(imdiff) != 0) {
    perror(imdiff->error_msg);
    goto CloseShop;
  }
  
  /*
   * Write the output diffuse features file:
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
  fclose(featurein);
  fclose(imageout);
}












