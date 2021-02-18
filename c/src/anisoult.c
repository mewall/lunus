/* ANISOULT.C - Generate a lattice with an anisotropic U distribution about the origin.
   
   Author: Mike Wall
   Date: 2/28/95; 4/21/15
   Version: 1.
   
   Usage:
   		"anisoult <input lattice> <output lattice> <U11> <U22> <U33> <U12> <U13> <U23>"

		Input is lattice.  Output is anisotropic U distribution using parameters supplied.

		NOTE: The U matrix must be in a cartesian coordinate system, such as found in the PDB format.
		      SHELX U matrix will also work for orthogonal unit cells, otherwise will need transform.
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *latticein,
    *latticeout;
  
  char
    error_msg[LINESIZE];

  LAT3D 
    *lat;

  float
    peak,
    width;

  struct xyzmatrix
    anisoU;

/*
 * Set input line defaults:
 */
	
	latticein = stdin;
	latticeout = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
	  case 9:
	  anisoU.yz = atof(argv[8]);
	  anisoU.zy = anisoU.yz;
	  case 8:
	  anisoU.xz = atof(argv[7]);
	  anisoU.zx = anisoU.xz;
	  case 7:
	  anisoU.xy = atof(argv[6]);
	  anisoU.yx = anisoU.xy;
	  case 6:
	  anisoU.zz = atof(argv[5]);
	  case 5:
	  anisoU.yy = atof(argv[4]);
	  case 4:
	  anisoU.xx = atof(argv[3]);
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
	    if ( (latticein = fopen(argv[1],"rb")) == NULL ) {
	      printf("\nCan't open %s.\n\n",argv[1]);
	      exit(0);
	    }
	  }
	  break;
	  default:
	  printf("\n Usage: normlt <input lattice> "
		 "<output lattice> <width>\n\n");
	  exit(0);
	}
  
  /*
   * Initialize lattices:
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
   * Copy aniso U to lattice
   */

  /*
  lat->anisoU.xx = anisoU.xx;
  lat->anisoU.xy = anisoU.xy;
  lat->anisoU.xz = anisoU.xz;
  lat->anisoU.yx = anisoU.yx;
  lat->anisoU.yy = anisoU.yy;
  lat->anisoU.yz = anisoU.yz;
  lat->anisoU.zx = anisoU.zx;
  lat->anisoU.zy = anisoU.zy;
  lat->anisoU.zz = anisoU.zz;
  */
  memcpy(&lat->anisoU,&anisoU,sizeof(struct xyzmatrix));

  //  printf("lat->anisoU.xx = %f\n",lat->anisoU.xx);

  /*
   * Calculate normal distribution lattice:
   */
  
  lanisoult(lat);
  
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


