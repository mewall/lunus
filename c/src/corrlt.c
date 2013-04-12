/* CORRLT.C - Calculate the correlation coefficient between two lattices.
   
   Author: Mike Wall
   Date: 1/22/2013
   Version: 1.
   
   Usage:
   		"corrlt <input lattice 1> <input lattice 2> <threshold>"

		Input is two 3D lattices and threshold below which voxels in lattice 1 are ignored. 
		Output is the correlation coefficient written to stdout.  
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *latticein1,
    *latticein2,
    *latticeout;
  
  char
    error_msg[LINESIZE];
  
  size_t
    i,
    j,
    k,
    num_wrote;
  
  LAT3D 
    *lat1,
    *lat2;
  
  RFILE_DATA_TYPE *rfile;

  LATTICE_DATA_TYPE threshold = -32768.f;

  struct ijkcoords
    origin;

  /*
   * Set input line defaults:
   */
  
  latticein1 = stdin;
  latticein2 = stdin;
  latticeout = stdout;
  
  /*
   * Read information from input line:
   */
  switch(argc) {
    case 4:
      threshold = atof(argv[3]);
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
    printf("\n Usage: corrlt <input lattice 1> <input lattice 2> "
	   "<threshold>\n\n");
    exit(0);
  }
  
  /*
   * Initialize lattice:
   */
  
  if (((lat1 = linitlt()) == NULL) || ((lat2 = linitlt()) == NULL)) {
    perror("Couldn't initialize lattices.\n\n");
    exit(0);
  }

  lat1->threshold = threshold;
  
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
 * Calculate the Pearson Correlation:
 */

  float corr;

  corr = lcorrlt(lat1,lat2);

  printf("C = %g\n",corr);

CloseShop:
  
  /*
   * Free allocated memory:
   */

  lfreelt(lat1);
  lfreelt(lat2);

  /*
   * Close files:
   */
  
  fclose(latticeout);
  fclose(latticein1);
  fclose(latticein2);
}

