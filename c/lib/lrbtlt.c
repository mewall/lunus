/* LRBTLT.C - Generate a liquid-like motions model lattice.
   
   Author: Mike Wall
   Date: 9/26/2016
   Version: 1.
   
   */

#include<mwmask.h>
#include<complex.h>
#ifdef USE_FFTW
#include<fftw3.h>
#endif

int lrbtlt(LAT3D *lat)
{
#ifdef USE_FFTW
  size_t
    lat_index = 0;
  
  int
    return_value = 0;

  LAT3D *latG;

  struct ijkcoords 
    rvec,
    index;

  float
    rscale,
    factor,
    rsqr,
    dwf;

  struct xyzcoords
    r1,r2,r3,rfloat,s,U_times_s;

  float *sf,*gamma;
  fftwf_complex *sf_t,*gamma_t;
  fftwf_plan p;

  sf = (float *)lat->lattice;
  lat_index = 0;
  float exparg;
  // Multiply by resolution sigma-dependent factor
  for(index.k = 0; index.k < lat->zvoxels; index.k++) {
    for(index.j = 0; index.j < lat->yvoxels; index.j++) {
      for (index.i = 0; index.i < lat->xvoxels; index.i++) {
	lat->index = index;
	s = lsFromIndex(lat);
	U_times_s = lmatvecmul(lat->anisoU,s);	
	exparg = 2.*PI*2.*PI*ldotvec(s,U_times_s);
	dwf = expf(-exparg);
	sf[lat_index] *= (LATTICE_DATA_TYPE)(1.-dwf)*dwf/((LATTICE_DATA_TYPE)lat->lattice_length*sqrtf((LATTICE_DATA_TYPE)lat->lattice_length));
	//sf[lat_index] *= (LATTICE_DATA_TYPE)exparg*dwf/((LATTICE_DATA_TYPE)lat->lattice_length*sqrtf((LATTICE_DATA_TYPE)lat->lattice_length));
	lat_index++;
      }
    }
  }


  CloseShop:
  return(return_value);
#endif
}


