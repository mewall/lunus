/* LXAVGR.C - Calculate the average pixel intensity product between two 
		images as a fuction of radius.

   Author: Mike Wall
   Version:1.0
   Date:4/7/94
				 
	 Input argument is two diffraction image.  Output is stored in rfile
	 of first image (imdiff1->rfile).
   Version:2.0
   Date:4/14/94

	 Use rdata indexing to calculate this, after it was discovered that
	    the old technique didn't work.
*/

#include<mwmask.h>

int lxavgr(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2)
{
	size_t
		max_r,
		radius,
		index1 = 0,
		index2 = 0;
	double
		value_sum;
	
  max_r = lmin(imdiff1->rdata_radius,imdiff2->rdata_radius);
  for(radius = 0; radius <= max_r; radius++) {
    value_sum = 0;
    printf("Working on radius %ld\n",radius);
/*    printf("   count[1] = %ld, count[2] = %ld\n",imdiff1->rdata[radius].count,
						 imdiff2->rdata[radius].count);
*/
    for(index1 = 0; index1 < imdiff1->rdata[radius].count; index1++) {
      for(index2 = 0; index2 < imdiff2->rdata[radius].count; index2++) {
	value_sum += (double)imdiff1->rdata[radius].value[index1] * 
		     (double)imdiff2->rdata[radius].value[index2];
      }
    }
    imdiff1->rfile[radius] = (RFILE_DATA_TYPE)(value_sum / (double)
			    (imdiff1->rdata[radius].count * 
			     imdiff2->rdata[radius].count));
  }
  imdiff1->rfile_length = max_r;
}
