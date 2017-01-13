/* MAKELT.C - Create a .lat file using the provided unit cell and resolution info.
   
   Author: Mike Wall
   Date: 1/13/2017
   Version: 1.
   
   Usage:
   		"makelt <output lattice> <cell_str> <resolution>"

			Outputs a lattice in .lat format.  
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
  

  char cell_str[256];

  float res;

/*
 * Set input line defaults:
 */
	latticeout = stdout;
	res = 1.6;

/*
 * Read information from input line:
 */
	switch(argc) {
	  case 4:
	    res = atof(argv[3]);
	  case 3:
	    strcpy(cell_str,argv[2]);
	  case 2:
	  if (strcmp(argv[1],"-") == 0) {
	    latticeout = stdout;
	  }
	  else {
	    strcpy(filename,argv[1]);
	    if ( (latticeout = fopen(argv[1],"r")) == NULL ) {
	      printf("\nCan't open %s.\n\n",argv[1]);
	      exit(0);
	    }
	  }
	  break;
	  default:
	  printf("\n Usage: makelt <output .lat> <cell_str> <resolution>\n\n");
	  exit(0);
	  }
  
  /*
   * Initialize lattice:
   */

  if ((lat = linitlt()) == NULL) {
    perror("Couldn't initialize lattice.\n\n");
    exit(0);
  }
  
  strcpy(lat->cell_str,cell_str);
  lparsecelllt(lat);

  // resize lattice for unit cell

  lat->xvoxels = ((uint32_t)(lat->cell.a/res)+1)*2;
  lat->yvoxels = ((uint32_t)(lat->cell.b/res)+1)*2;
  lat->zvoxels = ((uint32_t)(lat->cell.c/res)+1)*2;
  
  lat->xyvoxels = lat->xvoxels*lat->yvoxels;
  lat->lattice_length = lat->xyvoxels*lat->zvoxels;

  lat->xscale = 1./lat->cell.a;
  lat->yscale = 1./lat->cell.b;
  lat->zscale = 1./lat->cell.c;

  lat->origin.i = lat->xvoxels/2 - 1;
  lat->origin.j = lat->yvoxels/2 - 1;
  lat->origin.k = lat->zvoxels/2 - 1;

  lat->xbound.min = - lat->origin.i*lat->xscale;               
  lat->xbound.max = (lat->xvoxels - lat->origin.i - 1)*lat->xscale;
  lat->ybound.min = - lat->origin.j*lat->yscale; 
  lat->ybound.max = (lat->yvoxels - lat->origin.j - 1)*lat->yscale;
  lat->zbound.min = - lat->origin.k*lat->zscale; 
  lat->zbound.max = (lat->zvoxels - lat->origin.k - 1)*lat->zscale;

/*
 * Write lattice to output file:
 */

  printf("Writing file\n");

  lat->outfile = latticeout;
  if (lwritelt(lat) != 0) {
    perror("Couldn't write lattice.\n\n");
    exit(0);
  }

  //  printf("Done writing file\n");

CloseShop:
  
  /*
   * Free allocated memory:
   */

  //  lfreelt(lat);

  /*
   * Close files:
   */
  fclose(latticeout);
}

