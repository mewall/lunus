/* LLLMLT.C - Generate a liquid-like motions model lattice.
   
   Author: Mike Wall
   Date: 9/26/2016
   Version: 1.
   
   */

#include<mwmask.h>
#include<complex.h>
#ifdef USE_FFTW
#include<fftw3.h>
#endif
int lllmnlt(LAT3D *lat)
{
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

  int n = lat->llm_order;

  //  printf("LLMLT: Order %d calculation\n",n);
  
  float this_gamma = lat->gamma / (float)n;
  
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
	  //          sqrtf(PI/2.)*lat->gamma*expf(-2.*PI*2.*PI*rsqr*lat->gamma*lat->gamma);
		  8.*PI*this_gamma*this_gamma*this_gamma/powf((1.+this_gamma*this_gamma*2.*PI*2.*PI*rsqr),2.);
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
  //  for (i=0;i<latG->lattice_length;i++) {
  //    latG->lattice[i] /= sum;
  //  }
  size_t tlen = lat->zvoxels*lat->yvoxels*(lat->xvoxels/2+1);
  // Prepare for fftw
  sf = (float *)lat->lattice;
  gamma = (float *)latG->lattice;
  sf_t = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex)*tlen);
  gamma_t = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex)*tlen);

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
  // Multiply the transforms element-by-element
    for (i=0;i<tlen;i++) {
    sf_t[i] *= gamma_t[i];
  }
    // Inverse transform
  p = fftwf_plan_dft_c2r_3d(lat->zvoxels,lat->yvoxels,lat->xvoxels,sf_t,sf,FFTW_ESTIMATE);
  fftwf_execute(p);
  fftwf_destroy_plan(p);

  lat_index = 0;
  size_t nfac = 1;
  for (i=1;i<=lat->llm_order;i++) {
    nfac *= i;
  }
  
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
	//	sf[lat_index] *= (LATTICE_DATA_TYPE)powf(exparg,n)/(float)nfac/((LATTICE_DATA_TYPE)lat->lattice_length*sqrtf((LATTICE_DATA_TYPE)lat->lattice_length));
	sf[lat_index] *= (LATTICE_DATA_TYPE)dwf*powf(exparg,n)/(float)nfac/((LATTICE_DATA_TYPE)lat->lattice_length*sqrtf((LATTICE_DATA_TYPE)lat->lattice_length));
	//	sf[lat_index] *= (LATTICE_DATA_TYPE)exparg*dwf/((LATTICE_DATA_TYPE)lat->lattice_length*sqrtf((LATTICE_DATA_TYPE)lat->lattice_length));
	lat_index++;
      }
    }
  }


  fftwf_free(sf_t);
  fftwf_free(gamma_t);
  free(latG->lattice);

  //  for (i=5000;i<5010;i++) printf("lat->lattice[%ld] = %g\n",i,lat->lattice[i]);
  
 CloseShop:
  return(return_value);
}

int lllmlt(LAT3D *lat)
{
#ifdef USE_FFTW
  size_t
    lat_index = 0;
  
  int
    return_value = 0;

  LAT3D *latn,*latsum;

  size_t i;
  int j;

  latn = (LAT3D *)malloc(sizeof(LAT3D));
  *latn = *lat;

  latn->lattice = (LATTICE_DATA_TYPE *)malloc(sizeof(LATTICE_DATA_TYPE)
					      *latn->lattice_length);

  latsum = (LAT3D *)malloc(sizeof(LAT3D));
  *latsum = *lat;

  latsum->lattice = (LATTICE_DATA_TYPE *)malloc(sizeof(LATTICE_DATA_TYPE)
					      *latsum->lattice_length);

  for (i = 0;i < lat->lattice_length; i++)
    if (lat->lattice[i] != lat->mask_tag)
      latsum->lattice[i] = 0.0;

  for (j = 1;j<=lat->llm_order;j++) {
    latn->llm_order = j;
    for (i = 0; i < lat->lattice_length; i++)
      latn->lattice[i] = lat->lattice[i];
    lllmnlt(latn);
    for (i = 0; i < lat->lattice_length; i++)
      if (lat->lattice[i] != lat->mask_tag)
	latsum->lattice[i] += latn->lattice[i];
  }

  for (i = 0;i < lat->lattice_length; i++)
    if (lat->lattice[i] != lat->mask_tag)
      lat->lattice[i] = latsum->lattice[i];

 CloseShop:
  return(return_value);
#else
  perror("LLMLT: Unable to calculate liquid-like motions model. \n  Must build using FFTW with CFLAGS += -DUSE_FFTW\n");
  exit(1);
#endif
}


