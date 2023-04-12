/* LCCRLT.C - Calculate the Pearson correlation coefficient
               vs.  radius between two input lattices in radial shells
   
   Author: Mike Wall
   Date: 1/22/2013
   Version: 1.
   
   */

#include<mwmask.h>

int lccrlt(LAT3D *lat1, LAT3D *lat2)
{
  size_t
    *ct,
    i,
    j,
    k,
    r,
    index = 0;

  int
    return_value = 0;
  
  struct ijkcoords rvec;

  RFILE_DATA_TYPE *avg1, *avg2, *s11, *s22, *s12;

  float
    rscale,
    diffn;

  struct xyzcoords
    rfloat;

  /*
   * Allocate arrays:
   */

  ct = (size_t *)calloc(MAX_RFILE_LENGTH,sizeof(size_t));
  avg1 = (RFILE_DATA_TYPE *)calloc(MAX_RFILE_LENGTH,sizeof(RFILE_DATA_TYPE));
  avg2 = (RFILE_DATA_TYPE *)calloc(MAX_RFILE_LENGTH,sizeof(RFILE_DATA_TYPE));
  s11 = (RFILE_DATA_TYPE *)calloc(MAX_RFILE_LENGTH,sizeof(RFILE_DATA_TYPE));
  s22 = (RFILE_DATA_TYPE *)calloc(MAX_RFILE_LENGTH,sizeof(RFILE_DATA_TYPE));
  s12 = (RFILE_DATA_TYPE *)calloc(MAX_RFILE_LENGTH,sizeof(RFILE_DATA_TYPE));
  if (!ct||!avg1||!avg2) {
    sprintf(lat1->error_msg,"\nLCCRLT: Couldn't allocate "
	    "arrays.\n\n");
    return_value = 1;
    goto CloseShop;
  }

  rscale = ldotvec(lat1->cellstardiag,lat1->cellstardiag);
  printf("scattering vector scale = %f\n",sqrtf(rscale));
  //  rscale = (lat1->xscale*lat1->xscale + lat1->yscale*lat1->yscale +
  //		 lat1->zscale*lat1->zscale);
  lat1->rfile_length = 0;
  // Calculate means
  for(k = 0; k < lat1->zvoxels; k++) {
    for(j = 0; j < lat1->yvoxels; j++) {
      for (i = 0; i < lat1->xvoxels; i++) {
	lat1->index.i = i;
	lat1->index.j = j;
	lat1->index.k = k;
	/* rvec.i = i - lat1->origin.i; */
	/* rvec.j = j - lat1->origin.j; */
	/* rvec.k = k - lat1->origin.k; */
	/* rfloat.x = lat1->xscale * rvec.i; */
	/* rfloat.y = lat1->yscale * rvec.j; */
	/* rfloat.z = lat1->zscale * rvec.k; */
	/*	r = (size_t)(sqrtf((rfloat.x*rfloat.x + rfloat.y*rfloat.y + 
		rfloat.z*rfloat.z) / rscale)+.5); */
	r = (size_t)(sqrtf(lssqrFromIndex(lat1)/rscale)+0.5);
	if (r > lat1->rfile_length) lat1->rfile_length = r;
	if ((lat1->lattice[index] != lat1->mask_tag) &&
	    (lat2->lattice[index] != lat1->mask_tag)) {
	  avg1[r] += lat1->lattice[index];
	  avg2[r] += lat2->lattice[index];
	  ct[r]++;
/*printf("%f %f\n",lat1->lattice[index],lat2->lattice[index]);/***/
	}
	index++;
      }
    }
  }

  

  for (i=0;i<=lat1->rfile_length;i++) {
    if (ct[i]>0) {
      avg1[i] /= (float)ct[i];
      avg2[i] /= (float)ct[i];
    }
  }

  index=0;
  for(k = 0; k < lat1->zvoxels; k++) {
    for(j = 0; j < lat1->yvoxels; j++) {
      for (i = 0; i < lat1->xvoxels; i++) {
	rvec.i = i - lat1->origin.i;
	rvec.j = j - lat1->origin.j;
	rvec.k = k - lat1->origin.k;
	rfloat.x = lat1->xscale * rvec.i;
	rfloat.y = lat1->yscale * rvec.j;
	rfloat.z = lat1->zscale * rvec.k;
	r = (size_t)(sqrtf((rfloat.x*rfloat.x + rfloat.y*rfloat.y + 
		       rfloat.z*rfloat.z) / rscale)+.5);
	if (r > lat1->rfile_length) lat1->rfile_length = r;
	if ((lat1->lattice[index] != lat1->mask_tag) &&
	    (lat2->lattice[index] != lat1->mask_tag) && (ct[r]>0)) {
	  RFILE_DATA_TYPE d1 = lat1->lattice[index] - avg1[r];
	  RFILE_DATA_TYPE d2 = lat2->lattice[index] - avg2[r];
/*printf("%f %f\n",lat1->lattice[index],lat2->lattice[index]);/***/
	  s11[r] += d1*d1;
	  s22[r] += d2*d2;
	  s12[r] += d1*d2;
	}
	index++;
      }
    }
  }

  for (i=1;i<=lat1->rfile_length;i++) {
    if (ct[i]>0) {
      lat1->rfile[i] = s12[i]/sqrtf(s11[i])/sqrtf(s22[i]);
    }
  }

  float cumulative_corr=0.0f;
  size_t cumulative_ct=0;

  size_t last_nonzero_index;

  for (i=0;i<=lat1->rfile_length;i++) {
    if (ct[i]>0) {
      last_nonzero_index = i;
      cumulative_corr += ((float)ct[i])*lat1->rfile[i];
      cumulative_ct += ct[i];
      printf("cumulative_ct=%d,ct[%d]=%d, corr=%f\n",cumulative_ct,i,ct[i],lat1->rfile[i]);
    }
  }

  lat1->rfile_length = last_nonzero_index+1;

  cumulative_corr /= (float)cumulative_ct;

  printf("Cumulative C = %e,cumulative_ct=%d\n",cumulative_corr,cumulative_ct);



  free((size_t *)ct);
  free(avg1);
  free(avg2);
  free(s11);
  free(s22);
  free(s12);
  CloseShop:
  return(return_value);
}





