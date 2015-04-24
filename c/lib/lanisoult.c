/* LNORMLT.C - Generate a lattice with a normal distribution centered at the origin.
   
   Author: Mike Wall
   Date: 2/28/95; 4/21/15
   Version: 1.
   
   */

#include<mwmask.h>

int lanisoult(LAT3D *lat)
{
  size_t
    origin_index,
    lat_index;
  
  int
    i,j,k,
    return_value = 0;

  struct ijkcoords 
    rvec,
    index;

  float
    rscale,
    factor,
    rsqr,
    denom,
    sum;

  struct xyzcoords
    *origin_list,
    cell,
    rfloat,
    rdist,
    Uinv_times_rdist;

  struct xyzmatrix
    Uinv;

  cell.x = lat->xscale*(float)lat->xvoxels;
  cell.y = lat->yscale*(float)lat->yvoxels;
  cell.z = lat->zscale*(float)lat->zvoxels;
  
  int nn=1;
  
  int dim_ss;
  
  dim_ss = (2*nn+1)*(2*nn+1)*(2*nn+1);

  origin_list = (struct xyzcoords *)malloc(sizeof(struct xyzcoords)*dim_ss);

  origin_index = 0;
  for (i=-nn;i<=nn;i++) {
    for (j=-nn;j<=nn;j++) {
      for (k=-nn;k<=nn;k++) {
	origin_list[origin_index].x=cell.x*(float)i;
	origin_list[origin_index].y=cell.y*(float)j;
	origin_list[origin_index].z=cell.z*(float)k;
	origin_index++;
      }
    }
  }

  //  printf("LANISOULT: lat->anisoU.xx = %f\n",lat->anisoU.xx);

  Uinv = lmatinv(lat->anisoU);

  //  printf("LANISOULT: Uinv.xx = %f\n",Uinv.xx);

  //  factor = 1. / (sqrt(2*PI)*lat->width);
  //  rscale = (lat->xscale*lat->xscale + lat->yscale*lat->yscale +
  //		 lat->zscale*lat->zscale);
  lat_index = 0;
  for(index.k = 0; index.k < lat->zvoxels; index.k++) {
    for(index.j = 0; index.j < lat->yvoxels; index.j++) {
      for (index.i = 0; index.i < lat->xvoxels; index.i++) {
	rvec.i = index.i - lat->origin.i;
	rvec.j = index.j - lat->origin.j;
	rvec.k = index.k - lat->origin.k;
	rfloat.x = lat->xscale * rvec.i;
	rfloat.y = lat->yscale * rvec.j;
	rfloat.z = lat->zscale * rvec.k;
	lat->lattice[lat_index] = 0;
	for (origin_index=0;origin_index<dim_ss;origin_index++) {
	  rdist.x = rfloat.x - origin_list[origin_index].x;
	  rdist.y = rfloat.y - origin_list[origin_index].y;
	  rdist.z = rfloat.z - origin_list[origin_index].z;	  
	  Uinv_times_rdist = lmatvecmul(Uinv,rdist);
	  rsqr = ldotvec(rdist,Uinv_times_rdist);
	  lat->lattice[lat_index] += (LATTICE_DATA_TYPE)
	    expf(-rsqr/2.);
	}
	lat_index++;
      }
    }
  }
  lat_index = 0;
  sum = 0.0;
  for(index.k = 0; index.k < lat->zvoxels; index.k++) {
    for(index.j = 0; index.j < lat->yvoxels; index.j++) {
      for (index.i = 0; index.i < lat->xvoxels; index.i++) {
	sum += lat->lattice[lat_index];
	lat_index++;
      }
    }
  }
  lat_index = 0;
  for(index.k = 0; index.k < lat->zvoxels; index.k++) {
    for(index.j = 0; index.j < lat->yvoxels; index.j++) {
      for (index.i = 0; index.i < lat->xvoxels; index.i++) {
	lat->lattice[lat_index] /= sum;
	lat_index++;
      }
    }
  }

  CloseShop:
    free(origin_list);
  return(return_value);
}


