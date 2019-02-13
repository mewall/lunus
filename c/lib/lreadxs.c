/* LREADXS.C - Read crystal structure from a file.

   Author: Mike Wall
   Date: 12/3/15
   Version: 1.

*/

#include<mwmask.h>

int lreadxs(XTALSTRUCT *xs)
{

  size_t
    num_read = 0;

  int i;

  int
    return_value = 0;

  char buf[1024];

  fscanf(xs->infile,"%d",&xs->natoms); // read number of atoms
  //  printf("%d\n",xs->natoms);
  //  fscanf(xs->infile,"%s %s",buf,buf);
  //fgets(buf,1024,xs->infile); // skip a line
  //  printf("%s\n",buf);
  xs->pos = (struct xyzcoords *)malloc(xs->natoms*sizeof(struct xyzcoords));
  xs->u = (struct adps *)malloc(xs->natoms*sizeof(struct adps));
  
  int nline;

  for (i=0;i<xs->natoms;i++) {
    nline=fscanf(xs->infile,"%s %g %g %g %g %g %g %g %g %g",buf,&xs->pos[i].x,&xs->pos[i].y,&xs->pos[i].z,&xs->u[i].U11,&xs->u[i].U22,&xs->u[i].U33,&xs->u[i].U12,&xs->u[i].U13,&xs->u[i].U23);
    //printf("%d %f\n",nline,xs->pos[i].x);
    num_read++;
  }
  if (num_read != xs->natoms) {
    printf("/nCouldn't read all of the structure from input file.\n\n");
    return_value = 2;
    goto CloseShop;
  }
  CloseShop:
  return(return_value);
}
