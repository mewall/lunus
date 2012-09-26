/* LWRITEDF.C - Write a diffuse features file.

   Author: Mike Wall
   Date: 9/27/95
   Version: 1.

*/

#include<mwmask.h>

int lwritedf(DIFFIMAGE *imdiff)
{

  int
    return_value = 0;  
  
  size_t
    i,
    index;

  /*
   * Write diffuse features file:
   */

  for (i=0;i<imdiff->feature_count;i++) {
    fprintf(imdiff->outfile, "%d %d %g %ld %d\n", 
	    (int)imdiff->feature[i].pixel_pos.c, 
	    (int)imdiff->feature[i].pixel_pos.r, 
	    (float)imdiff->feature[i].radius,
	    (long)imdiff->feature[i].peak_value, 
	    (int)imdiff->feature[i].average_value);
  }
  
  return(return_value);
}
