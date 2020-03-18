/* HDRLT.C - Generate a gaussian lattice.
   
   Author: Mike Wall
   Date: 3/2/95
   Version: 1.
   
   Usage:
   		"hdrlt <input lattice> <output lattice>"

		Input is lattice.  Output is header of lattice to stdout.
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *latticein;
  
  char
    error_msg[LINESIZE];

  char cell_str[256];

  LAT3D 
    *lat;

/*
 * Set input line defaults:
 */
	
	latticein = stdin;
  strcpy(cell_str, "default");

/*
 * Read information from input line:
 */
	switch(argc) {
    case 3:
      strcpy(cell_str,argv[2]);
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
	  printf("\n Usage: hdrlt <input lattice> \n\n");
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

  strcpy(lat->cell_str,cell_str);
  lparsecelllt(lat);

  /*
   * Write lattice info to stdout:
   */

  printf("SIZE: %d %d %d\n",lat->xvoxels, lat->yvoxels, lat->zvoxels);
  printf("BOUNDS: (%f,%f) (%f,%f) (%f,%f)\n", 
	 lat->xbound.min,lat->xbound.max,
	 lat->ybound.min,lat->ybound.max,
	 lat->zbound.min,lat->zbound.max);
  printf("ORIGIN: (%ld,%ld,%ld)\n",lat->origin.i,lat->origin.j,lat->origin.k);
  printf("VOXEL SCALE: %f %f %f\n",(float)lat->xscale,(float)lat->yscale,
	 (float)lat->zscale);
  /* printf("VOXEL DIAGONAL: %f\n",sqrtf((double)(lat->xscale*lat->xscale + */
  /* 					      lat->yscale*lat->yscale + */
  /* 					      lat->zscale*lat->zscale))); */
  printf("RECIPROCAL LATTICE DIAGONAL: %f\n",sqrtf(ldotvec(lat->cellstardiag,lat->cellstardiag)));
CloseShop:
  
  /*
   * Free allocated memory:
   */

  lfreelt(lat);

  /*
   * Close files:
   */
  
  fclose(latticein);
}


