/* LLMDCLT.C - Use deconvolution to back out Bragg-like intensities from a diffuse lattice, assuming liquid-like motions
   
   Author: Mike Wall
   Date: 2/23/2017
   Version: 1.
   
   Usage:
   		"dcllmlt <input lattice> <output lattice> <cell_str> <gamma> <sigma1> <sigma2>"

		Input is diffuse lattice, unit cell, gamma, sigma1, sigma2 from llm model.  Output is Bragg-like intensities after dconvoluting liquid-like motions model of diffuse scattering.
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *latticein,
    *latticeout;
  
  char
    cell_str[256],
    error_msg[LINESIZE];

  LAT3D 
    *lat;

  float
    sigma1,
    sigma2,
    gamma;

/*
 * Set input line defaults:
 */
	
	latticein = stdin;
	latticeout = stdout;
	sigma2 = 0.0;
/*
 * Read information from input line:
 */
	switch(argc) {
	  case 7:
	  sigma2 = atof(argv[6]);
	  case 6:
	  sigma1 = atof(argv[5]);
	  case 5:
	  gamma = atof(argv[4]);
	case 4:
	  strcpy(cell_str,argv[3]);
	  case 3:
	  if (strcmp(argv[2],"-") == 0) {
	    latticeout = stdout;
	  }
	  else {
	    if ((latticeout = fopen(argv[2],"wb")) == NULL) {
	      printf("\nCan't open %s.\n\n",argv[2]);
	      exit(0);
	    }
	  }
	  case 2:
	  if (strcmp(argv[1],"-") == 0) {
	    latticein = stdin;
	  }
	  else {
	    if ( (latticein = fopen(argv[1],"rb")) == NULL ) {
	      printf("\nCan't open %s.\n\n",argv[1]);
	      exit(0);
	    }
	  }
	  break;
	  default:
	  printf("\n Usage: liquidcorrlt <input lattice> "
		 "<output lattice> <sigma> <gamma>\n\n");
	  exit(0);
	}

	if (sigma2 == 0.0) {
	  sigma2 = sigma1;
	}
  
  /*
   * Initialize lattices:
   */


  if ((lat = linitlt()) == NULL) {
    perror("Couldn't initialize lattice.\n\n");
    exit(0);
  }
  
  /*
   * Read in lattice:
   */

  lat->infile = latticein;
  if (lreadlt(lat) != 0) {
    perror("Couldn't read lattice.\n\n");
    exit(0);
  }

  /*
   * Calculate liquid-like motions prefactor:
   */
  
  lat->sigma = sigma1;
  lat->anisoU.xx = sigma1*sigma1;
  lat->anisoU.xy = 0.0;
  lat->anisoU.xz = 0.0;
  lat->anisoU.yx = 0.0;
  lat->anisoU.yy = sigma2*sigma2;
  lat->anisoU.yz = 0.0;
  lat->anisoU.zx = 0.0;
  lat->anisoU.zy = 0.0;
  lat->anisoU.zz = sigma1*sigma1;
  lat->gamma = gamma;
  strcpy(lat->cell_str,cell_str);
  lparsecelllt(lat);
  lllmlt(lat);
  
  /*
   * Write lattice to output file:
   */
  
  lat->outfile = latticeout;
  if (lwritelt(lat) != 0) {
    perror("Couldn't write lattice.\n\n");
    exit(0);
  }

CloseShop:
  
  /*
   * Free allocated memory:
   */

  lfreelt(lat);

  /*
   * Close files:
   */
  
  fclose(latticein);
  fclose(latticeout);
}


