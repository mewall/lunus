/* CULLCONELT.C - Throw away part of a lattice shaped like a cone.
   
   Author: Mike Wall
   Date: 9/28/2016
   Version: 1.
   
   Usage:
   		"cullconelt <input lattice> <output lattice> <cell_str> <axis>"

		Input is a lattice and axis specification. 
		Output is a culled lattice, with voxels in the cone about the axis
		masked.  
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
  

  int
    axis;

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
	  axis = atoi(argv[4]);
	case 4:
	  strcpy(cell_str,argv[3]);
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

  strcpy(lat->cell_str,cell_str);
  lparsecelllt(lat);
  lat->axis = axis;
  lcullconelt(lat);

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


