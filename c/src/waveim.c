/* WAVEIM - Create an image with a fixed length scale fluctuation.
   
   Author: Mike Wall
   Date: 4/6/95
   Version: 1.
   
   "waveim <image in> <constant> <image out>" 

   Input is image, amplitude and pitch.  
   Output is wavy image of type input image.

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
  
  float
    amplitude,
    pitch;
  
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
    pitch = atof(argv[4]);
    case 4:
    amplitude = atof(argv[3]);
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
    printf("\n Usage: waveim <image in> <image out> <amplitude> "
	   "<pitch>\n\n");
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
   * Set input line specs:
   */

  imdiff->amplitude = amplitude;
  imdiff->pitch = pitch;

  /*
   * Create wavy image:
   */
  
  lwaveim(imdiff);
  
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

