/* LLLMHYBLT.C - Generate a hybrid static disorder + liquid-like motions model lattice.
   
   Author: Mike Wall
   Date: 10/20/2016
   Version: 1.
   
   */

#include<mwmask.h>
#include<complex.h>
#ifdef USE_FFTW
#include<fftw3.h>
#endif

int lllmhyblt(LAT3D *lat1, LAT3D *lat2)
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
    ssqr,
    dwf;

  struct xyzcoords
    r1,r2,r3,rfloat,s,U_times_s;

  float *sf,*gamma;
  fftwf_complex *sf_t,*gamma_t;
  fftwf_plan p;

  latG = (LAT3D *)malloc(sizeof(LAT3D));
  *latG = *lat1;

  latG->lattice = (LATTICE_DATA_TYPE *)malloc(sizeof(LATTICE_DATA_TYPE)
					      *latG->lattice_length);

  latG->origin.i = 0;
  latG->origin.j = 0;
  latG->origin.k = 0;

  float sum=0;
  rscale = (lat1->xscale*lat1->xscale + lat1->yscale*lat1->yscale +
		 lat1->zscale*lat1->zscale);
  for(index.k = 0; index.k < lat1->zvoxels; index.k++) {
    for(index.j = 0; index.j < lat1->yvoxels; index.j++) {
      for (index.i = 0; index.i < lat1->xvoxels; index.i++) {
	latG->index = index;
	rsqr = lssqrFromIndex(latG);
	latG->lattice[lat_index] = (LATTICE_DATA_TYPE)
	  8.*PI*lat1->gamma*lat1->gamma*lat1->gamma/powf((1.+lat1->gamma*lat1->gamma*2.*PI*2.*PI*rsqr),2.);
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

  // Calculate the negative of the static component of the diffuse

  lsublt(lat1,lat2);

  int i;
  for (i=0;i<latG->lattice_length;i++) {
    latG->lattice[i] /= sum;
  }
  size_t tlen = lat1->zvoxels*lat1->yvoxels*(lat1->xvoxels/2+1);
  // Prepare for fftw
  sf = (float *)lat2->lattice;
  gamma = (float *)latG->lattice;
  sf_t = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex)*tlen);
  gamma_t = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex)*tlen);

  // Convert mask tags to zeros in preparation for FFT
  for (i=0;i<lat1->lattice_length;i++) {
    if (sf[i] == (float)lat1->mask_tag) sf[i] = 0;
  }

  // Perform sf fft
  p = fftwf_plan_dft_r2c_3d(lat1->zvoxels,lat1->yvoxels,lat1->xvoxels,sf,sf_t,FFTW_ESTIMATE);
  fftwf_execute(p);
  fftwf_destroy_plan(p);
  // Perform gamma fft
  p = fftwf_plan_dft_r2c_3d(lat1->zvoxels,lat1->yvoxels,lat1->xvoxels,gamma,gamma_t,FFTW_ESTIMATE);
  fftwf_execute(p);
  fftwf_destroy_plan(p);
  // Multiply the transforms element-by-element
    for (i=0;i<tlen;i++) {
    sf_t[i] *= gamma_t[i];
  }
    // Inverse transform
  p = fftwf_plan_dft_c2r_3d(lat1->zvoxels,lat1->yvoxels,lat1->xvoxels,sf_t,sf,FFTW_ESTIMATE);
  fftwf_execute(p);
  fftwf_destroy_plan(p);

  // Multiply by chi-dependent factor
  lat_index = 0;
  for(index.k = 0; index.k < lat1->zvoxels; index.k++) {
    for(index.j = 0; index.j < lat1->yvoxels; index.j++) {
      for (index.i = 0; index.i < lat1->xvoxels; index.i++) {
	lat1->index = index;
	ssqr = lssqrFromIndex(lat1);
	sf[lat_index] *= (LATTICE_DATA_TYPE)(2.*PI*2.*PI*ssqr*lat1->chi*lat1->chi)/((LATTICE_DATA_TYPE)lat1->lattice_length*sqrtf((LATTICE_DATA_TYPE)lat1->lattice_length));
	lat_index++;
      }
    }
  }

  // Add to static component and multiply by Debye-Waller factor
  lat_index = 0;
  for(index.k = 0; index.k < lat1->zvoxels; index.k++) {
    for(index.j = 0; index.j < lat1->yvoxels; index.j++) {
      for (index.i = 0; index.i < lat1->xvoxels; index.i++) {
	lat1->index = index;
	s = lsFromIndex(lat1);
	U_times_s = lmatvecmul(lat1->anisoU,s);	
	dwf = expf(-2.*PI*2.*PI*ldotvec(s,U_times_s));
	if (lat1->lattice[lat_index] != lat1->mask_tag) {
		lat1->lattice[lat_index] = dwf*(sf[lat_index] - lat1->lattice[lat_index]);
	}
		//	lat1->lattice[lat_index] = dwf*sf[lat_index];
	lat_index++;
      }
    }
  }



  fftwf_free(sf_t);
  fftwf_free(gamma_t);
  CloseShop:
  return(return_value);
#endif
}

