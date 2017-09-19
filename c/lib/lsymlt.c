/* LSYMLT.C - Symmetrize a lattice according to input line instructions.
   
   Author: Mike Wall
   Date: 2/28/95
         revised 5/10/2013 (add P222 group for PSII)
   Modified by Alex Wolff to generalize to all Laue classes. Added 6/16/2017 MEW
   Version: 1.
   
   */

#include<mwmask.h>

int lsymlt(LAT3D *lat)
{
  size_t
    max_op_count = 100,
    op_count,
    op_index,
    ct,
    li0;

  long
    *lat_index;
  
  int
    return_value = 0;

  struct ijkcoords 
    i0,
    *index,
    *rvec;

  LATTICE_DATA_TYPE
    *lattice;

  double
    lattice_tmp;

  /*
   * Allocate lattice:
   */

  lattice = (LATTICE_DATA_TYPE *)calloc(lat->lattice_length,
					sizeof(LATTICE_DATA_TYPE));
  lat_index = (long *)calloc(max_op_count, sizeof(size_t));
  index = (struct ijkcoords *)malloc(max_op_count*sizeof(struct
						     ijkcoords));
  rvec = (struct ijkcoords *)malloc(max_op_count*sizeof(struct
						     ijkcoords));
 


  if (!lattice || !lat_index || !index || !rvec) {
    sprintf(lat->error_msg,"\nLSYM1LT:  Couldn't allocate arrays.\n\n");
    return_value = 1;
    goto CloseShop;
  }

  lat->symvec = rvec;
  li0 = 0;
  for(i0.k = 0; i0.k < lat->zvoxels; i0.k++) {
    for(i0.j = 0; i0.j < lat->yvoxels; i0.j++) {
      for (i0.i = 0; i0.i < lat->xvoxels; i0.i++) {
	rvec[0] = lijksub(i0,lat->origin);
	switch(lat->symop_index) {
	case 0:
	  lLaue1(lat);
	  break;
	case 1:
	  lLaue2(lat);
	  break;
	case 2:
	  lLaue3(lat);
	  break;
	case 3:
	  lLaue4(lat);
	  break;
	case 4:
	  lLaue5(lat);
	  break;
  case 5:
    lLaue6(lat);
    break;
  case 6:
    lLaue7(lat);
    break;
  case 7:
    lLaue8(lat);
    break;
  case 8:
    lLaue9(lat);
    break;
  case 9:
    lLaue10(lat);
    break;
  case 10:
    lLaue11(lat);
    break;
	}
	op_count = lat->symop_count;
	lattice_tmp=0.0;
	ct = 0;
	for(op_index = 0;op_index < op_count; op_index++) {
	  index[op_index] = lijksum(rvec[op_index],lat->origin);
	  lat_index[op_index] = index[op_index].k*lat->xyvoxels +
	    index[op_index].j*lat->xvoxels + index[op_index].i;
	  if ((lat_index[op_index] >= 0) && 
	      (lat_index[op_index] < lat->lattice_length) &&
	      (lat->lattice[lat_index[op_index]] != lat->mask_tag)) {
	    //	    lattice[lat_index[0]] = ((float)ct*lattice[lat_index[0]] +
	    //				   lat->lattice[lat_index[op_index]])/
	    //				     (float)(ct + 1);	    
	    ct++;
	    lattice_tmp += lat->lattice[lat_index[op_index]];
/*	    if ((rvec[0].i==10 && rvec[0].j==10 && rvec[0].k==0) ||
		(rvec[0].i==-10 && rvec[0].j==10 && rvec[0].k==0) ) 
	      printf("%d,%f,%lf\n",ct,lat->lattice[lat_index[op_index]],lattice_tmp);*/
	  }
	}
	if (ct == 0) {
	  lattice[li0] = lat->mask_tag;
	} else {
	  lattice[li0] = (float)(lattice_tmp/(double)ct);
	}
	li0++;
      }
    }
  }

  /*
   * Copy lattice to input lattice:
   */

  for(lat_index[0] = 0; lat_index[0] < lat->lattice_length; lat_index[0]++)
    {
      lat->lattice[lat_index[0]] = lattice[lat_index[0]];
    }
  CloseShop:
  free((LATTICE_DATA_TYPE *)lattice);
  free((size_t *)lat_index);
  free((struct ijkcoords *)rvec);
  free((struct ijkcoords *)index);
  return(return_value);
}








