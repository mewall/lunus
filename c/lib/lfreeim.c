/* LFREEIM.C - Free the allocated memory for a diffraction image.

   Author: Mike Wall
   Date: 9/27/95
   Version: 1.

*/

#include<mwmask.h>

int lfreeim(DIFFIMAGE *imdiff_in)
{
  size_t
    i;
  int 
    return_value;
  
  /*
   * Free diffraction image:
   */
  int pidx;

  DIFFIMAGE *imdiff;

  for (pidx = 0; pidx < imdiff_in->num_panels; pidx++) {
    imdiff = &imdiff_in[pidx];

    if (imdiff->rfile != NULL) free((RFILE_DATA_TYPE *)imdiff->rfile);
    if (imdiff->imscaler != NULL) free((RFILE_DATA_TYPE *)imdiff->imscaler);
    if (imdiff->imoffsetr != NULL) free((RFILE_DATA_TYPE *)imdiff->imoffsetr);
    if (imdiff->mask != NULL) free((struct rccoords *)imdiff->mask);
    if (imdiff->image != NULL) free((IMAGE_DATA_TYPE *)imdiff->image);
    if (imdiff->correction != NULL) free(imdiff->correction);
    if (imdiff->header != NULL) free((char *)imdiff->header);
    if (imdiff->footer != NULL) free(imdiff->footer);
    if (imdiff->overload != NULL) free((struct rccorrds *)imdiff->overload);
    if (imdiff->peak != NULL) free((struct xycoords *)imdiff->peak);
    if (imdiff->weights != NULL) free((WEIGHTS_DATA_TYPE *)imdiff->weights);

  }

  free((DIFFIMAGE *)imdiff);
}
