/* LMIN.C - Output the minimum of two numbers.

   Author: Mike Wall
   Date: 9/27/95
   Version: 1.

*/

#include<mwmask.h>
size_t lmin(size_t arg1, size_t arg2)
{
	size_t return_value;

  if (arg1 > arg2) {
    return_value = arg2;
  }
  else return_value = arg1;
  return(return_value);
}
