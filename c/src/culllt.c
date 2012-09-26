/* CULLLT.C - Throw away part of a lattice.
   
   Author: Mike Wall
   Date: 4/26/95
   Version: 1.
   
   Usage:
   		"culllt <input lattice> <output lattice> <inner
		radius> <outer radius>"

		Input is a scalar and a radius range. 
		Output is a culled lattice, with voxels in the
		specified range preserved.  
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
    inner_radius,
    outer_radius;

/*
 * Set input line defaults:
 */
  latticein = stdin;
  latticeout = stdout;
  inner_radius = DEFAULT_INNER_RADIUS;
  outer_radius = DEFAULT_OUTER_RADIUS;

/*
 * Read information from input line:
 */
	switch(argc) {
	  case 5:
	  outer_radius = atof(argv[4]);
	  case 4:
	  inner_radius = atof(argv[3]);
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
	  printf("\n Usage: culllt <input lattice> "
		 "<output lattice> <inner radius> <outer radius>"
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
 * Cull the lattice:
 */

  lat->inner_radius = inner_radius;
  lat->outer_radius = outer_radius;
  lculllt(lat);

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


