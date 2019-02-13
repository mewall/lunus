/* LFREELT.C - Free the allocated memory for a lattice.

   Author: Mike Wall
   Date: 9/27/95
   Version: 1.

*/

#include<mwmask.h>

int lfreelt(LAT3D *lat)
{

  int return_value = 0;

  free((LATTICE_DATA_TYPE *)lat->lattice);
  free((LAT3D *)lat);
  free((RFILE_DATA_TYPE *)lat->rfile);

  return(return_value);
}
