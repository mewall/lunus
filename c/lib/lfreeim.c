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

  for (i=0; i<imdiff->num_panels; i++) {
    if (imdiff[i].rfile != NULL) free((RFILE_DATA_TYPE *)imdiff->rfile);
    if (imdiff[i].imscaler != NULL) free((RFILE_DATA_TYPE *)imdiff->imscaler);
    if (imdiff[i].imoffsetr != NULL) free((RFILE_DATA_TYPE *)imdiff->imoffsetr);
    if (imdiff[i].mask != NULL) free((struct rccoords *)imdiff->mask);
    if (imdiff[i].image != NULL) free((IMAGE_DATA_TYPE *)imdiff->image);
    if (imdiff[i].correction != NULL) free(imdiff->correction);
    if (imdiff[i].header != NULL) free((char *)imdiff->header);
    if (imdiff[i].footer != NULL) free(imdiff->footer);
    if (imdiff[i].overload != NULL) free((struct rccorrds *)imdiff->overload);
    if (imdiff[i].peak != NULL) free((struct xycoords *)imdiff->peak);
    if (imdiff[i].weights != NULL) free((WEIGHTS_DATA_TYPE *)imdiff->weights);
  }
  free((DIFFIMAGE *)imdiff);
}
