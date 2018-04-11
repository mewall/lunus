/* LSVTOOLS.C - Scattering vector calculation tools.
   
   Author: Mike Wall
   Date: 9/26/2016
   Version: 1.
   
   */

#include<mwmask.h>
#include<complex.h>

  float lssqrFromIndex(LAT3D *lat) 
  {
    struct ijkcoords rvec;
    struct xyzcoords r1,r2,r3,rfloat;
    XYZCOORDS_DATA rsqr;

    rvec.i = lat->index.i - lat->origin.i;
    rvec.j = lat->index.j - lat->origin.j;
    rvec.k = lat->index.k - lat->origin.k;
    r1 = lmulscvec((XYZCOORDS_DATA)rvec.i,lat->astar);
    r2 = lmulscvec((XYZCOORDS_DATA)rvec.j,lat->bstar);
    r3 = lmulscvec((XYZCOORDS_DATA)rvec.k,lat->cstar);
    rfloat = laddvec(laddvec(r1,r2),r3);
    rsqr = (rfloat.x*rfloat.x + rfloat.y*rfloat.y + 
		       rfloat.z*rfloat.z);
    return rsqr;
  }

  struct xyzcoords lsFromIndex(LAT3D *lat) 
  {
    struct ijkcoords rvec;
    struct xyzcoords r1,r2,r3,rfloat;
    XYZCOORDS_DATA rsqr;

    rvec.i = lat->index.i - lat->origin.i;
    rvec.j = lat->index.j - lat->origin.j;
    rvec.k = lat->index.k - lat->origin.k;
    r1 = lmulscvec((XYZCOORDS_DATA)rvec.i,lat->astar);
    r2 = lmulscvec((XYZCOORDS_DATA)rvec.j,lat->bstar);
    r3 = lmulscvec((XYZCOORDS_DATA)rvec.k,lat->cstar);
    rfloat = laddvec(laddvec(r1,r2),r3);
    //    rsqr = (rfloat.x*rfloat.x + rfloat.y*rfloat.y + 
    //		       rfloat.z*rfloat.z);
    return rfloat;
  }

  struct ijkcoords lindexFromS(LAT3D *lat) 
  {
    struct xyzmatrix cellstar;
    struct ijkcoords index;
    struct xyzcoords hkl;

    cellstar.xx = lat->astar.x;
    cellstar.xy = lat->bstar.x;
    cellstar.xz = lat->cstar.x;
    cellstar.yx = lat->astar.y;
    cellstar.yy = lat->bstar.y;
    cellstar.yz = lat->cstar.y;
    cellstar.zx = lat->astar.z;
    cellstar.zy = lat->bstar.z;
    cellstar.zz = lat->cstar.z;

    hkl = lmatvecmul(lmatinv(cellstar),lat->sv);

    index.i = (IJKCOORDS_DATA)roundf(hkl.x)+ lat->origin.i;
    index.j = (IJKCOORDS_DATA)roundf(hkl.y)+ lat->origin.j;
    index.k = (IJKCOORDS_DATA)roundf(hkl.z)+ lat->origin.k;

    //    index.i = (IJKCOORDS_DATA)roundf(ldotvec(lat->sv,lat->astar)/ldotvec(lat->astar,lat->astar)) + lat->origin.i;
    //    index.j = (IJKCOORDS_DATA)roundf(ldotvec(lat->sv,lat->bstar)/ldotvec(lat->bstar,lat->bstar)) + lat->origin.j;
    //    index.k = (IJKCOORDS_DATA)roundf(ldotvec(lat->sv,lat->cstar)/ldotvec(lat->cstar,lat->cstar)) + lat->origin.k;
    //    rsqr = (rfloat.x*rfloat.x + rfloat.y*rfloat.y + 
    //		       rfloat.z*rfloat.z);
    return index;
  }


