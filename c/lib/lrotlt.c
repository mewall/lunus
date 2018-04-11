/* LROTLT.C - Rotate a lattice about specified axis.
   
   Author: Mike Wall
   Date: 7/17/2014
   Version: 1.
   
   */

#include<mwmask.h>

int lrotlt(LAT3D *lat)
{
  size_t
    i1,i2,index,
    *ct,
    *lat_index;
  
  int
    return_value = 0;

  float cosangle,sinangle;

  struct ijkcoords 
    i,j,k,
    index1,index2,
    rvec1,rvec2;
  
  struct xyzcoords
    s1,s2;

  LATTICE_DATA_TYPE
    *lattice;

  /*
   * Allocate lattice:
   */

  lattice = (LATTICE_DATA_TYPE *)calloc(lat->lattice_length,
					sizeof(LATTICE_DATA_TYPE));
  ct = (size_t *)calloc(lat->lattice_length,
					sizeof(size_t));
  if (!lattice) {
    sprintf(lat->error_msg,"\nLROTLT:  Couldn't allocate lattice.\n\n");
    return_value = 1;
    goto CloseShop;
  }

  for(index1.k = 0; index1.k < lat->zvoxels; index1.k++) {
    for(index1.j = 0; index1.j < lat->yvoxels; index1.j++) {
      for (index1.i = 0; index1.i < lat->xvoxels; index1.i++) {
	index = index1.k*lat->xyvoxels+index1.j*lat->xvoxels+index1.i;
	lattice[index]=lat->mask_tag;
      }
    }
  }
  cosangle = cosf(lat->angle*PI/180.);
  sinangle = sinf(lat->angle*PI/180.);
  for(index1.k = 0; index1.k < lat->zvoxels; index1.k++) {
    for(index1.j = 0; index1.j < lat->yvoxels; index1.j++) {
      for (index1.i = 0; index1.i < lat->xvoxels; index1.i++) {
	i1 = index1.k*lat->xyvoxels+index1.j*lat->xvoxels+index1.i;
	if (lat->lattice[i1] != lat->mask_tag) {
	  //	  lat->index = lijksub(index1,lat->origin);
	  lat->index = index1;
	  //	printf("Before: %d %d %d\n",lat->index.i,lat->index.j,lat->index.k);
	  s1 = lsFromIndex(lat);
	  if (lat->axis == 3) {
	    s2 = lrotvecz(s1,cosangle,sinangle);
	  } else if (lat->axis == 2) {
	    s2 = lrotvecy(s1,cosangle,sinangle);
	  } else {
	    perror("LROTLT: axis number not recognized");
	    goto CloseShop;
	  }
	  lat->sv = s2;
	  index2 = lindexFromS(lat);
	  //	printf("After: %d %d %d\n",lat->index.i,lat->index.j,lat->index.k);
	  //	  index2 = lijksum(lat->index,lat->origin);
	  if (index2.i >= 0 && index2.i < lat->xvoxels && index2.j >= 0 && index2.j < lat->yvoxels && 
	      index2.k >= 0 && index2.k < lat->zvoxels) {
	    i2 = index2.k*lat->xyvoxels+index2.j*lat->xvoxels+index2.i;
	    if (ct[i2] == 0) {
	      lattice[i2] = lat->lattice[i1];
	      ct[i2]++;
	    } else {
	      lattice[i2] =
		((LATTICE_DATA_TYPE)ct[i2]*lattice[i2]+lat->lattice[i1])/(LATTICE_DATA_TYPE)(ct[i2]+1);
	      ct[i2]++;
	    }
	  }
	}	
      }
    }
  }
  
  /*
   * Copy lattice to input lattice:
   */
  
  for(index = 0; index < lat->lattice_length; index++)
    {
      lat->lattice[index] = lattice[index];
    }
 CloseShop:
  free((LATTICE_DATA_TYPE *)lattice);
  return(return_value);
}








