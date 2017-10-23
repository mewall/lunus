/* POLARIM.C - Perform polarization correction on a diffraction image.
   
   Author: Mike Wall
   Date: 4/26/94
   Version: 1.
   
   "polarim <image in> <image out> 
      <polarization> <offset angle>" 

   Input is diffraction image.  Output is polarization corrected image.

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


  float 
    polarization_offset,
    polarization;

/*
 * Set input line defaults:
 */
  polarization = DEFAULT_POLARIZATION;
  imagein = stdin;
  imageout = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
	  case 5:
	  polarization_offset = atof(argv[5]);
	  case 4:
	  polarization = atof(argv[4]);
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
		 "<polarization> <offset angle>\n\n");
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

  //  printf("imdiff->value_offset=%f\n",imdiff->value_offset);
  
  /*
   * Polarization correct image:
   */
  
  lpolarim(imdiff);
  
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


