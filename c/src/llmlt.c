/* LLMLT.C - Generate a liquid-like motions model.
   
   Author: Mike Wall
   Date: 9/26/2016
   Version: 1.
   
   Usage:
   		"llmlt <input lattice> <output lattice> <sigma> <gamma>"

		Input is I0(hkl) lattice and sigma, gamma from llm model.  Output is liquid-like motions model of diffuse scattering.
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *latticein,
    *latticeout;
  
  char
    error_msg[LINESIZE];

  LAT3D 
    *lat;

  float
    sigma,
    gamma;

/*
 * Set input line defaults:
 */
	
	latticein = stdin;
	latticeout = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
	  case 5:
	  gamma = atof(argv[4]);
	  case 4:
	  sigma = atof(argv[3]);
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
  
  lat->sigma = sigma;
  lat->gamma = gamma;
  //For now, just calculate for ICH
  lat->cell_str = "57.154,57.965,69.092,90.00,112.77,90.00";
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


