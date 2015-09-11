/* RESIZELT.C - Resize a lattice.
   
   Author: Mike Wall
   Date: 9/4/2015
   Version: 1.
   
   Usage:
   		"resizelt <input lattice> <output lattice> <xvoxels> <yvoxels> <zvoxels>"

		Input an input 
			lattice.  Output is a lattice resized using the specified x,y,z voxels.  
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *latticein,
    *latticeout;
  
  char
    error_msg[LINESIZE];
  
  size_t
    i,
    j,
    k,
    xv,
    yv,
    zv,
    num_read,
    num_wrote;
  
  LAT3D 
    *lat1,*lat2;
  
  RFILE_DATA_TYPE 
    *rfile;
  
  float
    scale_factor;

/*
 * Set input line defaults:
 */
	scale_factor = 1.;
	latticein = stdin;
	latticeout = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
	  case 6:
	  zv = atoi(argv[5]);
	  case 5:
	  yv = atoi(argv[4]);
	  case 4:
	  xv = atoi(argv[3]);
	  case 3:
	  if (strcmp(argv[2],"-") == 0) {
	    latticeout = stdout;
	  }
	  else {
	    if ((latticeout = fopen(argv[2],"wb")) == NULL) {
	      printf("\nCan't open %s.\n\n",argv[1]);
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
	  printf("\n Usage: resizelt <input lattice> <output lattice> "
		 "<xvoxels> <yvoxels> <zvoxels>\n\n");
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
   * Read in lattice:
   */

  lat1->infile = latticein;
  if (lreadlt(lat1) != 0) {
    perror("Couldn't read lattice.\n\n");
    exit(0);
  }

  /*
   * Set new size of latteice
   */

  lat2->xscale=lat1->xscale; lat2->yscale=lat1->yscale; lat2->zscale=lat1->zscale;
  lat2->xvoxels=xv; lat2->yvoxels=yv; lat2->zvoxels=zv;
  lat2->origin.i=(xv-1)/2; lat2->origin.j=(yv-1)/2; lat2->origin.k=(zv-1)/2;
  lat2->xyvoxels = lat2->xvoxels * lat2->yvoxels;
  lat2->lattice_length = lat2->xyvoxels*lat2->zvoxels;
  lat2->xbound.min = -lat2->origin.i*lat2->xscale;
  lat2->ybound.min = -lat2->origin.j*lat2->yscale;
  lat2->zbound.min = -lat2->origin.k*lat2->zscale;
  lat2->xbound.max = lat2->xbound.min + (lat2->xvoxels-1)*lat2->xscale;
  lat2->ybound.max = lat2->ybound.min + (lat2->yvoxels-1)*lat2->yscale;
  lat2->zbound.max = lat2->zbound.min + (lat2->zvoxels-1)*lat2->zscale;

  /*
   * Allocate memory for 3D lattice:
   */
  
  if (lat2->lattice) free((LATTICE_DATA_TYPE *)lat2->lattice);
  lat2->lattice = (LATTICE_DATA_TYPE *)calloc(lat2->lattice_length,
					     sizeof(LATTICE_DATA_TYPE));
  if (!lat2->lattice) {
    printf("\nCouldn't allocate 3D lattice.\n\n");
    goto CloseShop;
  }

/*
 * Resize the lattice:
 */

  lresizelt(lat1,lat2);

/*
 * Write lattice to output file:
 */

  lat2->outfile = latticeout;
  if (lwritelt(lat2) != 0) {
    perror("Couldn't write lattice.\n\n");
    exit(0);
  }

CloseShop:
  
  /*
   * Free allocated memory:
   */

  //  lfreelt(lat);

  /*
   * Close files:
   */
  
  fclose(latticein);
  fclose(latticeout);
}

