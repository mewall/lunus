/* LSETMETIM.C - Set default metrology given image parameters

   Author: Mike Wall
   Date: 6/20/2019
   Version: 1.

*/

#include<mwmask.h>

int lsetmetim(DIFFIMAGE *imdiff_in) 

{

  int pidx;

  DIFFIMAGE *imdiff;

  for (pidx = 0; pidx < imdiff_in->num_panels; pidx++) {
    imdiff = &imdiff_in[pidx];

    imdiff->fast_vec.x = 1.0;
    imdiff->fast_vec.y = 0.0;
    imdiff->fast_vec.z = 0.0;
    
    imdiff->slow_vec.x = 0.0;
    imdiff->slow_vec.y = -1.0;
    imdiff->slow_vec.z = 0.0;
    
    // Default normal direction
    
    imdiff->normal_vec = lcrossvec(imdiff->fast_vec,imdiff->slow_vec);
    
    // Default detector origin vector
    
    imdiff->origin_vec.x = - imdiff->beam_mm.x;
    imdiff->origin_vec.y = + imdiff->beam_mm.y;
    imdiff->origin_vec.z = - imdiff->distance_mm;
    
    // Default beam direction
    
    imdiff->beam_vec.x = 0.0;
    imdiff->beam_vec.y = 0.0;
    imdiff->beam_vec.z = -1.0;
      
  }

  return(0);

}

