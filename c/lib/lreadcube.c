/* LREADCUBE.C - Read a Gaussian cube file into a CCP4 map.

   Author: Mike Wall
   Date: 2/25/14
   Version: 1.

*/

#include<mwmask.h>

int lreadcube(CCP4MAP *map)
{

  int
    return_value = 0;
  
  int 
    len_inl = 100000;

  char
    *token, *inl;

  char
    *fgets_status;

  size_t index;

/*
 * Read the header:
 */

  int header_done = 0;

  inl = (char *)malloc(len_inl);

  // Skip the first two lines

  if (fgets(inl,len_inl-1,map->infile) == NULL) {
    perror("Couldn't read line from input file\n");
    exit(1);
  }
  if (fgets(inl,len_inl-1,map->infile) == NULL) {
    perror("Couldn't read line from input file\n");
    exit(1);
  }

  // Parse the header, stopping when the LOOKUP_TABLE line is encountered

  int natoms;

  printf("Reading the cube file header...\n");
  // Line with number of atoms
  if ((fgets_status=fgets(inl,len_inl-1,map->infile)) != NULL) {
    if ((token = strtok(inl," \n")) != NULL) {
      natoms = atoi(token);      
      printf("natoms = %d\n",natoms);
    }
  }
  struct ijkcoords mapsz;
  // Line with a axis info
  struct xyzcoords da;
  if ((fgets_status=fgets(inl,len_inl-1,map->infile)) != NULL) {
    if ((token = strtok(inl," \n")) != NULL) {
      mapsz.i = atoi(token);
      da.x =  atof(strtok(NULL," "));
      da.y =  atof(strtok(NULL," "));
      da.z =  atof(strtok(NULL," "));
      da = lmulscvec(BOHR,da);
      printf("mapsz.i = %d, da = (%f %f %f)\n",mapsz.i,da.x,da.y,da.z);
    }
  }
  // Line with b axis info
  struct xyzcoords db;
  if ((fgets_status=fgets(inl,len_inl-1,map->infile)) != NULL) {
    if ((token = strtok(inl," \n")) != NULL) {
      mapsz.j = atoi(token);
      db.x =  atof(strtok(NULL," "));
      db.y =  atof(strtok(NULL," "));
      db.z =  atof(strtok(NULL," "));
      db = lmulscvec(BOHR,db);
    }
  }
  // Line with c axis info
  struct xyzcoords dc;
  if ((fgets_status=fgets(inl,len_inl-1,map->infile)) != NULL) {
    if ((token = strtok(inl," \n")) != NULL) {
      mapsz.k = atoi(token);
      dc.x =  atof(strtok(NULL," \n"));
      dc.y =  atof(strtok(NULL," \n"));
      dc.z =  atof(strtok(NULL," \n"));
      dc = lmulscvec(BOHR,dc);
    }
  }
  // Next read and discard atomic structure info
  for (index = 0;index<natoms;index++) {
      if ((fgets_status=fgets(inl,len_inl-1,map->infile)) == NULL) {
	perror("Can't read atomic structure info\n");
	return_value = 1;
	goto CloseShop;
      }
  }
  // Now read the data

  map->map_length = mapsz.i*mapsz.j*mapsz.k;
  map->data_buf = (void *)realloc(map->data_buf,map->map_length*
					     sizeof(MAP_DATA_TYPE));
  map->data = (MAP_DATA_TYPE *)map->data_buf;
  index = 0;
  while (index < map->map_length) {
    if ((fgets_status=fgets(inl,len_inl-1,map->infile)) != NULL) {      
      if ((token = strtok(inl," \n")) == NULL) {
	printf("Can't read a data line\n");
	return_value = 2;
	goto CloseShop;
      } else {
	if (strlen(token) != 0) {
	  map->data[index] = atof(token);
	  index++;
	}
	while ((token = strtok(NULL," \n")) != NULL) {
	  if (strlen(token) != 0) {
	    map->data[index] = atof(token);
	    index++;
	  }
	}
      }
    } else {
	printf("Can't read a data line\n");
	return_value = 2;
	goto CloseShop;
    }
  }
  //  printf("...done\n");

  float amin = map->data[0], amax = map->data[0], amean = 0.0, arms = 0.9;

  for (index = 0;index < map->map_length;index++) {
    if (map->data[index] < amin) {
      amin = map->data[index];
    }
    if (map->data[index] > amax) {
      amax = map->data[index];
    }
    amean += map->data[index];
  }
  amean /= (float)map->map_length;
  for (index = 0;index < map->map_length;index++) {
    arms += (map->data[index]-amean)*(map->data[index]-amean);
  }
  arms = sqrtf(arms/(float)map->map_length);
  printf("amin = %f, amax = %f, amean = %f, arms = %f\n",amin,amax,amean,arms);

  // Populate the rest of the info in the map header

  float dalen,dblen,dclen;

  map->nc = mapsz.k; // 1
  map->nr = mapsz.j; // 2
  map->ns = mapsz.i; // 3
  map->mode = 2; // 4
  map->ncstart = 0; // 5
  map->nrstart = 0; // 6
  map->nsstart = 0; // 7
  map->nx = map->ns; // 8
  map->ny = map->nr; // 9
  map->nz = map->nc; // 10
  dalen = sqrtf(ldotvec(da,da));
  dblen = sqrtf(ldotvec(db,db));
  dclen = sqrtf(ldotvec(dc,dc));
  map->xlen = dalen*(float)map->nx; // 11
  map->ylen = dblen*(float)map->ny; // 12
  map->zlen = dclen*(float)map->nz; // 13
  map->alpha = acosf(ldotvec(db,dc)/dblen/dclen)*180./PI; // 14
  if (fabs(map->alpha-90.)<0.01) {map->alpha = 90.;}
  map->beta = acosf(ldotvec(da,dc)/dalen/dclen)*180./PI; // 15
  if (fabs(map->beta-90.)<0.01) {map->beta = 90.;}
  map->gamma = acosf(ldotvec(da,db)/dalen/dblen)*180./PI; // 16
  if (fabs(map->gamma-90.)<0.01) {map->gamma = 90.;}
  map->mapc = 3; // 17
  map->mapr = 2; // 18
  map->maps = 1; // 19
  map->amin = amin; // 20
  map->amax = amax; // 21
  map->amean = amean; // 22
  // Space group 1
  map->ispg = 1;
  map->nsymbt = 80;
  // Space group 113
  //  map->ispg = 113; 
  //  map->nsymbt = 640;
  // Generate symmetry info for P21 map, generalize this later
  //  map->ispg = 4; // 23
  //  map->nsymbt = 160; // 24. Two lines of symmetry info
  map->machst = DEFAULT_MACHST;
  map->symrec_buf = (void *)realloc(map->symrec_buf,(map->nsymbt+1)*sizeof(void));
  map->symrec = (char *)map->symrec_buf;
  // Space group 1
  sprintf(&map->symrec[0],"X,Y,Z");
  // Space group 113
  //  sprintf(&map->symrec[0],"X,Y,Z");
  //  sprintf(&map->symrec[80],"Y,-X,-Z");
  //  sprintf(&map->symrec[160],"-X,-Y,Z");
  //  sprintf(&map->symrec[240],"-Y,X,-Z");
  //  sprintf(&map->symrec[320],"X+1/2,-Y+1/2,-Z");
  //  sprintf(&map->symrec[400],"-Y+1/2,-X+1/2,Z");
  //  sprintf(&map->symrec[480],"-X+1/2,Y+1/2,-Z");
  //  sprintf(&map->symrec[560],"Y+1/2,X+1/2,Z");
  // P21 symmetry
  //sprintf(&map->symrec[0],"X,Y,Z");
  //sprintf(&map->symrec[80],"-X,1/2+Y,-Z");
  map->arms = arms; // 54

  CloseShop:
  free(inl);
  return(return_value);
}



