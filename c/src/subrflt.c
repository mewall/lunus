/* SUBRFLT.C - Subtract a radial intensity distribution from each pixel
		in a 3D lattice.
   
   Author: Mike Wall
   Date: 3/28/93
   Version: 1.
   
   Usage:
   		"subrflt <input rfile> <input lattice> <output lattice>"

		Input is a radial intensity distribution and an input 
			lattice.  Output is a lattice.  
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*infile,
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

  RFILE_DATA_TYPE *rfile;

/*
 * Set input line defaults:
 */
	
	latticein = stdin;
	latticeout = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
		case 4:
			if (strcmp(argv[3],"-") == 0) {
				latticeout = stdout;
			}
			else {
			 if ((latticeout = fopen(argv[3],"wb")) == NULL) {
				printf("\nCan't open %s.\n\n",argv[3]);
				exit(0);
			 }
			}
		case 3:
			if (strcmp(argv[2],"-") == 0) {
				latticein = stdin;
			}
			else {
			 if ( (latticein = fopen(argv[2],"rb")) == NULL ) {
				printf("\nCan't open %s.\n\n",argv[2]);
				exit(0);
			 }
			}
		case 2:
			if ((infile = fopen(argv[1],"rb")) == NULL) {
				printf("\nCan't open %s.\n\n",argv[1]);
				exit(0);
			}
			break;
		default:
			printf("\n Usage: subrflt <input rfile> <input lattice> "
				"<output lattice>\n\n");
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
 * Read in rfile:
 */

  i=0;

  while ((num_read = fread(&lat->rfile[i], sizeof(RFILE_DATA_TYPE), 1, 
			   infile)) == 1) {
    i++;
  }
  
 lat->rfile_length = i;

/*
 * Generate the subtracted image:
 */

  lsubrflt(lat);

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
  
  fclose(infile);
  fclose(latticein);
  fclose(latticeout);
}

