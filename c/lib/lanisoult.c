/* LNORMLT.C - Generate a lattice with a normal distribution centered at the origin.
   
   Author: Mike Wall
   Date: 2/28/95; 4/21/15
   Version: 1.
   
   */

#include<mwmask.h>
#ifdef USE_OPENMP
#include<omp.h>
#endif

int lanisoult(LAT3D *lat)
{
  
  int
    i,j,k,origin_index,
    return_value = 0;

  struct ijkcoords 
    index;

  float
    rscale,
    factor,
    denom,
    sum;

  struct xyzcoords
    *origin_list,
    cell;

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
#ifdef USE_OPENMP
#pragma omp parallel for
#endif
  for(k = 0; k < lat->zvoxels; k++) {
    size_t lat_index;
    lat_index = k*lat->xyvoxels;
    int j; 
    for(j = 0; j < lat->yvoxels; j++) {
      int i;
      for (i = 0; i < lat->xvoxels; i++) {
	struct ijkcoords rvec;
	rvec.i = i - lat->origin.i;
	rvec.j = j - lat->origin.j;
	rvec.k = k - lat->origin.k;
	struct xyzcoords rfloat;
	rfloat.x = lat->xscale * rvec.i;
	rfloat.y = lat->yscale * rvec.j;
	rfloat.z = lat->zscale * rvec.k;
	lat->lattice[lat_index] = 0;
	size_t origin_index;
	for (origin_index=0;origin_index<dim_ss;origin_index++) {
	  struct xyzcoords rdist, Uinv_times_rdist;
	  rdist.x = rfloat.x - origin_list[origin_index].x;
	  rdist.y = rfloat.y - origin_list[origin_index].y;
	  rdist.z = rfloat.z - origin_list[origin_index].z;	  
	  Uinv_times_rdist = lmatvecmul(Uinv,rdist);
	  float rsqr;
	  rsqr = ldotvec(rdist,Uinv_times_rdist);
	  lat->lattice[lat_index] += (LATTICE_DATA_TYPE)
	    expf(-rsqr/2.);
	}
	lat_index++;
      }
    }
  }
  sum = 0.0;
  for(index.k = 0; index.k < lat->zvoxels; index.k++) {
    size_t lat_index;
    lat_index = index.k*lat->xyvoxels;
    for(index.j = 0; index.j < lat->yvoxels; index.j++) {
      for (index.i = 0; index.i < lat->xvoxels; index.i++) {
	sum += lat->lattice[lat_index];
	lat_index++;
      }
    }
  }
  for(index.k = 0; index.k < lat->zvoxels; index.k++) {
    size_t lat_index;
    lat_index = index.k*lat->xyvoxels;
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


