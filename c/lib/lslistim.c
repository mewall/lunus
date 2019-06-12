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

    labpos.z = imdiff->distance_mm;

    for(r=0; r < imdiff->vpixels; r++) {

      labpos.y = (float)(r * imdiff->pixel_size_mm - imdiff->beam_mm.y);

      for(c=0; c < imdiff->hpixels; c++) {

	labpos.x = (float)(c * imdiff->pixel_size_mm - imdiff->beam_mm.x);

	labdist = sqrtf(labpos.x*labpos.x+labpos.y*labpos.y+labpos.z*labpos.z);

	imdiff->slist[index].x = labpos.x / labdist / imdiff->wavelength;
	imdiff->slist[index].y = labpos.y / labdist / imdiff->wavelength;
	imdiff->slist[index].z = - (1. - labpos.z / labdist) / imdiff->wavelength;

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
