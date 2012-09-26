/* MAPBOUND.C - Output the bounds in x,y,z,I for a list of input voxels (map).
   
   Author: Mike Wall
   Date: 3/20/93
   Version: 1.
   
   Usage:
   		"mapbound <input file>"

   Input is a list of voxels (float x, float y, float z, float value).  Output
	is (min(x),max(x),min(y),max(y),min(z),max(z),min(value),max(value).

   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*infile;

  char
    error_msg[LINESIZE];

  struct voxel
	*voxel_data;

  size_t
	num_read;

  LAT3D 
	*lat;

/*
 * Set input line defaults:
 */
	
	infile = stdin;

/*
 * Read information from input line:
 */
	switch(argc) {
		case 2:
			if (strcmp(argv[1], "-") == 0) {
				infile = stdin;
			}
			else {
			 if ( (infile = fopen(argv[1],"r")) == NULL ) {
				printf("Can't open %s.",argv[1]);
				exit(0);
			 }
			}
		case 1:
			break;
		default:
			printf("\n Usage: genmap <input file> \n\n");
			exit(0);
	}
  
/*
 * Allocate memory:
 */

  voxel_data = (struct voxel *)malloc(sizeof(struct voxel));
  lat = (LAT3D *)malloc(sizeof(LAT3D));
  if (!lat) {
    printf("\n***Unable to allocate all memory.\n");
    goto CloseShop;
  }
  
/*
 * Set main defaults:
 */

	lat->map3D = voxel_data;
	lat->mask_tag = PUNCH_TAG;

/*
 * Step through input file and update current bounds:
 */

  while ((num_read= fread(voxel_data, sizeof(struct voxel), 1, infile)) == 1) {
	lupdbd(lat);
  }
  
/*
 * Print bounds to standard output:
 */

  printf("xb(min,max) = %g %g\n"
	 "yb(min,max) = %g %g\n"
	 "zb(min,max) = %g %g\n"
	 "valuebound(min,max) = %g %g\n\n", lat->xbound.min, lat->xbound.max, 
				lat->ybound.min, lat->ybound.max, 
				lat->zbound.min, lat->zbound.max, 
				lat->valuebound.min, lat->valuebound.max);

CloseShop:
  
  /*
   * Free allocated memory:
   */

  free((LAT3D *)lat);
  free((struct voxel *)voxel_data);

  /*
   * Close files:
   */
  
  fclose(infile);
}

