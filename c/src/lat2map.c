/* LAT2MAP.C - Convert lunus .lat format to CCP4 .map format.
   
   Author: Mike Wall
   Date: 2/25/2014
   Version: 1.
   
   Usage:
   		"lat2map <input lat> <output map> <output map template>"

		Input is electron density map in cube format. 
			Output is electron density map in CCP4 .map format.  
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *latticein,
    *mapin,
    *mapout;
  
  char
    error_msg[LINESIZE],filename[256];
  
  size_t
    i,
    j,
    k,
    num_read,
    num_wrote;
  
  CCP4MAP 
    *map;

  LAT3D
    *lat;
/*
 * Set input line defaults:
 */
	latticein = stdin;
	mapout = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
	  case 4:
	    if ((mapin = fopen(argv[3],"rb")) == NULL) {
	      printf("\nCan't open %s.\n\n",argv[2]);
	      exit(0);
	    }
	  case 3:
	  if (strcmp(argv[2],"-") == 0) {
	    mapout = stdout;
	  }
	  else {
	    if ((mapout = fopen(argv[2],"wb")) == NULL) {
	      printf("\nCan't open %s.\n\n",argv[2]);
	      exit(0);
	    }
	  }
	  case 2:
	  if (strcmp(argv[1],"-") == 0) {
	    latticein = stdin;
	  }
	  else {
	    strcpy(filename,argv[1]);
	    if ( (latticein = fopen(argv[1],"rb")) == NULL ) {
	      printf("\nCan't open %s.\n\n",argv[1]);
	      exit(0);
	    }
	  }
	  break;
	  default:
	  printf("\n Usage: lat2map <input lat> <output map> <output map template>\n\n");
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
   * Read in map template:
   */

  map->infile = mapin;
  if (lreadmap(map) != 0) {
    perror("Couldn't read map file.\n\n");
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
   * Copy lattice to map
   */

  if (lcpltmap(lat,map)!=0) {
    perror("Couldn't copy lattice to map\n\n");
    exit(0);
  }

/*
 * Write map to output file:
 */

  printf("Writing file\n");

  map->outfile = mapout;
  if (lwritemap(map) != 0) {
    perror("Couldn't write map file.\n\n");
    exit(0);
  }

  //  printf("Done writing file\n");

CloseShop:
  
  /*
   * Free allocated memory:
   */

  //  lfreemap(map);

  /*
   * Close files:
   */
  
  fclose(latticein);
  fclose(mapin);
  fclose(mapout);
}

