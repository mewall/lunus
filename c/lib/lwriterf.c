/* LWRITERF.C - Write an rfile to a file.

   Author: Mike Wall
   Date: 9/27/95
   Version: 1.

*/

#include<mwmask.h>

int lwriterf(DIFFIMAGE *imdiff)
{
	size_t
		num_wrote;

	int
		return_value = 0;  


  num_wrote = fwrite(imdiff->rfile, sizeof(RFILE_DATA_TYPE),
                     imdiff->rfile_length, imdiff->outfile);
  if (num_wrote != imdiff->rfile_length) { 
    sprintf(imdiff->error_msg,"\nLWRITERF:  Only wrote %ld words to "
		"rfile.\n\n", num_wrote);
    return_value = 1;
  }
  printf("rfile length = %d\n", imdiff->rfile_length);
  printf("sizeof(RFILE_DATA_TYPE) = %d\n", sizeof(RFILE_DATA_TYPE));
  printf("num_wrote = %d\n", num_wrote);
  return(return_value);
}
