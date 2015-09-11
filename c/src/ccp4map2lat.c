/* CCP4MAP2LAT.C - Convert standard CCP4 .map format to internal .lat.
   
   Author: Mike Wall
   Date: 2/13/2014
   Version: 1.
   
   Usage:
   		"ccp4map2lat <input lattice> <output lattice>"

		Input is a map in CCP4 format. 
			Output is a lattice in internal format.  
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *mapin,
    *latticeout;
  
  char
    error_msg[LINESIZE],filename[256];
  
  size_t
    i,
    j,
    k,
    num_read,
    num_wrote;
  
  LAT3D 
    *lat;

  CCP4MAP
    *map;
  
/*
 * Set input line defaults:
 */
	mapin = stdin;
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
	      printf("\nCan't open %s.\n\n",argv[2]);
	      exit(0);
	    }
	  }
	  case 2:
	  if (strcmp(argv[1],"-") == 0) {
	    mapin = stdin;
	  }
	  else {
	    strcpy(filename,argv[1]);
	    if ( (mapin = fopen(argv[1],"rb")) == NULL ) {
	      printf("\nCan't open %s.\n\n",argv[1]);
	      exit(0);
	    }
	  }
	  break;
	  default:
	  printf("\n Usage: ccp4map2lat <input ccp4 map> <output lattice>\n\n");
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
   * Initialize map:
   */

  if ((map = linitmap()) == NULL) {
    perror("Couldn't initialize map.\n\n");
    exit(0);
  }
  
  /*
   * Read in map:
   */

  map->filename = filename;
  map->infile = mapin;
  if (lreadmap(map) != 0) {
    perror("Couldn't read map.\n\n");
    exit(0);
  }

  /* 
   * Copy map to lattice:
   */

  if (lcpmaplt(map,lat) != 0) {
    perror("Couldn't copy map to lattice\n\n");
    exit(0);
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
  
  fclose(mapin);
  fclose(latticeout);
}

