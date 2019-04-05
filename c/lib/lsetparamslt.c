/* LSETPARAMSLT.C - Set lattice parameters using a parameter definitions string.

   Author: Mike Wall
   Date: 4/5/2019
   Version: 1.

*/

#include<mwmask.h>
#include <string.h>

int lsetparamslt(LAT3D *lat)
{

  char *deck = lat->params;

	if (strstr(deck,"\nintegration_image_type") == NULL) {
	  lat->integration_image_type = (char *)calloc(strlen("raw")+1,sizeof(char));
	  strcpy(lat->integration_image_type,"raw");
	} else {
	  lat->integration_image_type=lgettag(deck,"\nintegration_image_type");
	}

	if (strstr(deck,"\nfilterhkl") == NULL) {
	  lat->filterhkl = 1;
	} else if (strcmp(lgettag(deck,"\nfilterhkl"),"False")==0) {
	  lat->filterhkl=0;
	} 

	if (strstr(deck,"\npphkl") == NULL) {
	  lat->pphkl = 1;
	} else {
	  lat->pphkl = lgettagi(deck,"\npphkl");
	}

	if (strstr(deck,"\npoints_per_hkl") != NULL) {
	  lat->pphkl = lgettagi(deck,"\npoints_per_hkl");
	}

	if (strstr(deck,"\nunit_cell") != NULL) {
	  lat->cell_str=lgettag(deck,"\nunit_cell");
	}

	if (strstr(deck,"\nspacegroup") != NULL) {
	  lat->space_group_str=lgettag(deck,"\nspacegroup");
	}

	if (strstr(deck,"\nresolution") == NULL) {
	  lat->resolution.max = 0.;
	} else {
	  lat->resolution.max = lgettagf(deck,"\nresolution");
	}

  return(0);
}

