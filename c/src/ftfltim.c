/* FTFLTIM.C - Fourier-transform filter an image.
   
   Author: Mike Wall
   Date: 4/7/95
   Version: 1.
   
   Usage:
   		"ftfltim <input image> <output image> <length scale>"

		Input is image.  Output is FT filtered image.
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *imagein1,
    *imageout1;
  
  char
    error_msg[LINESIZE];

  DIFFIMAGE 
    *imdiff;

  IMAGE_DATA_TYPE
    *image1;

  float
    inverse_scale = 1048576.,
    factor,
    atten,
    *fimage1,
    *fimage2,
    *fft_data;

  size_t
    fft_index,
    image_index,
    i,
    j,
    k;

  int
    range,
    length_scale,
    imaginput = 0,
    nn[3],
    ndim = 2,
    isign = 1;
/*
 * Set input line defaults:
 */
	
  imagein1 = stdin;
  imageout1 = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
	  case 4:
	  length_scale = atoi(argv[3]);
	  case 3:
	  if (strcmp(argv[2],"-") == 0) {
	    imageout1 = stdout;
	  }
	  else {
	    if ((imageout1 = fopen(argv[2],"wb")) == NULL) {
	      printf("\nCan't open %s.\n\n",argv[2]);
	      exit(0);
	    }
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
	  printf("\n Usage: ftfltim <input image> "
		 "<output image> <length_scale>\n\n");
	  exit(0);
	}
  
  /*
   * Initialize images:
   */

  if ((imdiff = linitim()) == NULL) {
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
   * Allocate memory for fft data array and intermediate floating
   *  point images:
   */

  fft_data = (float
	      *)calloc((imdiff->image_length+1)*2,sizeof(float));
  fimage1 = (float *)calloc(imdiff->image_length+1,sizeof(float));
  fimage2 = (float *)calloc(imdiff->image_length+1,sizeof(float));
  if (!fft_data || !fimage1 || !fimage2) {
    perror("Couldn't allocate fft data arrays.\n\n");
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
      fft_data[fft_index+1] = 0;
      fft_index = fft_index + 2;
    }
    /*printf("%d,%d,%d: %f\n",i,j,k,fft_data[fft_index-2]);/***/
  }
  printf("...done.\n");/***/
  /*
   * Calculate FFT :
   */
  
  nn[1] = 1024;
  nn[2] = 1024;
  isign=1;
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
      fimage1[image_index] = fft_data[fft_index];
      fimage2[image_index] = fft_data[fft_index+1];
      fft_index = fft_index+2;
    } 
  }
printf("...done. fft_index = %d,%d\n",fft_index,image_index);

  /*
   * Window the FFT:
   */

printf("Window...\n");
  factor = 1./2./3.14159/range/range;
  image_index = 0;
  range = imdiff->vpixels / length_scale;
  for(i=0;i<imdiff->vpixels;i++){
    for(j=0;j<imdiff->hpixels;j++) {
      atten = factor*expf(-(((float)i-512)*((float)i-512) +
			   ((float)j-512)*((float)j-512))/2./range/range);
      fimage1[image_index] = atten*fimage1[image_index];
      fimage2[image_index] = atten*fimage2[image_index];
    }
    image_index++;
  }
  printf("...done\n");
  
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
      fft_data[fft_index] = fimage1[image_index];
      fft_data[fft_index+1] = fimage2[image_index];
      fft_index = fft_index + 2;
    }
    /*printf("%d,%d,%d: %f\n",i,j,k,fft_data[fft_index-2]);/***/
  }
  printf("...done.\n");/***/
  
  /*
   * Calculate FFT :
   */
  
  nn[1] = 1024;
  nn[2] = 1024;
  isign=-1;
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
      fft_index = fft_index+2;
    } 
  }
  printf("...done. fft_index = %d,%d\n",fft_index,image_index);
  
  /*
   * Write image to output file:
   */
  printf("Writing images...\n");/***/  
  imdiff->image = image1;
  imdiff->outfile = imageout1;
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
  free((float *)fft_data);
  free((float *)fimage1);
  free((float *)fimage2);
  
  /*
   * Close files:
   */
  printf("Closing files...\n");
  fclose(imagein1);
  fclose(imageout1);
}



