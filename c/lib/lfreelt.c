/* LFREELT.C - Free the allocated memory for a lattice.

   Author: Mike Wall
   Date: 9/27/95
   Version: 1.

*/

#include<mwmask.h>

int lfreelt(LAT3D *lat)
{

  int return_value = 0;

  if (lat->lattice != NULL) free(lat->lattice);
  if (lat->rfile != NULL) free(lat->rfile);
  if (lat->rfile_svecs != NULL) free(lat->rfile_svecs);
  if (lat->lattice_type_str != NULL) free(lat->lattice_type_str);
  if (lat->space_group_str != NULL) free(lat->space_group_str);
  if (lat->cell_str != NULL) free(lat->cell_str);
  if (lat->shim != NULL) free(lat->shim);
  if (lat->latct != NULL) free(lat->latct);

  free(lat);
  
  return(return_value);
}
