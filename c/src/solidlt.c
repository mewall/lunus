/* SOLIDLT.C - Apply solid angle correction to lattice.
   
   Author: Mike Wall
   Date: 3/25/95
   Version: 1.
   
   Usage:
   		"solidlt <input lattice> <output lattice>"
		
		Input is a lattice.  Output is a lattice.  

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
  
  /*
   * Set input line defaults:
   */
  latticein = stdin;
  latticeout = stdout;
  
  /*
   * Read information from input line:
   */
  switch(argc) {
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
    printf("\n Usage: solidlt <input lattice> <output lattice>\n\n");
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
   * Generate the corrected lattice:
   */
  
  lsolidlt(lat);
  
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

