/* LFREEIM.C - Free the allocated memory for a diffraction image.

   Author: Mike Wall
   Date: 9/27/95
   Version: 1.

*/

#include<mwmask.h>

int lclearim(DIFFIMAGE *imdiff_in)
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

  if (imdiff_in != NULL) {
  
    for (pidx = 0; pidx < imdiff_in->num_panels; pidx++) {
      imdiff = &imdiff_in[pidx];
      
      if (imdiff->rfile != NULL) free(imdiff->rfile);
      if (imdiff->image != NULL) free(imdiff->image);
      if (imdiff->correction != NULL) free(imdiff->correction);
      if (imdiff->imscaler != NULL) free(imdiff->imscaler);
      if (imdiff->imoffsetr != NULL) free(imdiff->imoffsetr);
      if (imdiff->mask != NULL) free(imdiff->mask);
      if (imdiff->header != NULL) free(imdiff->header);
      if (imdiff->footer != NULL) free(imdiff->footer);
      if (imdiff->overload != NULL) free(imdiff->overload);
      if (imdiff->peak != NULL) free(imdiff->peak);
      if (imdiff->weights != NULL) free(imdiff->weights);
      
    }
  }
}

int lfreeim(DIFFIMAGE *imdiff_in)
{
  size_t
    i;
  int 
    return_value;

  lclearim(imdiff_in);  
  if (imdiff_in != NULL) free(imdiff_in);
}
