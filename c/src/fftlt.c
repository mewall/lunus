/* FFTLT.C - Calculate the fft of a lattice.
   
   Author: Mike Wall
   Date: 2/28/95
   Version: 1.
   
   Usage:
   		"fftlt <real input lattice> <imaginary input lattice>
		<real FFT lattice> <imaginary FFT lattice> <isign>"

		Input is real and imaginary lattice.  Output is FFT of
		lattice.  Isign=1 indicates forward; -1 indicates reverse.

   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *latticein1,
    *latticein2,
    *latticeout1,
    *latticeout2;
  
  char
    error_msg[LINESIZE];

  LAT3D 
    *lat;

  LATTICE_DATA_TYPE
    *lattice1,
    *lattice2;

  float
    inverse_scale = 262144.,
    *fft_data;

  size_t
    fft_index,
    lat_index,
    i,
    j,
    k;

  int
    imaginput = 0,
    nn[4],
    ndim = 3,
    isign = 1;
/*
 * Set input line defaults:
 */
	
  latticein1 = stdin;
  latticein2 = stdin;
  latticeout1 = stdout;
  latticeout2 = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
	  case 6:
	  isign = atoi(argv[5]);
	  case 5:
	  if (strcmp(argv[4],"-") == 0) {
	    latticeout2 = stdout;
	  }
	  else {
	    if ((latticeout2 = fopen(argv[4],"wb")) == NULL) {
	      printf("\nCan't open %s.\n\n",argv[4]);
	      exit(0);
	    }
	  }
	  case 4:
	  if (strcmp(argv[3],"-") == 0) {
	    latticeout1 = stdout;
	  }
	  else {
	    if ((latticeout1 = fopen(argv[3],"wb")) == NULL) {
	      printf("\nCan't open %s.\n\n",argv[3]);
	      exit(0);
	    }
	  }
	  case 3:
	  if (strcmp(argv[2],"-") == 0) {
	    imaginput = 0;
	  }
	  else {
	    if ( (latticein2 = fopen(argv[2],"rb")) == NULL ) {
	      printf("\nCan't open %s.\n\n",argv[2]);
	      exit(0);
	    }
	    imaginput = 1;
	  }
	  case 2:
	  if (strcmp(argv[1],"-") == 0) {
	    latticein1 = stdin;
	  }
	  else {
	    if ( (latticein1 = fopen(argv[1],"rb")) == NULL ) {
	      printf("\nCan't open %s.\n\n",argv[1]);
	      exit(0);
	    }
	  }
	  break;
	  default:
	  printf("\n Usage: fftlt <input real lattice> <input imag lattice>"
		 "<output real lattice> <output imag lattice> <isign>\n\n");
	  exit(0);
	}
  
  /*
   * Initialize lattices:
   */

  if ((lat = linitlt()) == NULL) {
    perror("Couldn't initialize lattice.\n\n");
    exit(0);
  }
  
  /*
   * Read in real lattice:
   */

  lat->infile = latticein1;
  if (lreadlt(lat) != 0) {
    perror("Couldn't read lattice.\n\n");
    exit(0);
  }
  lattice1 = lat->lattice;

  /*
   * Read in imageinary lattice if on input line;  Otherwise, set it
   * to zero:
   */

  if(imaginput) {
    lat->lattice = NULL;
    lat->infile = latticein2;
    if (lreadlt(lat) != 0) {
      perror("Couldn't read lattice.\n\n");
      exit(0);
    }
    lattice2 = lat->lattice;
  } else {
    lattice2 = (LATTICE_DATA_TYPE *)calloc(lat->lattice_length,
					   sizeof(LATTICE_DATA_TYPE));
  }

  /*
   * Allocate memory for fft data array:
   */

  fft_data = (float *)calloc(64*64*64*2+1,sizeof(float));
  if (!fft_data) {
    perror("Couldn't allocate fft data array.\n\n");
    exit(0);
  }

  /*
   * Prepare lattice for fft:
   */

printf("Preparing lattice...\n");/***/
  fft_index = 1; /* Data array starts at position 1 */
  for(i=0;i<64;i++) {
    for(j=0;j<64;j++) {
      for(k=0;k<64;k++) {
	lat_index = 4096*((k+31)%64) + 64*((j+31)%64) + (i+31)%64;
	fft_data[fft_index] = (float)lattice1[lat_index];
	fft_data[fft_index+1] = (float)lattice2[lat_index];
/*
 * Convert ignore_tags to zeroes:
 */
	if (fft_data[fft_index] == (float)lat->mask_tag)
	  fft_data[fft_index] = 0;
	if (fft_data[fft_index+1] == (float)lat->mask_tag)
	  fft_data[fft_index+1] = 0;
	fft_index = fft_index + 2;
	/*printf("%d,%d,%d: %f\n",i,j,k,fft_data[fft_index-2]);/***/
      }
    } 
  }
printf("...done.\n");/***/
  /*
   * Calculate FFT :
   */
  
  nn[1] = 64;
  nn[2] = 64;
  nn[3] = 64;
printf("Entering FFT...\n");
  lfft(fft_data,nn,ndim,isign);
printf("...done.\n");
  /*
   * Extract lattice from fft data array -- take magnitude.
   */

printf("Extracting lattice...\n");/***/
  fft_index = 1; /* Data array starts at position 1 */
  lat_index = 0;
  for(i=0;i<64;i++) {
    for(j=0;j<64;j++) {
      for(k=0;k<64;k++) {
	lat_index = 4096*((k+31)%64) + 64*((j+31)%64) + (i+31)%64;
	if (isign == -1) {
	  lattice1[lat_index] = 1./inverse_scale * 
	    (LATTICE_DATA_TYPE)fft_data[fft_index];
	  lattice2[lat_index] = 1./inverse_scale *
	    (LATTICE_DATA_TYPE)fft_data[fft_index+1];
	} else {
	  lattice1[lat_index] = (LATTICE_DATA_TYPE)fft_data[fft_index];
	  lattice2[lat_index] = (LATTICE_DATA_TYPE)fft_data[fft_index+1];
	}
	fft_index = fft_index + 2;
      }
    } 
  }
printf("...done. fft_index = %d,%d\n",fft_index,lat_index);

  /*
   * Prepare lattice for output:
   */

  lat->xscale = 1./lat->xscale*1/64.;
  lat->yscale = 1./lat->yscale*1/64.;
  lat->zscale = 1./lat->zscale*1/64.;
  lat->xbound.min = - (LATTICE_DATA_TYPE)(lat->origin.i*lat->xscale);
  lat->xbound.max = (LATTICE_DATA_TYPE)((lat->xvoxels -
					lat->origin.i - 1)*lat->xscale); 
  lat->ybound.min = - (LATTICE_DATA_TYPE)(lat->origin.j*lat->yscale); 
  lat->ybound.max = (LATTICE_DATA_TYPE)((lat->yvoxels -
					lat->origin.j - 1)*lat->yscale); 
  lat->zbound.min = - (LATTICE_DATA_TYPE)(lat->origin.i*lat->zscale); 
  lat->zbound.max = (LATTICE_DATA_TYPE)((lat->zvoxels -
					lat->origin.k - 1)*lat->zscale); 

  /*
   * Write lattices to output file:
   */
printf("Writing lattice...\n");/***/  
  lat->lattice = lattice1;
  lat->outfile = latticeout1;
  if (lwritelt(lat) != 0) {
    perror("Couldn't write lattice.\n\n");
    exit(0);
  }

printf("Writing lattice...\n");/***/  
  lat->lattice = lattice2;
  lat->outfile = latticeout2;
  if (lwritelt(lat) != 0) {
    perror("Couldn't write lattice.\n\n");
    exit(0);
  }


CloseShop:
  
  /*
   * Free allocated memory:
   */
printf("Freeing memory...\n");
  lfreelt(lat);
  free((LATTICE_DATA_TYPE *)lattice1);
  free((float *)fft_data);

  /*
   * Close files:
   */
printf("Closing files...\n");
  fclose(latticein1);
  fclose(latticein2);
  fclose(latticeout1);
  fclose(latticeout2);
}


