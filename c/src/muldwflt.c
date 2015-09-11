/* MULDWFLT.C - Multiply lattice by an anisotropic Debye-Waller factor.
   
   Author: Mike Wall
   Date: 7/27/15
   Version: 1.
   
   Usage:
   		"dwflt <input lattice> <output lattice> <U11> <U22> <U33> <U12> <U13> <U23>"

		Input is lattice.  Output is lattice multiplied by Debye-Waller factor using aniso U parameters supplied.

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

  struct xyzcoords
    shift;

/*
 * Set input line defaults:
 */
	
	latticein = stdin;
	latticeout = stdout;
	
	shift.x=shift.y=shift.z=0.0;

/*
 * Read information from input line:
 */
	switch(argc) {
	case 12:
	  shift.z=atof(argv[11]);
	case 11:
	  shift.y=atof(argv[10]);
	case 10:
	  shift.x=atof(argv[9]);
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
	  printf("\n Usage: muldwflt <input lattice> "
		 "<output lattice> <U11> <U22> <U33> <U12> <U13> <U23>\n\n");
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

  memcpy(&lat->anisoU,&anisoU,sizeof(struct xyzmatrix));

  /*
   * Copy shift vector to lattice
   */

  memcpy(&lat->shift,&shift,sizeof(struct xyzcoords));

  //  printf("lat->anisoU.xx = %f\n",lat->anisoU.xx);

  /*
   * Muiltiply the lattice by the Debyw-Waller factor:
   */
  
  lmuldwflt(lat);
  
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


