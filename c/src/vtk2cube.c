/* VTK2CUBE.C - Convert standard .vtk with lunus header info to standard .cube.
   
   Author: Mike Wall
   Date: 9/20/2015
   Version: 1.
   
   Usage:
   		"vtk2cube <input .vtk> <output .cube>"

		Input is a lattice in VTK format. 
		The header may contain the following tags in the second line, separated by semicolons:
		lattice_type=(P1,AU) (P1 = expanded to P1, AU = asymmetric unit)
		unit_cell=a,b,c,alpha,beta,gamma
		space_group=spacegroup
		
		Output is a lattice in .cube format.  
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *latticein,
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
	  printf("\n Usage: vtk2cube <input .vtk> <output .cube>\n\n");
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

  printf("reading .vtk file...\n");

  lat->filename = filename;
  lat->infile = latticein;
  if (lreadvtk(lat) != 0) {
    perror("Couldn't read lattice.\n\n");
    exit(0);
  }

/*
 * Write lattice to output file:
 */

  printf("Writing file\n");

  lat->outfile = latticeout;
  if (lwritecube(lat) != 0) {
    perror("Couldn't write lattice.\n\n");
    exit(0);
  }

  printf("Done writing file\n");

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

