/* CFIM.C - Create a correction factor field for a diffraction image.
   
   Author: Mike Wall
   Date: 6/5/2017
   Version: 1.
   
   "cfim <image in> <correction out> 
      <distance[mm]> <polarization> <offset angle> <scale>" 

   Input is diffraction image.  Output is floating point correction image.

   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*imagein,
	*imageout;
  
  size_t
    i,num_wrote;

  DIFFIMAGE 
	*imdiff;

  struct rccoords
	origin;

  float 
    polarization_offset,
    polarization,
    distance_mm,
    scale;

/*
 * Set input line defaults:
 */

  scale = 1.;
  origin.r = DEFAULT_IMAGE_ORIGIN;
  origin.c = DEFAULT_IMAGE_ORIGIN;
  distance_mm = DEFAULT_DISTANCE_MM;
  polarization = DEFAULT_POLARIZATION;
  imagein = stdin;
  imageout = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
	  case 7:
	  scale = atof(argv[6]);
	  case 6:
	  polarization_offset = atof(argv[5]);
	  case 5:
	  polarization = atof(argv[4]);
	  case 4:
	  distance_mm = atof(argv[3]);
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
		 "<x origin> <y origin> <distance [mm]> <polarization> "
		 "<offset angle>\n\n");
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
  
  //  imdiff->origin = origin;

  /*
   * Set sample-to-detector distance for the image
   */

  imdiff->distance_mm = distance_mm;
  
  /* 
   * Set the polarization of the beam:
   */

  imdiff->polarization = polarization;
  imdiff->polarization_offset = polarization_offset;

  /*
   * Read diffraction image:
   */
  
  imdiff->infile = imagein;
  if (lreadim(imdiff) != 0) {
    perror(imdiff->error_msg);
    goto CloseShop;
  }
  
  // Allocate memory for the correction image

  imdiff->correction = (float *)malloc(imdiff->image_length*sizeof(float));

  /*
   * Calculate the correction image:
   */
  
  imdiff->correction[0]=scale;
  lcfim(imdiff);
  
  /*
   * Write the correction image:
   */
  
  num_wrote = fwrite(imdiff->correction,sizeof(float),imdiff->image_length,imageout);
  if(num_wrote != imdiff->image_length) {
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


