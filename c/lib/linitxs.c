/* LINITXS.C - Initialize a crystal structure.

   Author: Mike Wall
   Date: 12/3/15
   Version: 1.

*/

#include<mwmask.h>

XTALSTRUCT *linitxs(void) 
{

  XTALSTRUCT *xs;

  xs = (XTALSTRUCT *)malloc(sizeof(XTALSTRUCT));
  if (!xs) {
    printf("\nLINITXS:  Unable to allocate all memory (xs).\n");
    xs = NULL;
    goto CloseShop;
  }
  
  xs->natoms = DEFAULT_NATOMS;
  xs->pos = (struct xyzcoords *)calloc(xs->natoms,sizeof(struct xyzcoords));
  xs->u = (struct adps *)calloc(xs->natoms,sizeof(struct adps));
  if (!xs->pos || !xs->u) {
    printf("\nLINITXS:  Unable to allocate all memory.\n");
    xs = NULL;
    goto CloseShop;
  }

  CloseShop:

  return(xs);

}












