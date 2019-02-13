/* LCONSTRF.C - Create an rfile with the value of a constant given at the
		input line.
   
   Author: Mike Wall
   Date: 4/19/94
   Version: 1.
   
   */

#include<mwmask.h>

int lconstrf(DIFFIMAGE *imdiff)
{
	size_t
		radius;

	RFILE_DATA_TYPE
		rfile_value;

	int
		return_value = 0;

  rfile_value = imdiff->rfile[0];
  for(radius = 1; radius < imdiff->rfile_length; radius++) {
    imdiff->rfile[radius] = rfile_value;
  }
  return(return_value);
}
