/* LFREEIM.C - Free the allocated memory for a diffraction image.

   Author: Mike Wall
   Date: 9/27/95
   Version: 1.

*/

#include<mwmask.h>

int lfreeim(DIFFIMAGE *imdiff)
{
  size_t
    i;
  int 
    return_value;
  
  /*
   * Free diffraction image:
   */
  
  free((RFILE_DATA_TYPE *)imdiff->rfile);
  free((RFILE_DATA_TYPE *)imdiff->imscaler);
  free((RFILE_DATA_TYPE *)imdiff->imoffsetr);
  free((struct rccoords *)imdiff->mask);
  free((IMAGE_DATA_TYPE *)imdiff->image);
  free((char *)imdiff->header);
  free((struct rccorrds *)imdiff->overload);
  free((struct xycoords *)imdiff->peak);
  free((WEIGHTS_DATA_TYPE *)imdiff->weights);
  free(imdiff->correction);
  //  free(imdiff->footer);
  free((DIFFIMAGE *)imdiff);
}
