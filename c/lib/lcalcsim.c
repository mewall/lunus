/* LSVIM.C - Compute scattering vectors for a diffraction image.
   
   Author: Mike Wall  
   Date: 6/11/2019
   Version: 1.
   
   */

#include<mwmask.h>

struct xyzcoords lcalcsim(DIFFIMAGE *imdiff)
{
  size_t 
    index = 0;

  RCCOORDS_DATA
    r,
    c;

  int 
    return_value = 0;

  struct xyzcoords
    labpos, labdir, s, s0;

  float
    labdist;

  int pidx;

  labpos.x = imdiff->pos.c * imdiff->fast_vec.x * imdiff->pixel_size_mm + imdiff->pos.r * imdiff->slow_vec.x * imdiff->pixel_size_mm + imdiff->origin_vec.x;
  labpos.y = imdiff->pos.c * imdiff->fast_vec.y * imdiff->pixel_size_mm + imdiff->pos.r * imdiff->slow_vec.y * imdiff->pixel_size_mm + imdiff->origin_vec.y;
  labpos.z = imdiff->pos.c * imdiff->fast_vec.z * imdiff->pixel_size_mm + imdiff->pos.r * imdiff->slow_vec.z * imdiff->pixel_size_mm + imdiff->origin_vec.z;

#ifdef DEBUG

  printf("labpos.xyz = (%f, %f, %f)\n",labpos.x,labpos.y,labpos.z);
  printf("origin.xyz = (%f, %f, %f)\n",imdiff->origin_vec.x,imdiff->origin_vec.y,imdiff->origin_vec.z);
  printf("labpos.xyz = (%f, %f, %f)\n",imdiff->slow_vec.x,imdiff->slow_vec.y,imdiff->slow_vec.z);

#endif

  labdist = sqrtf(ldotvec(labpos,labpos));

  labdir.x = - labpos.x / labdist;
  labdir.y = - labpos.y / labdist;
  labdir.z = - labpos.z / labdist;

  s.x = (imdiff->beam_vec.x - labdir.x) / imdiff->wavelength;
  s.y = (imdiff->beam_vec.y - labdir.y)/ imdiff->wavelength;
  s.z = (imdiff->beam_vec.z - labdir.z)/ imdiff->wavelength;

  return(s);
}
