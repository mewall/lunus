/* LTIME.C - Return the wall clock time.
   
   Author: Mike Wall   
   Date: 3/18/2020
   Version: 1.
   
*/

#ifdef USE_OPENMP
#include<omp.h>
#else
#include<time.h>
#endif

double ltime() {
  double t;
#ifdef USE_OPENMP
  t = omp_get_wtime();
  //  t = 0.0;
#else
  t = ((double)clock())/CLOCKS_PER_SEC;
#endif
  return t;
}

