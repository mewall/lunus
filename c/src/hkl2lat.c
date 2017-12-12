/* HKL2LAT.C - Convert h,k,l,I format to .lat format.
   
   Author: Mike Wall
   Date: 1/18/2013
   Version: 1.
   
   Usage:
   		"hkl2lat <input lattice> <output lattice> <template>"

		Input is a lattice in hkl format, using template .lat for header info. 
			Output is a lattice in .lat format.  
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *latticein,
    *latticeout,
    *tmplin;
  
  char
    error_msg[LINESIZE],filename[256];
  
  size_t
    i,
    j,
    k,
    num_read,
    num_wrote;
  
  LAT3D 
    *lat,
    *tmpl;
  
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
	    tmplin = stdin;
	  }
	  else {
	    if ( (tmplin = fopen(argv[3],"r")) == NULL ) {
	      printf("\nCan't open %s.\n\n",argv[3]);
	      exit(0);
	    }
	  }
	  case 3:
	  if (strcmp(argv[2],"-") == 0) {
	    latticeout = stdout;
	  }
	  else {
	    if ((latticeout = fopen(argv[2],"w")) == NULL) {
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
	    if ( (latticein = fopen(argv[1],"r")) == NULL ) {
	      printf("\nCan't open %s.\n\n",argv[1]);
	      exit(0);
	    }
	  }
	  break;
	  default:
	  printf("\n Usage: hkl2lat <input .hkl> <output .lat> <template .lat>\n\n");
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
   * Initialize template:
   */

  if ((tmpl = linitlt()) == NULL) {
    perror("Couldn't initialize template.\n\n");
    exit(0);
  }

  /*
   * Read in template:
   */

  tmpl->infile = tmplin;
  if (lreadlt(tmpl) != 0) {
    perror("Couldn't read template.\n\n");
    exit(0);
  }

  /*
   * Read in .hkl file using template
   */

  lat->infile = latticein;

  if (lreadhkl(lat,tmpl) != 0) {
    perror("Couldn't read hkl file.\n\n");
    exit(0);
  }

/*
 * Write lattice to output file:
 */

  printf("Writing file\n");

  lat->outfile = latticeout;
  if (lwritelt(lat) != 0) {
    perror("Couldn't write lattice.\n\n");
    exit(0);
  }

  printf("Done writing file\n");

CloseShop:
  
  /*
   * Free allocated memory:
   */

  //  lfreelt(lat);

  /*
   * Close files:
   */
  fclose(tmplin);
  fclose(latticein);
  fclose(latticeout);
}

