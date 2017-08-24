/* CUBE2MAP.C - Convert Gaussian .cube format to CCP4 .map format.
   
   Author: Mike Wall
   Date: 2/25/2014
   Version: 1.
   
   Usage:
   		"cube2map <input cube> <output map>"

		Input is electron density map in cube format. 
			Output is electron density map in CCP4 .map format.  
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *cubein,
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
  
/*
 * Set input line defaults:
 */
	cubein = stdin;
	mapout = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
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
	    cubein = stdin;
	  }
	  else {
	    strcpy(filename,argv[1]);
	    if ( (cubein = fopen(argv[1],"rb")) == NULL ) {
	      printf("\nCan't open %s.\n\n",argv[1]);
	      exit(0);
	    }
	  }
	  break;
	  default:
	  printf("\n Usage: cube2map <input cube> <output map>\n\n");
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

  map->infile = cubein;
  if (lreadcube(map) != 0) {
    perror("Couldn't read cube file.\n\n");
    exit(0);
  }

  printf("Read the cube file\n");

/*
 * Write lattice to output file:
 */

  printf("Writing file\n");

  map->outfile = mapout;
  if (lwritemap(map) != 0) {
    perror("Couldn't write map file.\n\n");
    exit(0);
  }

  printf("Done writing file\n");

CloseShop:
  
  /*
   * Free allocated memory:
   */

  //  lfreemap(map);

  /*
   * Close files:
   */
  
  fclose(cubein);
  fclose(mapout);
}

