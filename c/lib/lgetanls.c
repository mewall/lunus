/* LGETANLS.C - Create an annular pixel mask.

   Author: Mike Wall
   Date: 9/27/95
   Version: 1.

*/

#include<mwmask.h>

int lgetanls(DIFFIMAGE *imdiff)
{
   short l,
         m;
   long mod_sq,
        outer_sqr,
        inner_sqr,
        k = 0;

   outer_sqr = imdiff->mask_outer_radius*imdiff->mask_outer_radius;
   inner_sqr = imdiff->mask_inner_radius*imdiff->mask_inner_radius;

   for (l = -imdiff->mask_outer_radius; l <= imdiff->mask_outer_radius; l++){
      for (m = -imdiff->mask_outer_radius; m <= imdiff->mask_outer_radius; m++){
         mod_sq = l*l+m*m;
         if ( ( mod_sq <= outer_sqr) && ( mod_sq >= inner_sqr) ){
            imdiff->mask[k].c = m;
            imdiff->mask[k].r = l;
            k++;
         }
      }
   }
   imdiff->mask_count = k;
}

