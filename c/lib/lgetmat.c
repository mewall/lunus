/* LGETMAT.C - Extract the crystal orientation from a DENZO output
               file.

   Author: Mike Wall
   Date: 9/27/95
   Version: 1.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mwmask.h>

#define XPOS 45
#define XLEN 9

int lgetmat(DIFFIMAGE *imdiff)
{
  char *inl;
  long i;
  int xpos;
  float u[9];

  /*
   * Allocate memory:
   */
  
  inl = (char *)malloc(sizeof(char)*(LINESIZE+1));
  
  /*
   * Skip the first line in the file:
   */
  
  fgets(inl, LINESIZE, imdiff->infile);
  for(i=0;i<=2;i++) {
    fgets(inl, LINESIZE, imdiff->infile);
    
    /*
     * Extract u-matrix:
     */
    
    xpos=XPOS;
    inl[xpos+XLEN]=0;
    u[i] = atof(inl+xpos);
    xpos=xpos+XLEN+1;
    inl[xpos+XLEN]=0;
    u[3+i] = atof(inl+xpos);
    xpos=xpos+XLEN+1;
    inl[xpos+XLEN]=0;
    u[6+i] = atof(inl+xpos);
  }
  imdiff->u.xx = u[2];
  imdiff->u.xy = u[1];
  imdiff->u.xz = u[0];
  imdiff->u.yx = u[5];
  imdiff->u.yy = u[4];
  imdiff->u.yz = u[3];
  imdiff->u.zx = u[8];
  imdiff->u.zy = u[7];
  imdiff->u.zz = u[6];
  
/*for(i=0;i<=8;i++){printf ("%f\n",u[i]);}/***/

  /*
   * Free memory:
   */
  
  free((char *)inl);
  return(0);
}




