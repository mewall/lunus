/* BUTTIM.C - Mask a diffraction image using a butterfly shape.
   
   Author: Mike Wall
   Date: 6/22/94
   Version: 1.
   
   "buttim <image in> <image out> <x origin> <y origin>
      <offset angle> <opening angle>" 

   Input is diffraction image and butterfly parameters.  
       Output is masked image.

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

  struct rccoords
	origin;

  float 
    butterfly_offset,
    opening_angle;
    

/*
 * Set input line defaults:
 */
  origin.r = DEFAULT_IMAGE_ORIGIN;
  origin.c = DEFAULT_IMAGE_ORIGIN;
  
  butterfly_offset = DEFAULT_BUTTERFLY_OFFSET;
  opening_angle = DEFAULT_OPENING_ANGLE;
  imagein = stdin;
  imageout = stdout;

/*
 * Read information from input line:
 */
 switch(argc) {
 case 7:
 opening_angle = atof(argv[6]);
 case 6:
 butterfly_offset = atof(argv[5]);
	  case 5:
	  origin.r = (RCCOORDS_DATA)atol(argv[4]);
	  case 4:
	  origin.c = (RCCOORDS_DATA)atol(argv[3]);
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
	  printf("\n Usage: polarim <image in> <image out> "
		 "<x origin> <y origin> <offset angle> "
		 "<opening angle>\n\n");
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
   * Set the origin for the image:
   */
  
  imdiff->origin = origin;

  /* 
   * Set the butterfly parameters:
   */

  imdiff->polarization = opening_angle;
  imdiff->polarization_offset = butterfly_offset;

  /*
   * Read diffraction image:
   */
  
  imdiff->infile = imagein;
  if (lreadim(imdiff) != 0) {
    perror(imdiff->error_msg);
    goto CloseShop;
  }
  
  /*
   * Mask image:
   */
  
  lbuttim(imdiff);
  
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


