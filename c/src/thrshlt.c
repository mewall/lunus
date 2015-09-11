/* THRSHLT.C - Convert all voxels with values outside a specified range in a lattice to an 
             ignore_tag.
   
   Author: Mike Wall
   Date: 8/11/2014
   Version: 1.
   
   Usage:
   		"thrshlt <input lattice> <output lattice> <lower bound> <upper bound>"

		Input is an upper and lower bound and an input 
			lattice.  Output is a lattice.  
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *latticein,
    *latticeout;
  
  char
    error_msg[LINESIZE];
  
  size_t
    i,
    j,
    k,
    num_read,
    num_wrote;
  
  LAT3D 
    *lat;
  
  RFILE_DATA_TYPE 
    *rfile;
  
  float
    lower_bound,upper_bound;

/*
 * Set input line defaults:
 */
	lower_bound = 0.;
	upper_bound = (float)MAX_IMAGE_DATA_VALUE;
	latticein = stdin;
	latticeout = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
	  case 5:
	  lower_bound = atof(argv[4]);
	  case 4:
	  lower_bound = atof(argv[3]);
	  case 3:
	  if (strcmp(argv[2],"-") == 0) {
	    latticeout = stdout;
	  }
	  else {
	    if ((latticeout = fopen(argv[2],"wb")) == NULL) {
	      printf("\nCan't open %s.\n\n",argv[1]);
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
	  printf("\n Usage: thrshlt <input lattice> <output lattice> "
		 "<lower bound> <upper bound>\n\n");
	  exit(0);
	}
  
  /*
   * Initialize lattice:
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
 * Generate the thresholded lattice:
 */

  lat->valuebound.min = lower_bound;
  lat->valuebound.max = upper_bound;
  lthrshlt(lat);

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
