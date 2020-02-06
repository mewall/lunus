/* FFTIM.C - Calculate the fft of an image.
   
   Author: Mike Wall
   Date: 3/15/95
   Version: 1.
   
   Usage:
   		"fftim <real input image> <imaginary input image>
		<real FFT image> <imaginary FFT image> <isign>"

		Input is real and imaginary image.  Output is FFT of
		image.  Isign=1 indicates forwards; -1 indicates inverse.

   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *imagein1,
    *imagein2,
    *imageout1,
    *imageout2;
  
  char
    error_msg[LINESIZE];

  DIFFIMAGE 
    *imdiff;

  IMAGE_DATA_TYPE
    *image1,
    *image2;

  float
    inverse_scale = 1048576.,
    *fft_data;

  size_t
    fft_index,
    image_index,
    i,
    j,
    k;

  int
    imaginput = 0,
    nn[3],
    ndim = 2,
    isign = 1;
/*
 * Set input line defaults:
 */
	
  imagein1 = stdin;
  imagein2 = stdin;
  imageout1 = stdout;
  imageout2 = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
	  case 6:
	  isign = atoi(argv[5]);
	  case 5:
	  if (strcmp(argv[4],"-") == 0) {
	    imageout2 = stdout;
	  }
	  else {
	    if ((imageout2 = fopen(argv[4],"wb")) == NULL) {
	      printf("\nCan't open %s.\n\n",argv[4]);
	      exit(0);
	    }
	  }
	  case 4:
	  if (strcmp(argv[3],"-") == 0) {
	    imageout1 = stdout;
	  }
	  else {
	    if ((imageout1 = fopen(argv[3],"wb")) == NULL) {
	      printf("\nCan't open %s.\n\n",argv[3]);
	      exit(0);
	    }
	  }
	  case 3:
	  if (strcmp(argv[2],"-") == 0) {
	    imaginput = 0;
	  }
	  else {
	    if ( (imagein2 = fopen(argv[2],"rb")) == NULL ) {
	      printf("\nCan't open %s.\n\n",argv[2]);
	      exit(0);
	    }
	    imaginput = 1;
	  }
	  case 2:
	  if (strcmp(argv[1],"-") == 0) {
	    imagein1 = stdin;
	  }
	  else {
	    if ( (imagein1 = fopen(argv[1],"rb")) == NULL ) {
	      printf("\nCan't open %s.\n\n",argv[1]);
	      exit(0);
	    }
	  }
	  break;
	  default:
	  printf("\n Usage: fftim <input real image> <input imag image>"
		 "<output real image> <output imag image> <isign>\n\n");
	  exit(0);
	}
  
  /*
   * Initialize images:
   */

  if ((imdiff = linitim(1)) == NULL) {
    perror("Couldn't initialize image.\n\n");
    exit(0);
  }
  
  /*
   * Read in real image:
   */

  imdiff->infile = imagein1;
  if (lreadim(imdiff) != 0) {
    perror("Couldn't read image.\n\n");
    exit(0);
  }
  image1 = imdiff->image;

  /*
   * Read in imaginary image if on input line;  Otherwise, set it
   * to zero:
   */

  image2 = (IMAGE_DATA_TYPE *)calloc(imdiff->image_length,
				     sizeof(IMAGE_DATA_TYPE));
  if(imaginput) {
    imdiff->image = image2;
    imdiff->infile = imagein2;
    if (lreadim(imdiff) != 0) {
      perror("Couldn't read image.\n\n");
      exit(0);
    }
  }

  /*
   * Allocate memory for fft data array:
   */

  fft_data = (float *)calloc((imdiff->image_length+1)*2,sizeof(float));
  if (!fft_data) {
    perror("Couldn't allocate fft data array.\n\n");
    exit(0);
  }

  /*
   * Prepare image for fft:
   */

  printf("Preparing lattice...\n");/***/
  fft_index = 1; /* Data array starts at position 1 */
  for(i=0;i<imdiff->vpixels;i++) {
    for(j=0;j<imdiff->hpixels;j++) {
      image_index =
	imdiff->hpixels*((j+imdiff->vpixels/2)%imdiff->vpixels) +
	  (i+imdiff->hpixels/2)%imdiff->hpixels;
      fft_data[fft_index] = (float)image1[image_index];
      fft_data[fft_index+1] = (float)image2[image_index];
      fft_index = fft_index + 2;
    }
    //printf("%d,%d,%d: %f\n",i,j,k,fft_data[fft_index-2]);/***/
  }
printf("...done.\n");/***/
  /*
   * Calculate FFT :
   */
  
  nn[1] = 1024;
  nn[2] = 1024;
printf("Entering FFT...\n");
  lfft(fft_data,nn,ndim,isign);
printf("...done.\n");
  /*
   * Extract lattice from fft data array -- take magnitude.
   */

printf("Extracting lattice...\n");/***/
  fft_index = 1; /* Data array starts at position 1 */
  for(i=0;i<imdiff->vpixels;i++) {
    for(j=0;j<imdiff->hpixels;j++) {
      image_index =
	imdiff->hpixels*((j+imdiff->vpixels/2)%imdiff->vpixels) +
	  (i+imdiff->hpixels/2)%imdiff->hpixels;
      image1[image_index] = (IMAGE_DATA_TYPE)(1./inverse_scale*
					      fft_data[fft_index]);
      image2[image_index] = (IMAGE_DATA_TYPE)(1./inverse_scale*
					      fft_data[fft_index+1]);
      fft_index = fft_index+2;
    } 
  }
printf("...done. fft_index = %ld,%ld\n",fft_index,image_index);

  /*
   * Write images to output file:
   */
printf("Writing images...\n");/***/  
  imdiff->image = image1;
  imdiff->outfile = imageout1;
  if (lwriteim(imdiff) != 0) {
    perror("Couldn't write image.\n\n");
    exit(0);
  }

printf("Writing image...\n");/***/  
  imdiff->image = image2;
  imdiff->outfile = imageout2;
  if (lwriteim(imdiff) != 0) {
    perror("Couldn't write image.\n\n");
    exit(0);
  }


CloseShop:
  
  /*
   * Free allocated memory:
   */
printf("Freeing memory...\n");
  lfreeim(imdiff);
  free((IMAGE_DATA_TYPE *)image1);
  free((float *)fft_data);

  /*
   * Close files:
   */
printf("Closing files...\n");
  fclose(imagein1);
  fclose(imagein2);
  fclose(imageout1);
  fclose(imageout2);
}


