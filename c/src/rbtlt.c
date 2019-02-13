/* RBTLT.C - Generate a rigid-body translation model.
   
   Author: Mike Wall
   Date: 9/26/2016
   Version: 1.
   
   Usage:
   		"llmlt <input lattice> <output lattice> <cell_str> <sigma>"

		Input is I0(hkl) lattice, unit cell, sigma from rigid body translation model.  Output is rigid body translation model of diffuse scattering.
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
    sigma;

/*
 * Set input line defaults:
 */
	
	latticein = stdin;
	latticeout = stdout;
	sigma = 0.0;
/*
 * Read information from input line:
 */
	switch(argc) {
	  case 5:
	  sigma = atof(argv[4]);
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
  lat->anisoU.xx = sigma*sigma;
  lat->anisoU.xy = 0.0;
  lat->anisoU.xz = 0.0;
  lat->anisoU.yx = 0.0;
  lat->anisoU.yy = sigma*sigma;
  lat->anisoU.yz = 0.0;
  lat->anisoU.zx = 0.0;
  lat->anisoU.zy = 0.0;
  lat->anisoU.zz = sigma*sigma;
  strcpy(lat->cell_str,cell_str);
  lparsecelllt(lat);
  lrbtlt(lat);
  
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


