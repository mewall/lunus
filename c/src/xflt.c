/* XFLT.C - Transform a lattice according to input line instructions.
   
   Author: Mike Wall
   Date: 3/24/95
   Version: 1.
   
   Usage:
   		"xflt <input lattice> <output lattice> <operation>"

		Input are lattice and operation
		specification.  Output is transformed lattice.
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
    xfop;

  LAT3D 
	*lat;

  RFILE_DATA_TYPE *rfile;

/*
 * Set input line defaults:
 */
	
	latticein = stdin;
	latticeout = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
	  case 4:
	  xfop = atoi(argv[3]);
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
	  printf("\n Usage: xflt <input lattice> "
		 "<output lattice> <operation>\n\n"
		 "  Operations:\n"
		 "    1 = Mirror reflection through x-z plane\n\n");
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

  switch(xfop) {
    case 1:
    if (lxf1lt(lat) != 0) {
      perror(lat->error_msg);
      goto CloseShop;
    }
    break;
    default:
    printf("\n Usage: xflt <input lattice> "
	   "<output lattice> <operation>\n\n"
	   "  Operations:\n"
	   "    1 = Mirror reflection through x-z plane\n\n");
    exit(0);
    goto CloseShop;
  }
  
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


