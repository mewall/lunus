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

  struct xyzcoords o;

/*
 * Write lattice to output file:
 */

//  printf("Writing cube file\n");

  o.x = lat->origin.i*lat->astar.x + lat->origin.j*lat->bstar.x + lat->origin.k*lat->cstar.x;
  o.y = lat->origin.i*lat->astar.y + lat->origin.j*lat->bstar.y + lat->origin.k*lat->cstar.y;
  o.z = lat->origin.i*lat->astar.z + lat->origin.j*lat->bstar.z + lat->origin.k*lat->cstar.z;

  fprintf(lat->outfile,"Gaussian cube format\n");
  fprintf(lat->outfile,"lattice_type_str=%s;unit_cell=%s;space_group=%s;\n",lat->lattice_type_str,lat->cell_str,lat->space_group_str);
  fprintf(lat->outfile,"%5d%12.6f%12.6f%12.6f\n",1,-o.z,-o.x,-o.y);
  //  fprintf(lat->outfile,"%d %e %e %e\n",0,0.0,0.0,0.0);
  fprintf(lat->outfile,"%5d%12.6f%12.6f%12.6f\n",lat->zvoxels,lat->cstar.z,lat->cstar.x,lat->cstar.y);
  fprintf(lat->outfile,"%5d%12.6f%12.6f%12.6f\n",lat->yvoxels,lat->bstar.z,lat->bstar.x,lat->bstar.y);
  fprintf(lat->outfile,"%5d%12.6f%12.6f%12.6f\n",lat->xvoxels,lat->astar.z,lat->astar.x,lat->astar.y);
  fprintf(lat->outfile,"%5d%12.6f%12.6f%12.6f%12.6f\n",1,0.0,0.0,0.0,0.0);

  int index = 0,ct=0;
  int i,j,k;

  for (k=0;k<lat->zvoxels;k++) {
    for (j=0;j<lat->yvoxels;j++) {
      for (i=0;i<lat->xvoxels;i++) {
	index = lat->xyvoxels*k+lat->xvoxels*j+i;
	fprintf(lat->outfile, "%12.5g ",lat->lattice[index]);
	if (i%6 == 5) fprintf(lat->outfile,"\n");
	ct++;
      }
      fprintf(lat->outfile, "\n");
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



