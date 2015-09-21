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
  lat->b.x = lat->cell.b*cosf(lat->cell.gamma*180./PI);
  if (fabs(lat->b.x<1.0E-4)) {
    lat->b.x = 0.0;
  }
  lat->b.y = sqrtf(lat->cell.b*lat->cell.b-lat->b.x*lat->b.x);
  lat->b.z = 0.0;
  lat->c.x = lat->cell.c*cosf(lat->cell.beta*180./PI);
  lat->c.y = (lat->cell.b*lat->cell.c*cosf(lat->cell.alpha*180./PI)-lat->b.x*lat->c.x)/lat->b.y;
  if (fabs(lat->c.y)<1.0E-4) {
    lat->c.y = 0.0;
  }
  lat->c.z = sqrtf(lat->cell.c*lat->cell.c-lat->c.x*lat->c.x-lat->c.y*lat->c.y);
  struct xyzcoords cross;
  float denom;
  cross = lcrossvec(lat->a,lat->b);
  denom = ldotvec(lat->c,cross);
  lat->cstar = lmulscvec(1./denom,cross);
  cross = lcrossvec(lat->b,lat->c);
  denom = ldotvec(lat->a,cross);
  lat->astar = lmulscvec(1./denom,cross);
  cross = lcrossvec(lat->c,lat->a);
  denom = ldotvec(lat->b,cross);
  lat->bstar = lmulscvec(1./denom,cross);
  
  //  printf("%f,%f,%f,%f,%f,%f\n",lat->cell.a,lat->cell.b,lat->cell.c,lat->cell.alpha,lat->cell.beta,lat->cell.gamma);
  CloseShop:
  return(return_value);
}





