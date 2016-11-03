/* LLMHYBLT.C - Generate a hybrid static disorder + liquid-like motions model.
   
   Author: Mike Wall
   Date: 10/20/2016
   Version: 1.
   
   Usage:
   		"llmhyblt <input |<F>|^2 lattice> <input <|F|^2> lattice> <output lattice> <cell_str> <gamma> <chi> <sigma1> <sigma2>"

		Input is I0(hkl) lattice, unit cell, gamma, sigma1, sigma2 from llm model.  Output is liquid-like motions model of diffuse scattering.
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *latticein1,
    *latticein2,
    *latticeout;
  
  char
    cell_str[256],
    error_msg[LINESIZE];

  LAT3D 
    *lat1,
    *lat2;

  float
    sigma1,
    sigma2,
    chi,
    gamma;

/*
 * Set input line defaults:
 */
	
	latticeout = stdout;

	sigma1 = 0.0;
/*
 * Read information from input line:
 */
	switch(argc) {
	  case 8:
	  sigma1 = atof(argv[7]);
	  case 7:
	  chi = atof(argv[6]);
	  case 6:
	  gamma = atof(argv[5]);
	case 5:
	  strcpy(cell_str,argv[4]);
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
	    if ( (latticein2 = fopen(argv[2],"rb")) == NULL ) {
	      printf("\nCan't open %s.\n\n",argv[2]);
	      exit(0);
	    }
	  case 2:
	    if ( (latticein1 = fopen(argv[1],"rb")) == NULL ) {
	      printf("\nCan't open %s.\n\n",argv[1]);
	      exit(0);
	    }
	  break;
	  default:
	  printf("\n Usage: liquidcorrlt <input lattice> "
		 "<output lattice> <sigma> <gamma>\n\n");
	  exit(0);
	}
  
	if (sigma2 == 0.0) {
	  sigma2 = sigma1;
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

  lat1->infile = latticein1;
  if (lreadlt(lat1) != 0) {
    perror("Couldn't read lattice.\n\n");
    exit(0);
  }
  lat2->infile = latticein2;
  if (lreadlt(lat2) != 0) {
    perror("Couldn't read lattice.\n\n");
    exit(0);
  }

  /*
   * Define parameters:
   */
  
  lat1->anisoU.xx = sigma1*sigma1;
  lat1->anisoU.xy = 0.0;
  lat1->anisoU.xz = 0.0;
  lat1->anisoU.yx = 0.0;
  lat1->anisoU.yy = sigma2*sigma2;
  lat1->anisoU.yz = 0.0;
  lat1->anisoU.zx = 0.0;
  lat1->anisoU.zy = 0.0;
  lat1->anisoU.zz = sigma1*sigma1;
  lat1->chi = chi;
  lat1->gamma = gamma;
  strcpy(lat1->cell_str,cell_str);
  lparsecelllt(lat1);
  strcpy(lat2->cell_str,cell_str);
  lparsecelllt(lat2);
  lllmhyblt(lat1,lat2);
  
  /*
   * Write lattice to output file:
   */
  
  lat1->outfile = latticeout;
  if (lwritelt(lat1) != 0) {
    perror("Couldn't write lattice.\n\n");
    exit(0);
  }

CloseShop:
  
  /*
   * Free allocated memory:
   */

  lfreelt(lat1);
  lfreelt(lat2);

  /*
   * Close files:
   */
  
  fclose(latticein1);
  fclose(latticein2);
  fclose(latticeout);
}


