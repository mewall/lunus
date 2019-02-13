/* NORMIM.C - Correct for solid-angle normalization and 
              detector-face rotation in a diffraction image.
   
   Author: Mike Wall
   Date: 2/24/95, 11/14/2012
   Version: 1.
   
   "normim <image in> <image out> <cassette rotx> <cassette roty>" 

   Input is diffraction image.  Output is normalized corrected image.

   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*imagein,
	*imageout;
  
  size_t
    i;

  IMAGE_DATA_TYPE
    value_offset;

  DIFFIMAGE 
	*imdiff;

  struct rccoords
	origin;

  float 
    distance_mm;

  struct xyzcoords
    cassette,
    beam;

/*
 * Set input line defaults:
 */

  value_offset = DEFAULT_VALUE_OFFSET;
  cassette.x = DEFAULT_CASSETTE_ROTX;
  cassette.y = DEFAULT_CASSETTE_ROTY;
  beam.x = 0;
  beam.y = 0;
  imagein = stdin;
  imageout = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
	case 7:
	  beam.y = (XYZCOORDS_DATA)atof(argv[6]);
	case 6:
	  beam.x = (XYZCOORDS_DATA)atof(argv[5]);
	  case 5:
	  cassette.y = (XYZCOORDS_DATA)atof(argv[4]);
	  case 4:
	  cassette.x = (XYZCOORDS_DATA)atof(argv[3]);
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
	  printf("\n Usage: normim <image in> <image out> "
		 "(<cassette rotx> <cassette roty>)\n\n");
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
   * Normalize the diffraction image:
   */
  
  //imdiff->value_offset = value_offset;
  imdiff->cassette.x = cassette.x;
  imdiff->cassette.y = cassette.y;
  imdiff->cassette.z = 0.0;
  if (beam.x != 0) {imdiff->beam_mm.y = beam.x;};
  if (beam.y != 0) {imdiff->beam_mm.x = beam.y;};
  lnormim(imdiff);
  
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


