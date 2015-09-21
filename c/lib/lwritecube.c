/* LWRITECUBE.C - Write a lattice to a .cube file.

   Author: Mike Wall
   Date: 9/20/15
   Version: 1.

*/

#include<mwmask.h>

int lwritecube(LAT3D *lat)
{

  int
    return_value = 0;

/*
 * Write lattice to output file:
 */

//  printf("Writing cube file\n");

  fprintf(lat->outfile,"Gaussian cube format\n");
  fprintf(lat->outfile,"lattice_type_str=%s;unit_cell=%s;space_group=%s;\n",lat->lattice_type_str,lat->cell_str,lat->space_group_str);
  fprintf(lat->outfile,"%d %e %e %e\n",0,lat->xbound.min,lat->ybound.min,lat->zbound.min);
  fprintf(lat->outfile,"%d %e %e %e\n",lat->xvoxels,lat->astar.x,lat->astar.y,lat->astar.z);
  fprintf(lat->outfile,"%d %e %e %e\n",lat->yvoxels,lat->bstar.x,lat->bstar.y,lat->bstar.z);
  fprintf(lat->outfile,"%d %e %e %e\n",lat->zvoxels,lat->cstar.x,lat->cstar.y,lat->cstar.z);

  int index = 0,ct=0;
  int i,j,k;

  for (i=0;i<lat->xvoxels;i++) {
    for (j=0;j<lat->yvoxels;j++) {
      for (k=0;k<lat->zvoxels;k++) {
	index = lat->xyvoxels*k+lat->xvoxels*j+i;
	fprintf(lat->outfile, "%g ",lat->lattice[index]);
	if (k%6 == 5) fprintf(lat->outfile,"\n");
	ct++;
      }
    }
  }
  

  if (ct != lat->lattice_length) {
    printf("/nCouldn't write all of the lattice to output file.\n\n");
    return_value = 1;
    goto CloseShop;
  }

  CloseShop:

  return(return_value);
}



