/* LREADVTK.C - Read a lattice from a .vtk file.

   Author: Mike Wall
   Date: 11/19/12
   Version: 1.

*/

#include<mwmask.h>

int lreadvtk(LAT3D *lat)
{

  int
    return_value = 0;
  
  int 
    len_inl = 10000;

  char
    *token, *inl;

/*
 * Read the header:
 */

  int header_done = 0;

  inl = (char *)malloc(len_inl);

  // Skip the first two lines

  if (fgets(inl,len_inl-1,lat->infile) == NULL) {
    perror("Couldn't read line from input file\n");
    exit(1);
  }
  if (fgets(inl,len_inl-1,lat->infile) == NULL) {
    perror("Couldn't read line from input file\n");
    exit(1);
  }

  // Parse the header, stopping when the LOOKUP_TABLE line is encountered

  printf("Reading the vtk file header...\n");

  int found_token = 1;
  while (found_token == 1) {
    found_token = 0;
    if (fgets(inl,len_inl-1,lat->infile) != NULL) {      
      if ((token = strtok(inl," \n")) == NULL) {
	printf("Null...\n");
	token = inl;
      } else {
	printf("%s\n",token);
	if (strstr(token,"DIMENSIONS")!=NULL) {
	  found_token = 1;
	  lat->xvoxels = atoi(strtok(NULL," "));
	  lat->yvoxels = atoi(strtok(NULL," "));
	  lat->zvoxels = atoi(strtok(NULL," "));
	}
	if (strstr(token,"SPACING")!=NULL) {
	  found_token = 1;
	  lat->xscale = atof(strtok(NULL," "));
	  lat->yscale = atof(strtok(NULL," "));
	  lat->zscale = atof(strtok(NULL," "));
	}
	if (strstr(token,"ORIGIN")!=NULL) {
	  found_token = 1;
	  lat->xbound.min = atof(strtok(NULL," "));
	  lat->ybound.min = atof(strtok(NULL," "));
	  lat->zbound.min = atof(strtok(NULL," "));
	}
	if (strstr(token,"POINT_DATA")!=NULL) {
	  found_token = 1;
	  lat->lattice_length = atoi(strtok(NULL," "));
	}
	if (strstr(token,"SCALARS")!=NULL) {
	  found_token = 1;
	}
	if (strstr(token,"LOOKUP_TABLE")!=NULL) {
	  found_token = 1;
	}
	if (strstr(token,"ASCII")!=NULL) {
	  found_token = 1;
	}
	if (strstr(token,"DATASET")!=NULL) {
	  found_token = 1;
	}
      }
    }
  }

  printf("...done\n");

  if (lat->lattice_length != lat->xvoxels*lat->yvoxels*lat->zvoxels) {
    perror("POINT_DATA isn't equal to the product of the dimensions\n");
    exit(1);
  }

  lat->xbound.max = lat->xbound.min + lat->xvoxels*lat->xscale;
  lat->ybound.max = lat->ybound.min + lat->yvoxels*lat->yscale;
  lat->zbound.max = lat->zbound.min + lat->zvoxels*lat->zscale;

  lat->origin.i = (IJKCOORDS_DATA)(-lat->xbound.min*lat->xscale+.49);
  lat->origin.j = (IJKCOORDS_DATA)(-lat->ybound.min*lat->yscale+.49);
  lat->origin.k = (IJKCOORDS_DATA)(-lat->zbound.min*lat->zscale+.49);

  int index = 0,ct=0;
  int i,j,k;

  printf("%d %d %d\n",lat->xvoxels,lat->yvoxels,lat->zvoxels);

  printf("Reading the vtk file data...\n");

  for (k=0;k<lat->zvoxels;k++) {
    for (j=0;j<lat->yvoxels;j++) {
      for (i=0;i<lat->xvoxels;i++) {
	found_token = 0;
	if ((token = strtok(NULL," \n")) == NULL) {
	  while (found_token == 0) {
	    if (fgets(inl,len_inl-1,lat->infile) == NULL) {
	      perror("Failed to complete reading of .vtk file\n");
	      exit(1);
	    }
	    if ((token = strtok(inl," \n")) != NULL) {
	      found_token = 1;
	    }
	  }
	} else {
	  found_token = 1;
	}
	lat->lattice[index] = atof(token);
	index++;
      }
    }
  }
  
  printf("...done\n");

  if (index != lat->lattice_length) {
    printf("/nCouldn't write all of the lattice to output file.\n\n");
    return_value = 1;
    goto CloseShop;
  }

  CloseShop:
  free(inl);
  return(return_value);
}



