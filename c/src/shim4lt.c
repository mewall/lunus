/* SHIM4LT.C - Generate an image of the full angular distribution of 
               intensity in a specified spherical shell of a lattice.
   
   Author: Mike Wall
   Date: 3/23/95
   Version: 1.
   
   Usage:
   		"shim4lt <input lattice> <output shim> <inner
		radius> <outer radius>"

		Input is a 3D lattice.  Output is a shell image.  
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *shimout,
    *latticein;
  
  char
    error_msg[LINESIZE];
  
  LAT3D 
    *lat;
  
  size_t
    index,
    i,
    j,
    k,
    *histogram,
    r,
    inner_radius,
    outer_radius;

  struct ijkcoords rvec;

  struct xyzcoords rfloat;

  float
    rscale;

  /*
   * Set input line defaults:
   */
  
  latticein = stdin;
  
  /*
   * Read information from input line:
   */
  switch(argc) {
    case 5:
    outer_radius = atol(argv[4]);
    case 4:
    inner_radius = atol(argv[3]);
    case 3:
    if (strcmp(argv[2],"-") == 0) {
      shimout = stdout;
    }
    else {
      if ( (shimout = fopen(argv[2],"w")) == NULL ) {
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
    printf("\n Usage: shim4lt <input lattice> <output shim> "
	   "<inner radius> <outer radius>\n\n");
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
   * Read in lattices:
   */

  lat->infile = latticein;
  if (lreadlt(lat) != 0) {
    perror("Couldn't read lattice.\n\n");
    exit(0);
  }

  /*
   * Generate shell image:
   */

  lat->inner_radius = inner_radius;
  lat->outer_radius = outer_radius;
  lshim4lt(lat);

  /*
   * Write shell image to a file:
   */

  lat->outfile = shimout;
  lwritesh(lat);

CloseShop:
  
  /*
   * Free allocated memory:
   */

  lfreelt(lat);

  /*
   * Close files:
   */
  
  fclose(latticein);
  fclose(shimout);
}

