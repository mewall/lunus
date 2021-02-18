/* MAP2LAT.C - Generate a 3D lattice from an input voxel map.
   
   Author: Mike Wall
   Date: 3/28/93
   Version: 1.
   
   Usage:
   "map2lat <input file> <output file> <inner radius> 
   <outer radius> <x-origin> <y-origin> <z-origin> <minrange>"
   
   Input is a list of voxels (float x, float y,
   float z, float value).  Output 
   is a 3D lattice representation of the voxel map.
   
*/

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *infile,
    *outfile;
  
  char
    error_msg[LINESIZE];
  
  struct voxel
    *voxel_data;
  
  size_t
    index,
    i,
    j,
    k,
    num_read,
    num_wrote;
  
  LAT3D 
    *lat;
  
  size_t
    *ct;
  
  int
    hh,
    kk,
    ll;

  float 
    minrange,
    q_squared,
    inner_radius,
    inner_radius_sq,
    outer_radius,
    outer_radius_sq;
  
  struct xyzcoords
    dist,
    pos;

  struct ijkcoords
    origin;

  /*
   * Set input line defaults:
   */
  
  infile = stdin;
  outfile = stdout;
  inner_radius= DEFAULT_INNER_RADIUS_LT;
  outer_radius = DEFAULT_OUTER_RADIUS_LT;
  inner_radius_sq = inner_radius*inner_radius;
  outer_radius_sq = DEFAULT_BOUND_MAX*DEFAULT_BOUND_MAX;
  minrange = DEFAULT_MINRANGE;
  origin.i = origin.j = origin.k = DEFAULT_LATTICE_ORIGIN;
  
  /*
   * Read information from input line:
   */
  switch(argc) {
    case 9:
    minrange = atof(argv[8]);
    case 8:
    origin.k = atol(argv[7]);
    case 7:
    origin.j = atol(argv[6]);
    case 6:
    origin.i = atol(argv[5]);
    case 5:
    outer_radius = atof(argv[4]);
    outer_radius_sq = outer_radius*outer_radius;
    case 4:
    inner_radius = atof(argv[3]);
    inner_radius_sq = inner_radius*inner_radius;
    case 3:
    if (strcmp(argv[2],"-") == 0) {
      outfile = stdout;
    }
    else {
      if ((outfile = fopen(argv[2],"wb")) == NULL) {
	printf("\nCan't open %s.\n\n",argv[2]);
	exit(0);
      }
    }
    case 2:
    if (strcmp(argv[1],"-") == 0) {
      infile = stdin;
    }
    else {
      if ( (infile = fopen(argv[1],"rb")) == NULL ) {
	printf("\nCan't open %s.\n\n",argv[1]);
	exit(0);
      }
    }
    break;
    default:
    printf("\n Usage: map2lat <input file> "
	   "<output file> <inner radius> <outer radius> "
	   "<x-origin> <y-origin> <z-origin> <minrange>\n\n");
    exit(0);
  }
  
  /*
   * Allocate memory:
   */
  
  voxel_data = (struct voxel *)malloc(sizeof(struct voxel));
  if (!voxel_data) {
    printf("\n***Unable to allocate all memory.\n");
    goto CloseShop;
  }

  /*
   * Initialize lattice:
   */

  if ((lat = linitlt()) == NULL) {
    perror("Couldn't initialize lattice.n\n");
    exit(0);
  }
  
  /*
   * Set main defaults:
   */
  
  lat->map3D = voxel_data;
  lat->minrange.x = lat->minrange.y = lat->minrange.z = minrange;
  lat->origin = origin;
  lat->xbound.min = - lat->origin.i*lat->xscale;               
  lat->xbound.max = (lat->xvoxels - lat->origin.i - 1)*lat->xscale;
  lat->ybound.min = - lat->origin.j*lat->yscale; 
  lat->ybound.max = (lat->yvoxels - lat->origin.j - 1)*lat->yscale;
  lat->zbound.min = - lat->origin.k*lat->zscale; 
  lat->zbound.max = (lat->zvoxels - lat->origin.k - 1)*lat->zscale;
  
  /*
   * Allocate ct:
   */
  
  ct = (size_t *)calloc(lat->lattice_length,sizeof(size_t));
  if (!ct) {
    printf("\nNot enough room to allocate counting table (ct).\n\n");
    exit(0);
  }
  
  /*
   * Step through input file and update current bounds:
   */
  
  while ((num_read = fread(voxel_data, sizeof(struct voxel), 1, infile)) == 1){
    q_squared = (voxel_data->pos.x*voxel_data->pos.x +
		 voxel_data->pos.y*voxel_data->pos.y +
		 voxel_data->pos.z*voxel_data->pos.z);
    if ((inner_radius_sq < q_squared) && (outer_radius_sq >
					  q_squared)) {
	if (voxel_data->pos.x < 0) {
	  hh = (int)(voxel_data->pos.x - .5);
	} else {
	  hh = (int)(voxel_data->pos.x + .5);
	}
	if (voxel_data->pos.y < 0) {
	  kk = (int)(voxel_data->pos.y - .5);
	} else {
	  kk = (int)(voxel_data->pos.y + .5);
	}
	if (voxel_data->pos.z < 0) {
	  ll = (int)(voxel_data->pos.z - .5);
	} else {
	  ll = (int)(voxel_data->pos.z + .5);
	}
	i = (size_t) (voxel_data->pos.x + lat->origin.i + .5); 
	j = (size_t) (voxel_data->pos.y + lat->origin.j + .5); 
	k = (size_t) (voxel_data->pos.z + lat->origin.k + .5); 
      dist.x = (float)fabsf(voxel_data->pos.x - (float)hh);
      dist.y = (float)fabsf(voxel_data->pos.y - (float)kk);
      dist.z = (float)fabsf(voxel_data->pos.z - (float)ll);
      if ((voxel_data->value != lat->mask_tag) && 
	  (voxel_data->value != 0) &&
	  (dist.x > lat->minrange.x) &&
	  (dist.y > lat->minrange.y) &&
	  (dist.z > lat->minrange.z)) {
	index = k*lat->xyvoxels + j*lat->xvoxels + i;
	if (index >= lat->lattice_length) {
	  printf("\nTried to index lattice outside of range.\n");
	  goto CloseShop;
	}
	if (ct[index == 0]) {
	  lat->lattice[index] = (LATTICE_DATA_TYPE)voxel_data->value;
	  ct[index]++;
	} else {
	  lat->lattice[index] =
	    (LATTICE_DATA_TYPE)(((float)ct[index]*lat->lattice[index] + 
				 voxel_data->value)
				/(float)(ct[index]+1.));
	  //printf("%d %d %d : %f %f %f : %f\n",i,j,k,voxel_data->pos.x,voxel_data->pos.y,voxel_data->pos.z,voxel_data->value);/***/
	  ct[index]++;
	}
      }
    }
  }
  
  /*
   * Write lattice to output file:
   */

  lat->outfile = outfile;
  if (lwritelt(lat) != 0) {
    perror("Couldn't write lattice.\n\n");
    exit(0);
  }
  
  CloseShop:
  
  /*
   * Free allocated memory:
   */

  lfreelt(lat);
  free((struct voxel *)voxel_data);
  free((size_t *)ct);
  
  /*
   * Close files:
   */
  
  fclose(infile);
}

     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
     
