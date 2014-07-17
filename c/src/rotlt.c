/* ROTLT.C - Symmetrize a lattice according to input line instructions.
   
   Author: Mike Wall
   Date: 7/17/2014
   Version: 1.
   
   Usage:
   		"rotlt <input lattice> <output lattice> <axis code>"

		Input are lattice and axis code specification (1 = x; 2 = y; 3 = z)
		specification.  Output is lattice rotated by 90 degrees about the specified axis.
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
    origin;

/*
 * Set input line defaults:
 */
	
	latticein = stdin;
	latticeout = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
    case 7: 
    origin.k = atol(argv[6]);
    case 6:
    origin.j = atol(argv[5]);
    case 5:
    origin.i = atol(argv[4]);
	  case 4:
	  axis = atol(argv[3]);
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
	  printf("\n Usage: rotlt <input lattice> "
		 "<output lattice> <axis code>\n\n"
		 "  Axis Codes:\n"
		 "    1 = x\n"
		 "    2 = y\n"
		 "    3 = z\n\n");
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

  if (argc==7) {
    lat->origin.i=origin.i; lat->origin.j=origin.j; lat->origin.k=origin.k;
  }
  lrotlt(lat,axis);
  
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


