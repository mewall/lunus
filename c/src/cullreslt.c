/* CULLRESLT.C - Throw away part of a lattice.
   
   Author: Mike Wall
   Date: 2/1/2017
   Version: 1.
   
   Usage:
   		"cullreslt <input lattice> <output lattice> <low res> <high res> <unit cell string>"

		Input is a scalar and a resolution range plus unit cell. 
		Output is a culled lattice, with voxels in the
		specified resolution range preserved.  
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
  
  struct bounds resolution;

  float
    inner_radius,
    outer_radius;

  char cell_str[256];

/*
 * Set input line defaults:
 */
  latticein = stdin;
  latticeout = stdout;
  strcpy(cell_str, "default");

/*
 * Read information from input line:
 */
	switch(argc) {
    case 6:
      strcpy(cell_str,argv[5]);
	  case 5:
	  resolution.min = atof(argv[4]);
	  case 4:
	  resolution.max = atof(argv[3]);
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
	  printf("\n Usage: cullreslt <input lattice> "
		 "<output lattice> <low resolution> <high resolution> <unit cell string>"
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

  lat->resolution.min = resolution.min;
  lat->resolution.max = resolution.max;
  lcullreslt(lat);

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


