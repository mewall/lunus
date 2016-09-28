/* LSVTOOLS.C - Scattering vector calculation tools.
   
   Author: Mike Wall
   Date: 9/26/2016
   Version: 1.
   
   */

#include<mwmask.h>
#include<complex.h>
#include<fftw3.h>

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



