/* EXPLT.C - Create an exponential decay lattice based on a template.
   
   Author: Mike Wall
   Date: 7/25/2014
   Version: 1.
   
   Usage:
   		"explt <input lattice template> <output lattice> <decay constant>"

		Input is an input lattice template and the decay constant of an exponential.
		Output is exponential decay lattice.  
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
    decay_constant;

/*
 * Set input line defaults:
 */
	decay_constant = 1.;
	latticein = stdin;
	latticeout = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
	  case 4:
	  decay_constant = atof(argv[3]);
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
	  printf("\n Usage: explt <input lattice template> <decay constant> <output lattice> "
		 "\n\n");
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
 * Generate the scaled lattice:
 */

  lat->rfile[0] = decay_constant;
  lexplt(lat);

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


