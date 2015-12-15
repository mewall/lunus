/* SHIFTSFLT.C - Apply a real-space shift to a structure factor lattice.
   
   Author: Mike Wall
   Date: 7/31/15
   Version: 1.
   
   Usage:
   		"shiftsflt <input real lattice> <input imag lattice> <output real lattice> <output imag lattice> <x shift> <y shift> <z shift>"

		Input is structure factor lattice. Output is a real-space shifted structure factor lattice

   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *lat1in,
    *lat2in,
    *lat1out,
    *lat2out;
  
  char
    error_msg[LINESIZE];

  LAT3D 
    *lat1, *lat2;

  struct xyzcoords
    shift;

/*
 * Set input line defaults:
 */
	
	shift.x=shift.y=shift.z=0.0;

/*
 * Read information from input line:
 */
	switch(argc) {
	case 8:
	  shift.z=atof(argv[7]);
	case 7:
	  shift.y=atof(argv[6]);
	case 6:
	  shift.x=atof(argv[5]);
	case 5:
	  if ((lat2out = fopen(argv[4],"wb")) == NULL) {
	    printf("\nCan't open %s.\n\n",argv[4]);
	    exit(0);
	  }
	case 4:
	  if ((lat1out = fopen(argv[3],"wb")) == NULL) {
	    printf("\nCan't open %s.\n\n",argv[3]);
	    exit(0);
	  }
	case 3:
	  if ( (lat2in = fopen(argv[2],"rb")) == NULL ) {
	    printf("\nCan't open %s.\n\n",argv[2]);
	    exit(0);
	  }
	case 2:
	  if ( (lat1in = fopen(argv[1],"rb")) == NULL ) {
	    printf("\nCan't open %s.\n\n",argv[1]);
	    exit(0);
	  }
	  break;
	default:
	  printf("\n Usage: shiftsflt <input real lattice> <input imag lattice>"
		 "<output real lattice> <output imag lattice> <shift x> <shift y> <shift z>\n\n");
	  exit(0);
	}
  
  /*
   * Initialize lattices:
   */

  if ((lat1 = linitlt()) == NULL) {
    perror("Couldn't initialize lattice.\n\n");
    exit(0);
  }

  if ((lat2 = linitlt()) == NULL) {
    perror("Couldn't initialize lattice.\n\n");
    exit(0);
  }
  
  /*
   * Read in lattices:
   */

  lat1->infile = lat1in;
  if (lreadlt(lat1) != 0) {
    perror("Couldn't read lattice.\n\n");
    exit(0);
  }

  lat2->infile = lat2in;
  if (lreadlt(lat2) != 0) {
    perror("Couldn't read lattice.\n\n");
    exit(0);
  }

  /* 
   * Copy shift vector to lattice
   */

  // The input shift units are fraction of unit cell. Change to number of grid points.
  lat1->shift.x = shift.x*(float)lat1->xvoxels;
  lat1->shift.y = shift.y*(float)lat1->yvoxels;
  lat1->shift.x = shift.z*(float)lat1->zvoxels;

  //  memcpy(&lat1->shift,&shift,sizeof(struct xyzcoords));

  //  printf("lat->anisoU.xx = %f\n",lat->anisoU.xx);

  // Calculate shifted lattice:

  lshiftsflt(lat1,lat2);
  
  /*
   * Write lattices to output files:
   */
  
  lat1->outfile = lat1out;
  if (lwritelt(lat1) != 0) {
    perror("Couldn't write lattice.\n\n");
    exit(0);
  }

  lat2->outfile = lat2out;
  if (lwritelt(lat2) != 0) {
    perror("Couldn't write lattice.\n\n");
    exit(0);
  }

CloseShop:
  

  /*
   * Close files:
   */
  
  fclose(lat1in);
  fclose(lat2in);
  fclose(lat1out);
  fclose(lat2out);
}


