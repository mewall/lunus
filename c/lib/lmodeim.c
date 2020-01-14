/* LMODEIM.C - Remove the peaks from an image by mode filetering using
               a specified mask size.
   
   Author: Mike Wall 
   Date: 2/2/95
   Version: 1.
   
   "modeim <image in> <image out> <mask size> <bin size>"

   Input is ascii coordinates file.  Output is 16-bit 
   image of specified size (1024 x 1024 default).

   */

#include<mwmask.h>
#ifdef USE_OPENMP
#include<omp.h>
#endif

int lmodeim(DIFFIMAGE *imdiff_in)
{
  RCCOORDS_DATA 
    half_height,
    half_width,
    n,
    m,
    r, 
    c; 

  IMAGE_DATA_TYPE
    *image,
    maxval,
    minval,
    binsize;
  
  int
    return_value = 0;

  size_t
    *image_mode,
    num_bins;

  DIFFIMAGE *imdiff;

  int pidx;

  size_t
    index,
    i,
    j,
    k,
    *distn;
  
  for (pidx = 0; pidx < imdiff_in->num_panels; pidx++) {
    imdiff = &imdiff_in[pidx];
    if (pidx != imdiff->this_panel) {
      perror("LMODEIM: Image panels are not indexed sequentially. Aborting\n");
      exit(1);
    }

    image = imdiff->image;

    /* 
     * Allocate working mode filetered image: 
     */ 
  
    image_mode = (size_t *)calloc(imdiff->image_length,
				  sizeof(size_t));
    if (!image_mode) {
      sprintf(imdiff->error_msg,"\nLMODEIM:  Couldn't allocate arrays.\n\n");
      return_value = 1;
      goto CloseShop;
    }

    // Compute min and max for image

    int got_first_val = 0;
    
    for (index = 0; index < imdiff->image_length; index++) {
      if ((image[index] != imdiff->overload_tag) &&
	  (image[index] != imdiff->ignore_tag) &&
	  (image[index] < MAX_IMAGE_DATA_VALUE)) {
	if (got_first_val != 0) {
	  if (image[index] < minval) minval = image[index];
	  if (image[index] > maxval) maxval = image[index];
	} else {
	  minval = image[index];
	  maxval = image[index];
	  got_first_val = 1;
	}
      }
    }

    // Allocate the distribution

    binsize = imdiff->mode_binsize;
    num_bins = (size_t)((maxval - minval)/binsize) + 1;

    distn = (size_t *)calloc(num_bins,sizeof(size_t));

    // Compute the mode filtered image

    half_height = imdiff->mode_height / 2;
    half_width = imdiff->mode_width / 2;
    int hpixels = imdiff->hpixels;
    int vpixels = imdiff->vpixels;
    size_t *window;

    size_t wlen = (hpixels+1)*(vpixels+1);
    
    window = (size_t *)calloc(wlen,sizeof(size_t));
    
    for (j = half_height; j < vpixels-half_height; j++) {
      for (i = half_width; i < hpixels-half_width; i++) {
	// Compute the initial j distribution first
	size_t index_mode = j*hpixels + i;
	int l = 0;
	for(n=-half_height; n<=half_height; n++) {
	  r = j + n;
	  for(m=-half_width; m<=half_width; m++) {
	    c = i + m;
	    index = r*hpixels + c;
	    if ((image[index] != imdiff->overload_tag) &&
		(image[index] != imdiff->ignore_tag) &&
		(image[index] < MAX_IMAGE_DATA_VALUE)) {
	      window[l] = (image[index]-minval)/binsize + 1;
	      distn[window[l]]++;
	      l++;
	    }
	  }
	}
	if (l == 0) {
	  image_mode[index_mode] = 0;
	}
	else {
	  int mode_ct = 0;
	  size_t mode_value=0, max_count=0;
	  for (k = 0; k < l; k++) {
	    if (distn[window[k]] == max_count) {
	      mode_value += window[k];
	      mode_ct++;
	    } else if (distn[window[k]] > max_count) {
	      mode_value = window[k];
	      max_count = distn[window[k]];
	      mode_ct = 1;
	    }
	  }
	  if (mode_ct == 0) {
	    printf("Exception\n");
	    exit(1);
	  }
	  image_mode[index_mode] = (size_t)((float)(mode_value/mode_ct) + .5);
	  for (k = 0; k < l; k++) {
	    distn[window[k]] = 0;
	  }
	}
      }
    }
    // Now image_mode holds the mode filtered values
    // Convert these values to pixel values and store them in the input image
    for (j = half_height; j < vpixels - half_height; j++) {
      for (i = half_width; i < hpixels - half_width; i++) {
	size_t this_index = j * hpixels + i;
	if (image_mode[this_index] != 0) {
	  image[this_index] = (image_mode[this_index]-1)*binsize + minval;
	} else {
	  image[this_index] = imdiff->mask_tag;
	}
      }
    }
  }
 CloseShop:
  return(return_value);
}

int lmodeim_old(DIFFIMAGE *imdiff_in) 
{
  
  RCCOORDS_DATA 
    half_height,
    half_width,
    n, 
    m, 
    r, 
    c; 

  size_t
    avg_max_count_count = 0,
    num_max_count_1 = 0,
    index = 0; 
  
  float
    avg_max_count = 0;

  IMAGE_DATA_TYPE
    *image;

  int
    return_value = 0;

  int
    pidx,
    nt;
  
  DIFFIMAGE *imdiff;

  for (pidx = 0; pidx < imdiff_in->num_panels; pidx++) {
    imdiff = &imdiff_in[pidx];
    if (pidx != imdiff->this_panel) {
      perror("LMODEIM: Image panels are not indexed sequentially. Aborting\n");
      exit(1);
    }

    /* 
     * Allocate working image: 
     */ 
  
    image = (IMAGE_DATA_TYPE *)calloc(imdiff->image_length, 
				      sizeof(IMAGE_DATA_TYPE)); 
    if (!image) {
      sprintf(imdiff->error_msg,"\nLMODEIM:  Couldn't allocate arrays.\n\n");
      return_value = 1;
      goto CloseShop;
    }


    half_height = imdiff->mode_height / 2;
    half_width = imdiff->mode_width / 2;
    index = 0;

    size_t j;

#ifdef USE_OPENMP
    //  omp_set_num_threads(16);
    //  nt = omp_get_max_threads();
    //  printf("Using OpenMP with %d threads\n",nt);
    {
      //    #pragma omp for schedule(dynamic,1)
      //#pragma omp parallel for shared(imdiff,image,half_height,half_width) private(j) reduction(+:avg_max_count,avg_max_count_count,num_max_count_1)
#pragma omp parallel for shared(imdiff,image,half_height,half_width) private(j)
#endif
      for (j=0; j<imdiff->vpixels; j++) {
	size_t i;
	size_t *count;
	count = (size_t *)calloc(MAX_IMAGE_DATA_VALUE,sizeof(size_t));
	unsigned int *count_pointer;
	count_pointer = (unsigned int *)calloc((imdiff->mode_height+1) *
					       (imdiff->mode_width+1), 
					       sizeof(unsigned int));
	for (i=0; i<imdiff->hpixels; i++) {
	  long mode_value=0;
	  size_t max_count=0;
	  size_t mode_ct=1;
	  size_t l=0;
	  RCCOORDS_DATA n,m,r,c;
	  size_t k;
	  size_t index;
	  index = j*imdiff->hpixels+i;
	  if (imdiff->image[index] != imdiff->ignore_tag && imdiff->image[index] != imdiff->overload_tag) {
	    for(n=-half_height; n<=half_height; n++) {
	      r = j + n;
	      for(m=-half_width; m<=half_width; m++) {
		c = i + m;
		if ((r >= 0) && (r < imdiff->vpixels) && (c >= 0) &&       
		    (c < imdiff->hpixels)) {
		  size_t imd_index;
		  imd_index = index + n*imdiff->hpixels + m;
		  if ((imdiff->image[imd_index] != imdiff->overload_tag) &&
		      (imdiff->image[imd_index] != imdiff->ignore_tag) &&
		      (imdiff->image[imd_index] < MAX_IMAGE_DATA_VALUE) &&
		      (imdiff->image[imd_index] >= 0)) {
		    //		count_pointer[l]=(imdiff->image[imd_index] - 
		    //				    (imdiff->image[imd_index] % 
		    //				     imdiff->mode_binsize) + 32768);		
		    count_pointer[l]=(unsigned int)imdiff->image[imd_index];
		    //		l++;
		    count[count_pointer[l]]++;
		    l++;
		  }
		}
	      }
	    }
	    if (l==0) {
	      /*	printf("\nl=0 index=%ld\n",index);/***/
	      image[index]=imdiff->ignore_tag;
	    }
	    else {
	      //	  for(k=0;k<l;k++) {
	      //	    count[count_pointer[k]]++;
	      //	  }
	      for(k=0;k<l;k++) {
		if (count[count_pointer[k]] == max_count) {
		  mode_value += count_pointer[k];
		  mode_ct++;
		}
		else if (count[count_pointer[k]] > max_count) {
		  mode_value = count_pointer[k];
		  max_count = count[count_pointer[k]];
		  mode_ct = 1;
		}
	      }
	      for(k=0;k<l;k++) count[count_pointer[k]] = 0;
	      image[index] = (IMAGE_DATA_TYPE)((float)mode_value/(float)mode_ct);
	      //	  avg_max_count += max_count;
	      //	  avg_max_count = (avg_max_count*avg_max_count_count +
	      // max_count) / (float)(avg_max_count_count + 1);
	      //	  if (max_count == 1) {num_max_count_1++;}
	      //	  avg_max_count_count++;
	    }
	  }
	  else {
	    image[index] = imdiff->image[index];
	  }
	  //      index++;
	  //      if (index % 1000 == 0) printf("%d\n",index);
	}
	free(count);
	free(count_pointer);
      }
#ifdef USE_OPENMP
    }
#endif
    for(index=0;index<imdiff->image_length; index++) {
      imdiff->image[index] = image[index];
    }
    //avg_max_count /= (float)avg_max_count_count;
    //  printf("avg_max_count,num_max_count_1 = %f,%d\n\n",avg_max_count,num_max_count_1);/***/
    free((IMAGE_DATA_TYPE *)image);/***/
  } // for(pidx)

 CloseShop:
  return(return_value);
}







