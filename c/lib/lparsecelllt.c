/* LPARSECELLLT.C - Parse the unit cell string and place results in the cell struct, and in the astar, bstar, and cstar structs.

   
   Author: Mike Wall
   Date: 9/20/15
   Version: 1.
   
   */

#include<mwmask.h>

int lparsecelllt(LAT3D *lat)
{

  char cell_str[256];

  char *token;

  size_t
    *ct,
    i,
    j,
    k,
    r,
    index = 0;

  int
    return_value = 0;
  
  struct ijkcoords rvec;
  
  float
    rscale;

  struct xyzcoords
    rfloat;

  strcpy(cell_str,lat->cell_str);
  i=0;
  token=strtok(cell_str,",");
  while (token!=NULL) {
    ((float *)&lat->cell)[i]=atof(token);
    token = strtok(NULL,",");
    i++;
  }
  lat->a.x = lat->cell.a;
  lat->a.y = 0.0;
  lat->a.z = 0.0;
  lat->b.x = lat->cell.b*cosf(lat->cell.gamma*PI/180.);
  if (fabs(lat->b.x<1.0E-4)) {
    lat->b.x = 0.0;
  }
  lat->b.y = sqrtf(lat->cell.b*lat->cell.b-lat->b.x*lat->b.x);
  lat->b.z = 0.0;
  lat->c.x = lat->cell.c*cosf(lat->cell.beta*PI/180.);
  lat->c.y = (lat->cell.b*lat->cell.c*cosf(lat->cell.alpha*PI/180.)-lat->b.x*lat->c.x)/lat->b.y;
  if (fabs(lat->c.y)<1.0E-4) {
    lat->c.y = 0.0;
  }
  lat->c.z = sqrtf(lat->cell.c*lat->cell.c-lat->c.x*lat->c.x-lat->c.y*lat->c.y);
  printf("c.x=%f,c.y=%f,c.z=%f\n",lat->c.x,lat->c.y,lat->c.z);
  struct xyzcoords cross;
  float fac;
  cross = lcrossvec(lat->a,lat->b);
  fac = 1./ldotvec(lat->c,cross);
  lat->cstar = lmulscvec(fac,cross);
  cross = lcrossvec(lat->b,lat->c);
  fac = 1./ldotvec(lat->a,cross);
  lat->astar = lmulscvec(fac,cross);
  cross = lcrossvec(lat->c,lat->a);
  fac = 1./ldotvec(lat->b,cross);
  lat->bstar = lmulscvec(fac,cross);
  
  //  printf("%f,%f,%f,%f,%f,%f\n",lat->cell.a,lat->cell.b,lat->cell.c,lat->cell.alpha,lat->cell.beta,lat->cell.gamma);
  CloseShop:
  return(return_value);
}





