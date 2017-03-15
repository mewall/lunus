/* MINRLT.C - Calculate the minimum voxel value vs. radius for an 
              input lattice
   
   Author: Mike Wall
   Date: 3/1/95
   Version: 1.
   
   Usage:
   		"avgrlt <input lattice> <output rfile> <x-origin>
		        <y-origin> <z-origin>"

		Input is a 3D lattice and voxel coordinates for the
		origin.  Output is an rfile.  
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *latticein,
    *outfile;
  
  char
    error_msg[LINESIZE];
  
  size_t
    i,
    j,
    k,
    num_wrote;
  
  LAT3D 
    *lat;
  
  RFILE_DATA_TYPE *rfile;
  
  struct ijkcoords
    origin;

  /*
   * Set input line defaults:
   */
  
  latticein = stdin;
  outfile = stdout;
  
  /*
   * Read information from input line:
   */
  switch(argc) {
    case 6: 
    origin.k = atol(argv[5]);
    case 5:
    origin.j = atol(argv[4]);
    case 4:
    origin.i = atol(argv[3]);
    case 3:
    if ((outfile = fopen(argv[2],"wb")) == NULL) {
      printf("\nCan't open %s.\n\n",argv[2]);
      exit(0);
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
    printf("\n Usage: avgrlt <input lattice> "
	   "<output rfile> <x-origin> "
	   "<y-origin> <z-origin>\n\n");
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
 * Generate the minimum vs radius:
 */

  lminrlt(lat);

/*
 * Write rfile to output file:
 */

  num_wrote = fwrite(lat->rfile, sizeof(RFILE_DATA_TYPE),
		     lat->rfile_length, outfile);
  if (num_wrote != lat->rfile_length) {
    printf("\nCouldn't write rfile.\n\n");
    goto CloseShop;
  }

CloseShop:
  
  /*
   * Free allocated memory:
   */

  lfreelt(lat);

  /*
   * Close files:
   */
  
  fclose(outfile);
  fclose(latticein);
}

