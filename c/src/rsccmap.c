/* RSCCMAP.C - Calculate real space corrlation coefficients given two maps and a structure.
   
   Author: Mike Wall
   Date: 7/6/2015
   Version: 1.
   
   Usage:
   		"rsccmap <input ccp4 map 1> <input ccp4 map 2> <structure .xyz>"

		Input two maps in CCP4 format, and a structure. 
			Output is RSCC stats to stdout.  
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *mapin1,
    *mapin2,
    *xyzin;
  
  char
    error_msg[LINESIZE],filename[256];
  
  size_t
    i,
    num_read,
    num_wrote;

  CCP4MAP
    *map1,*map2;

  struct xyzcoords 
    *xyzlist;

  float arad=0.5;

/*
 * Set input line defaults:
 */
	mapin1 = stdin;

/*
 * Read information from input line:
 */
	switch(argc) {

	case 5:
	  arad = atof(argv[4]);
	  case 4:
	    if ( (xyzin = fopen(argv[3],"r")) == NULL ) {
	      printf("\nCan't open %s.\n\n",argv[3]);
	      exit(0);
	    }
	  case 3:
	    if ( (mapin2 = fopen(argv[2],"rb")) == NULL ) {
	      printf("\nCan't open %s.\n\n",argv[2]);
	      exit(0);
	    }
	  case 2:
	    if ( (mapin1 = fopen(argv[1],"rb")) == NULL ) {
	      printf("\nCan't open %s.\n\n",argv[1]);
	      exit(0);
	    }
	  break;
	  default:
	  printf("\n Usage: rsccmap <input ccp4 map 1> <input ccp4 map 2> <structure .xyz>\n\n");
	  exit(0);
	}
  
  /*
   * Initialize maps:
   */

  if ((map1 = linitmap()) == NULL) {
    perror("Couldn't initialize map.\n\n");
    exit(0);
  }
  
  if ((map2 = linitmap()) == NULL) {
    perror("Couldn't initialize map.\n\n");
    exit(0);
  }
  /*
   * Read in maps:
   */

  map1->infile = mapin1;
  if (lreadmap(map1) != 0) {
    perror("Couldn't read map.\n\n");
    exit(0);
  }

  map2->infile = mapin2;
  if (lreadmap(map2) != 0) {
    perror("Couldn't read map.\n\n");
    exit(0);
  }


  // Read xyz coords file

  int natoms;
  char buf[1024];

  fscanf(xyzin,"%d",&natoms); // read number of atoms
  fgets(buf,1024,xyzin); // skip a line
  xyzlist = (struct xyzcoords *)malloc(natoms*sizeof(struct xyzcoords));

  for (i=0;i<natoms;i++) {
    fscanf(xyzin,"%c %g %g %g",buf,&xyzlist[i].x,&xyzlist[i].y,&xyzlist[i].z);
  }

  map1->xyzlist = xyzlist;
  map1->natoms = natoms;
  map1->arad = arad;

  // Calculate correlations:

  lrsccmap(map1,map2);

  // Print results

  printf("CC_fore: %11.9f CC_back: %11.9f RSR_fore: %11.9f RSR_back: %11.9f\n",map1->cc_fore,map1->cc_back,map1->rsr_fore,map1->rsr_back);


CloseShop:
  
  /*
   * Free allocated memory:
   */

  //  lfreelt(lat);

  /*
   * Close files:
   */
  
  fclose(mapin1);
  fclose(mapin2);
  fclose(xyzin);
}

