/* TRANSLT.C - Translate the intensity in a lattice by specified coordinate shift.
   
   Author: Mike Wall
   Date: 7/17/2014
   Version: 1.
   
   Usage:
   		"translt <input lattice> <output lattice> <x shift> <y shift> <z shift>"

		Input are lattice and shift coords.  Output is lattice with density translated by the shift.
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*latticein,
	*latticeout;

  char
    error_msg[LINESIZE];

  int
    axis;

  LAT3D 
	*lat;

  RFILE_DATA_TYPE *rfile;

  struct ijkcoords
    t;

/*
 * Set input line defaults:
 */
	
	latticein = stdin;
	latticeout = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
    case 6: 
    t.k = atol(argv[5]);
    case 5:
    t.j = atol(argv[4]);
    case 4:
    t.i = atol(argv[3]);
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
	  printf("\n Usage: translt <input lattice> "
		 "<output lattice> <shift>\n\n");
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
   * Perform symmetry operation:
   */

  ltranslt(lat,t);
  
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


