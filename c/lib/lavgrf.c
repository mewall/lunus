/* LAVGRF.C - Average the values of an rfile.
   
   Author: Mike Wall
   Date: 1/12/95
   Version: 1.
   
*/

#include<mwmask.h>

int lavgrf(DIFFIMAGE *imdiff1)
{
  size_t
    radius,
    n=0;
  
  int
    return_value = 0;
  
  for(radius = 0; radius < imdiff1->rfile_length; radius++) {
    if (imdiff1->rfile[radius] != imdiff1->rfile_mask_tag) {
         imdiff1->rfile[0]=(n*imdiff1->rfile[0] +
			    imdiff1->rfile[radius])/(float)(n+1);
	 n++;
    }
  }
  return(return_value);
}
