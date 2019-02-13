/* SCALELT.C - Find the best scale factor relating two input lattices.
   
   Author: Mike Wall
   Date: 3/21/95
   Version: 1.
   
   Usage:
   		"scalelt <input lattice 1> <input lattice 2>"

		Input is two 3D lattices.  Output is a scale factor.  
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *latticein1,
    *latticein2;
  
  char
    error_msg[LINESIZE];
  
  LAT3D 
    *lat1,
    *lat2;
  
  size_t
    inner_radius,
    outer_radius;

  /*
   * Set input line defaults:
   */
  
  latticein1 = stdin;
  latticein2 = stdin;
  
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
      latticein2 = stdin;
    }
    else {
      if ( (latticein2 = fopen(argv[2],"rb")) == NULL ) {
	printf("\nCan't open %s.\n\n",argv[2]);
	exit(0);
      }
    }
    case 2:
    if (strcmp(argv[1],"-") == 0) {
      latticein1 = stdin;
    }
    else {
      if ( (latticein1 = fopen(argv[1],"rb")) == NULL ) {
	printf("\nCan't open %s.\n\n",argv[1]);
	exit(0);
      }
    }
    break;
    default:
    printf("\n Usage: scalelt <input lattice 1> <input lattice 2> "
	   "<inner radius> <outer radius>\n\n");
    exit(0);
  }
  
  /*
   * Initialize lattice:
   */
  
  if (((lat1 = linitlt()) == NULL) || ((lat2 = linitlt()) == NULL)) {
    perror("Couldn't initialize lattices.\n\n");
    exit(0);
  }
  
  /*
   * Read in lattices:
   */

  lat1->infile = latticein1;
  if (lreadlt(lat1) != 0) {
    perror("Couldn't read lattice.\n\n");
    exit(0);
  }

  lat2->infile = latticein2;
  if (lreadlt(lat2) != 0) {
    perror("Couldn't read lattice.\n\n");
    exit(0);
  }

/*
 * Generate the normalized difference squared:
 */

  lat1->inner_radius = inner_radius;
  lat1->outer_radius = outer_radius;
  lscalelt(lat1,lat2);

/*
 * Write rfile to output file:
 */

  printf("%f\n",lat1->rfile[0]);

CloseShop:
  
  /*
   * Free allocated memory:
   */

  lfreelt(lat1);
  lfreelt(lat2);

  /*
   * Close files:
   */
  
  fclose(latticein1);
  fclose(latticein2);
}

