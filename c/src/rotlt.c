/* ROTLT.C - Rotate a lattice according to input line instructions.
   
   Author: Mike Wall
   Date: 7/17/2014
   Version: 1.
   
   Usage:
   		"rotlt <input lattice> <output lattice> <axis code> <angle>"

		Input are lattice, axis code specification (1 = x; 2 = y; 3 = z)
		and angle.  Output is lattice rotated by angle about the specified axis.
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*latticein,
	*latticeout;

  char
    error_msg[LINESIZE],
    cell_str[256];

  int
    axis;

  float
    angle;

  LAT3D 
	*lat;

  RFILE_DATA_TYPE *rfile;

  struct ijkcoords
    origin;

/*
 * Set input line defaults:
 */
	
	latticein = stdin;
	latticeout = stdout;
	angle = 90.0;

  strcpy(cell_str,"None");

/*
 * Read information from input line:
 */
	switch(argc) {
    case 9: 
    origin.k = atol(argv[8]);
    case 8:
    origin.j = atol(argv[7]);
    case 7:
    origin.i = atol(argv[6]);
    case 6:
      strcpy(cell_str,argv[5]);
	case 5:
	  angle = atof(argv[4]);
	  case 4:
	  axis = atol(argv[3]);
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
	  printf("\n Usage: rotlt <input lattice> "
		 "<output lattice> <axis code> <angle>\n\n"
		 "  Axis Codes:\n"
		 "    1 = x\n"
		 "    2 = y\n"
		 "    3 = z\n\n");
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

  if (!(strcmp(cell_str,"None")==0)) {
    strcpy(lat->cell_str,cell_str);
  } else {
    lat->cell.a = 1./lat->xscale;
    lat->cell.b = 1./lat->yscale;
    lat->cell.c = 1./lat->zscale;
    lat->cell.alpha = lat->cell.beta = lat->cell.gamma = 90.0;
    sprintf(cell_str,"%f,%f,%f,90.0,90.0,90.0",(float)lat->cell.a,(float)lat->cell.b,(float)lat->cell.c);
    printf("%s\n",cell_str);
  }
  lparsecelllt(lat);
  printf("%f,%f,%f,%f,%f,%f\n",(float)lat->cell.a,(float)lat->cell.b,(float)lat->cell.c,(float)lat->cell.alpha,(float)lat->cell.beta,(float)lat->cell.gamma);

  /*
   * Perform symmetry operation:
   */

  if (argc==9) {
    lat->origin.i=origin.i; lat->origin.j=origin.j; lat->origin.k=origin.k;
  }
  lat->axis = axis;
  lat->angle = angle;
  lrotlt(lat);
  
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


