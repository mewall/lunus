/* LINTXDSLT.C - Integrate an image worth of diffuse intensity and place on a lattice.
   
   Author: Mike Wall
   Date: 9/15/2015
   Version: 1.
   
   */

#include<mwmask.h>

int lintxdslt(DIFFIMAGE *imdiff,LAT3D *lat)
{
  size_t 
    index,latidx;
  
  int i,j,
    return_value = 0;

  struct xyzmatrix R;
  struct xyzcoords X,V;
  struct ijkcoords hkl;

  index=0;

  for(j=0; j < imdiff->vpixels; j++) {
    for(i=0; i < imdiff->hpixels; i++) {
      if (imdiff->image[index] != imdiff->mask_tag) {
	V.x=(float)i;
	V.y=(float)j;
	V.z=1.;
	R=lrotmat(1.,1.,1.);
	X=lmatvecmul(R,V);
	hkl.i=(int)X.x; hkl.j=(int)X.y; hkl.k=(int)X.z;
	latidx = imdiff->image_length*hkl.k+imdiff->image_length*hkl.j+hkl.i;
	if (latidx<lat->lattice_length) {
	  lat->lattice[latidx]=imdiff->image[index];
	}
      }
      index++;  
    }
  }
  return(return_value);
}
