/* SYMLT.C - Symmetrize a lattice according to input line instructions.
   
   Author: Mike Wall
   Date: 2/28/95
   Modified by Alex Wolff to include all Laue classes. Added 6/16/2017 MEW
   Version: 1.
   
   Usage:
   		"symlt <input lattice> <output lattice> <symmetry_operation>"

		Input are lattice and symmetry operation
		specification.  Output is symmetrized lattice.
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
    symop;

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
	  symop = atol(argv[3]);
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
	  printf("\n Usage: symlt <input lattice> "
		 "<output lattice> <symmetry_operation>\n\n"
		 "  Symmetry Operations:\n"
		 "    0   =  Laue class     -1    (space group        1)\n"
		 "    -1   =  Laue class    2/m    (space groups     3-5)\n"
		 "    -2   =  Laue class    mmm    (space groups   16-24)\n"
		 "    -3   =  Laue class    4/m    (space groups   75-80)\n"
     "    -4   =  Laue class  4/mmm    (space groups   89-98)\n"
     "    -5   =  Laue class     -3    (space groups 143-146)\n"
     "    -6   =  Laue class    -3m    (space groups 149-155)\n"
     "    -7   =  Laue class    6/m    (space groups 168-173)\n"
     "    -8   =  Laue class  6/mmm    (space groups 177-182)\n"
     "    -9   =  Laue class    m-3    (space groups 195-199)\n"
		 "    -10  =  Laue class   m-3m    (space groups 207-214)\n");
	      
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

  if (symop <= 0) {    
    lat->symop_index = -symop;
  } else {
    switch (symop) {
    case 1:
      lat->symop_index = 3;
      break;
    case 2:
      lat->symop_index = 2;
      break;
    case 3:
      lat->symop_index = 9;
      break;
    case 4:
      lat->symop_index = 1;
      break;
    default:
      printf("Use positive value for legacy symmetry operations, negative value 0-10 for ordered Laue groups.\n");
      exit(1);
    }
  }

  if (argc==7) {
    lat->origin.i=origin.i; lat->origin.j=origin.j; lat->origin.k=origin.k;
  }
  lsymlt(lat);
  
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


