/* CORRANLT.C - Calculate correlation of the anisotropic component of two lattices.
   
   Author: Mike Wall
   Date: 9/27/2016
   Version: 1.
   
   Usage:
   		"corranlt <input lattice 1> <input lattice 2> <threshold>"

   */

#include<mwmask.h>

  float rsqrFromIndex(LAT3D *lat) 
  {
    struct ijkcoords rvec;
    struct xyzcoords r1,r2,r3,rfloat;
    XYZCOORDS_DATA rsqr;

    rvec.i = lat->index.i - lat->origin.i;
    rvec.j = lat->index.j - lat->origin.j;
    rvec.k = lat->index.k - lat->origin.k;
    r1 = lmulscvec((XYZCOORDS_DATA)rvec.i,lat->astar);
    r2 = lmulscvec((XYZCOORDS_DATA)rvec.j,lat->bstar);
    r3 = lmulscvec((XYZCOORDS_DATA)rvec.k,lat->cstar);
    rfloat = laddvec(laddvec(r1,r2),r3);
    rsqr = (rfloat.x*rfloat.x + rfloat.y*rfloat.y + 
		       rfloat.z*rfloat.z);
    return rsqr;
  }

void lanisolt(LAT3D *lat)
{

  size_t *ct,r,lat_index;

  float rf,rscale;

  struct ijkcoords index;

  ct = (size_t *)calloc(MAX_RFILE_LENGTH,sizeof(size_t));
  if (!ct) {
    sprintf(lat->error_msg,"\nLLMLT: Couldn't allocate counting "
	    "array ct.\n\n");
    //    return_value = 1;
    //    goto CloseShop;
  }

  lat->rfile_length = 0;
  rscale = (lat->xscale*lat->xscale + lat->yscale*lat->yscale +
		 lat->zscale*lat->zscale);
  // calculate isotropic component
  lat_index = 0;
  for(index.k = 0; index.k < lat->zvoxels; index.k++) {
    for(index.j = 0; index.j < lat->yvoxels; index.j++) {
      for (index.i = 0; index.i < lat->xvoxels; index.i++) {
	lat->index = index;
	
	r = (size_t)(sqrtf(rsqrFromIndex(lat) / rscale)+.5);
	if (r > lat->rfile_length) lat->rfile_length = r;
	if (lat->lattice[lat_index] != lat->mask_tag) {
	  //	  if (lat->lattice[index]<0) printf("%d,%f\n",(int)index,lat->lattice[index]);
	  if (ct[r] == 0) {
	    lat->rfile[r] = lat->lattice[lat_index];
	    ct[r] = 1;
	  } else {
	    lat->rfile[r] = ((float)ct[r]*lat->rfile[r] +
			     lat->lattice[lat_index])/(float)(ct[r]+1);
	    ct[r]++;
	  }
	}
	lat_index++;
      }
    }
  }

  // subtract isotropic component

  size_t maxr=30;
  lat_index = 0;
  for(index.k = 0; index.k < lat->zvoxels; index.k++) {
    for(index.j = 0; index.j < lat->yvoxels; index.j++) {
      for (index.i = 0; index.i < lat->xvoxels; index.i++) {
	lat->index = index;
	rf = sqrtf(rsqrFromIndex(lat) / rscale);
	r = (size_t)rf;
	if ((r < lat->rfile_length) && 
	    (lat->lattice[lat_index] != lat->mask_tag)) {
	  //    lat->lattice[index] -= lat->rfile[r];
	  if (r <= maxr) {
	    lat->lattice[lat_index] -= (lat->rfile[r] + 
				  (rf - (float)r)*(lat->rfile[r+1] - 
						   lat->rfile[r]));
	  } else {
	    lat->lattice[lat_index] = lat->mask_tag;
	  }
	}

	lat_index++;
      }
    }
  }
  free((size_t *)ct);
}

int main(int argc, char *argv[])
{
  FILE
    *latticein1,
    *latticein2;
  
  char
    error_msg[LINESIZE];
  
  size_t
    i,
    j,
    k,
    num_wrote;
  
  LAT3D 
    *lat1,
    *lat2;
  
  RFILE_DATA_TYPE *rfile;

  LATTICE_DATA_TYPE threshold = -32768.f;

  struct ijkcoords
    origin;

  /*
   * Set input line defaults:
   */
  
  latticein1 = stdin;
  latticein2 = stdin;
  
  /*
   * Read information from input line:
   */
  switch(argc) {
    case 4:
      threshold = atof(argv[3]);
    case 3:
    if (strcmp(argv[2],"-") == 0) {
      latticein2 = stdin;
    }
    else {
      if ( (latticein2 = fopen(argv[2],"rb")) == NULL ) {
	printf("\nCan't open %s.\n\n",argv[2]);
	exit(0);
      }
    }
    case 2:
    if (strcmp(argv[1],"-") == 0) {
      latticein1 = stdin;
    }
    else {
      if ( (latticein1 = fopen(argv[1],"rb")) == NULL ) {
	printf("\nCan't open %s.\n\n",argv[1]);
	exit(0);
      }
    }
    break;
    default:
    printf("\n Usage: corranlt <input lattice 1> <input lattice 2> "
	   "<threshold>\n\n");
    exit(0);
  }
  
  /*
   * Initialize lattice:
   */
  
  if (((lat1 = linitlt()) == NULL) || ((lat2 = linitlt()) == NULL)) {
    perror("Couldn't initialize lattices.\n\n");
    exit(0);
  }

  lat1->threshold = threshold;
  
  /*
   * Read in lattices:
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

  // Calculate anisotropic component of lattices

  lat1->cell_str = "57.154,57.965,69.092,90.00,112.77,90.00";
  lparsecelllt(lat1);
  lat2->cell_str = "57.154,57.965,69.092,90.00,112.77,90.00";
  lparsecelllt(lat2);
  lanisolt(lat1);
  lanisolt(lat2);

  // Calculate correlation between the two

  printf("Calculating correlation\n");

  float corr;

  corr = lcorrlt(lat1,lat2);
   
  printf("%g\n",corr);

  //  lccrlt(lat1,lat2);

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
}


