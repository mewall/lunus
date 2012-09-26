/* LREADDF.C - Read diffuse features from a file.

   Author: Mike Wall
   Date: 9/27/95
   Version: 1.

*/

#include<mwmask.h>

int lreaddf(DIFFIMAGE *imdiff)
{
  int
    r,
    c,
    average_value,
    return_value = 0;  
  
  char input_line[LINESIZE+1];
  
  size_t
    i = 0;
 
  /*
   * Read diffuse features file:
   */
  
  while (fgets(input_line, LINESIZE, imdiff->infile) != NULL) {
    sscanf(input_line, "%d %d %g %ld %d", &c, &r, 
	   &imdiff->feature[i].radius,
	   &imdiff->feature[i].peak_value,
	   &average_value);
    imdiff->feature[i].pixel_pos.c = (RCCOORDS_DATA)c; 
    imdiff->feature[i].pixel_pos.r = (RCCOORDS_DATA)r; 
    imdiff->feature[i].average_value = (IMAGE_DATA_TYPE)average_value;
    i++;
  }
  imdiff->feature_count = i;
  return(return_value);
}


