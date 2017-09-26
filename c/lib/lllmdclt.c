/* LLLMDCLT.C - Use deconvolution to back out Bragg-like intensities from a diffuse lattice, assuming liquid-like motions
   
   Author: Mike Wall
   Date: 2/23/2017
   Version: 1.
   
   */

#include<mwmask.h>
#include<complex.h>
#ifdef USE_FFTW
#include<fftw3.h>
#endif
int lllmdclt(LAT3D *lat)
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

  latG = (LAT3D *)malloc(sizeof(LAT3D));
  *latG = *lat;

  latG->lattice = (LATTICE_DATA_TYPE *)malloc(sizeof(LATTICE_DATA_TYPE)
					      *latG->lattice_length);

  latG->origin.i = 0;
  latG->origin.j = 0;
  latG->origin.k = 0;

  float sum=0;
  rscale = (lat->xscale*lat->xscale + lat->yscale*lat->yscale +
		 lat->zscale*lat->zscale);
  for(index.k = 0; index.k < lat->zvoxels; index.k++) {
    for(index.j = 0; index.j < lat->yvoxels; index.j++) {
      for (index.i = 0; index.i < lat->xvoxels; index.i++) {
	latG->index = index;
	rsqr = lssqrFromIndex(latG);
	latG->lattice[lat_index] = (LATTICE_DATA_TYPE)
	  8.*PI*lat->gamma*lat->gamma*lat->gamma/powf((1.+lat->gamma*lat->gamma*2.*PI*2.*PI*rsqr),2.);
	//	  4.*PI*lat->width*lat->width*lat->width/(1.+lat->width*lat->width*2.*PI*2.*PI*rsqr);
	sum += latG->lattice[lat_index];
	  // Clarage et al 1992 model:	  
	  //	  	  rsqr*2.*PI*2.*PI*lat->width*lat->width*expf(-rsqr*2.*PI*2.*PI*lat->width*lat->width);
	// Modfied clarage model:
	//(1.0 - expf(-rsqr*2.*PI*2.*PI*lat->width*lat->width))*expf(-rsqr*2.*PI*2.*PI*lat->width*lat->width);
	  // Wall et al 1997 model:
	//(1.0 - expf(-rsqr*2.*PI*2.*PI*lat->width*lat->width));
	lat_index++;
      }
    }
  }

  int i;
  for (i=0;i<latG->lattice_length;i++) {
    latG->lattice[i] /= sum;
  }
  size_t tlen = lat->zvoxels*lat->yvoxels*(lat->xvoxels/2+1);
  // Prepare for fftw
  sf = (float *)lat->lattice;
  gamma = (float *)latG->lattice;
  sf_t = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex)*tlen);
  gamma_t = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex)*tlen);

  lat_index = 0;
  float exparg;
  // Divide by resolution sigma-dependent factor
  for(index.k = 0; index.k < lat->zvoxels; index.k++) {
    for(index.j = 0; index.j < lat->yvoxels; index.j++) {
      for (index.i = 0; index.i < lat->xvoxels; index.i++) {
	lat->index = index;
	s = lsFromIndex(lat);
	U_times_s = lmatvecmul(lat->anisoU,s);	
	exparg = 2.*PI*2.*PI*ldotvec(s,U_times_s);
	if (exparg <= 0.001) {
	  exparg = 0.001;
	}
	dwf = expf(-exparg);
	//	sf[lat_index] *= (LATTICE_DATA_TYPE)(1.-dwf)*dwf/((LATTICE_DATA_TYPE)lat->lattice_length*sqrtf((LATTICE_DATA_TYPE)lat->lattice_length));
	sf[lat_index] /= (LATTICE_DATA_TYPE)exparg*dwf/((LATTICE_DATA_TYPE)lat->lattice_length*sqrtf((LATTICE_DATA_TYPE)lat->lattice_length));
	lat_index++;
      }
    }
  }

  // Convert mask tags to zeros in preparation for FFT
  for (i=0;i<lat->lattice_length;i++) {
    if (sf[i] == (float)lat->mask_tag) sf[i] = 0;
  }

  // Perform sf fft
  p = fftwf_plan_dft_r2c_3d(lat->zvoxels,lat->yvoxels,lat->xvoxels,sf,sf_t,FFTW_ESTIMATE);
  fftwf_execute(p);
  fftwf_destroy_plan(p);
  // Perform gamma fft
  p = fftwf_plan_dft_r2c_3d(lat->zvoxels,lat->yvoxels,lat->xvoxels,gamma,gamma_t,FFTW_ESTIMATE);
  fftwf_execute(p);
  fftwf_destroy_plan(p);
  // Divide the transforms element-by-element
  for (i=0;i<tlen;i++) {
      sf_t[i] /= gamma_t[i];
  }
    // Inverse transform
  p = fftwf_plan_dft_c2r_3d(lat->zvoxels,lat->yvoxels,lat->xvoxels,sf_t,sf,FFTW_ESTIMATE);
  fftwf_execute(p);
  fftwf_destroy_plan(p);



  fftwf_free(sf_t);
  fftwf_free(gamma_t);
  CloseShop:
  return(return_value);
#endif
}


