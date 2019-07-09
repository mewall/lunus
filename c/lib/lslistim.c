/* LSVIM.C - Compute scattering vectors for a diffraction image.
   
   Author: Mike Wall  
   Date: 6/11/2019
   Version: 1.
   
   */

#include<mwmask.h>

int lslistim(DIFFIMAGE *imdiff_in)
{
  size_t 
    index = 0;

  RCCOORDS_DATA
    r,
    c;

  int 
    return_value = 0;

  struct xyzcoords
    labpos;

  float
    labdist;

  int pidx;

  DIFFIMAGE *imdiff;

  for (pidx = 0; pidx < imdiff_in->num_panels; pidx++) {
    imdiff = &imdiff_in[pidx];
    index = 0;

    if (imdiff->slist == NULL) {
      imdiff->slist = (struct xyzcoords *)
	malloc(imdiff->image_length*sizeof(struct xyzcoords));
    }

    for(r=0; r < imdiff->vpixels; r++) {

      imdiff->pos.r = r;

      for(c=0; c < imdiff->hpixels; c++) {

	imdiff->pos.c = c;

	imdiff->slist[index] = lcalcsim(imdiff);

	index++;

      }
    }
#ifdef DEBUG
    int j;
	printf("SAMPLES\n");
	for (j=50000;j<50010;j++) {
	  printf("slist[%d]: (%f, %f, %f)\n",j,imdiff->slist[j].x,imdiff->slist[j].y,imdiff->slist[j].z);
	}
	printf("\n");
#endif
  }


  return(return_value);
}
