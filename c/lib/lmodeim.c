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
#include<time.h>

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
    *distn,
    *window;
  
  for (pidx = 0; pidx < imdiff_in->num_panels; pidx++) {
    imdiff = &imdiff_in[pidx];
    if (pidx != imdiff->this_panel) {
      perror("LMODEIM: Image panels are not indexed sequentially. Aborting\n");
      exit(1);
    }

    image = imdiff->image;
    IMAGE_DATA_TYPE overload_tag = imdiff->overload_tag;
    IMAGE_DATA_TYPE ignore_tag = imdiff->ignore_tag;
    size_t image_length = imdiff->image_length;
    
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
    
    
    for (index = 0; index < image_length; index++) {
      if ((image[index] != overload_tag) &&
	  (image[index] != ignore_tag) &&
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
    num_bins = (size_t)((maxval - minval)/binsize) + 2;

    // Compute the mode filtered image

    half_height = imdiff->mode_height / 2;
    half_width = imdiff->mode_width / 2;
    int hpixels = imdiff->hpixels;
    int vpixels = imdiff->vpixels;

    size_t wlen = (hpixels+1)*(vpixels+1);
    

#pragma omp target enter data map(alloc:image[0:image_length],image_mode[0:image_length])
#pragma omp target update to(image[0:image_length],image_mode[0:image_length])
    //#pragma omp target data map (to:distn[0:num_bins])
    size_t num_teams[1];
    size_t num_threads[1];
#pragma omp target enter data map(alloc:num_teams,num_threads)
#pragma omp target update to(num_teams,num_threads)
#pragma omp target teams distribute parallel for collapse(2) private(i)
    for (j = half_height; j < vpixels - half_height; j++) {
      for (i = half_width; i < hpixels - half_width; i++) {
	num_teams[0] = omp_get_num_teams();
	num_threads[0] = omp_get_num_threads();
      }
    }
#pragma omp target update from(num_teams,num_threads)
    printf(" Number of teams, threads = %ld, %ld\n",num_teams[0],num_threads[0]);
    size_t team_num[num_teams[0]];
    size_t thread_num[num_teams[0]*num_threads[0]];
#pragma omp target enter data map(alloc:team_num[:num_teams[0]],thread_num[:num_teams[0]*num_threads[0]])
#pragma omp target teams distribute parallel for collapse(2)
    for (j = half_height; j < vpixels - half_height; j++) {
      for (i = half_width; i < hpixels - half_width; i++) {
	size_t tm = omp_get_team_num();
	size_t th = omp_get_thread_num();
	team_num[tm] = tm;
	thread_num[tm*num_threads[0]+th] = omp_get_thread_num();
	image_mode[j*hpixels+i] = tm*num_threads[0]+th;
      }
    }
#pragma omp target update from(team_num[:num_teams[0]],thread_num[:num_teams[0]*num_threads[0]],image_mode[:image_length])
    /*
    for (k = 0;k < num_teams[0];k++) {
      printf("Team number %ld:\n", team_num[k]);
      for (j = 0; j < num_threads[0];j++) {
	printf("  Thread number %ld\n",thread_num[k*num_threads[0]+j]);
      }
    }
    for (j = half_height; j < vpixels - half_height; j++) {
      for (i = half_width; i < hpixels - half_width; i++) {
	printf("Image index = %ld, thread index = %ld\n",j*hpixels+i,image_mode[j*hpixels+i]);
      }
    }
    fflush(stdout);
    */
#pragma omp target exit data map(delete:team_num[:num_teams[0]],thread_num[:num_teams[0]*num_threads[0]])
#pragma omp target exit data map(delete:num_teams,num_threads)

    window = (size_t *)calloc(wlen*num_teams[0]*num_threads[0],sizeof(size_t));
    distn = (size_t *)calloc(num_bins*num_teams[0]*num_threads[0],sizeof(size_t));

#pragma omp target enter data map(alloc:window[0:wlen*num_teams[0]*num_threads[0]],distn[0:num_bins*num_teams[0]*num_threads[0]])
#pragma omp target update to(window[0:wlen*num_teams[0]*num_threads[0]],distn[0:num_bins*num_teams[0]*num_threads[0]])

    clock_t start = clock();

#pragma omp target teams distribute parallel for collapse(2)	\
  private(i,k,r,c,index)
    //    for (j = half_height; j < vpixels - half_height; j++) {
    //      for (i = half_width; i < hpixels - half_width; i++) {
    for (j = half_height; j < 400-half_height; j++) {
      for (i = half_width; i < 400-half_width; i++) {
	size_t index_mode = j*hpixels + i;
	int l = 0;
	size_t tm = omp_get_team_num();
	size_t th = omp_get_thread_num();
	size_t nt = omp_get_num_threads();
	size_t *this_window = &window[(tm*nt+th)*wlen];
	size_t *this_distn = &distn[(tm*nt+th)*num_bins];
	for (k = 0; k < wlen; k++) this_window[k] = 0;
	for (k = 0; k < num_bins; k++) this_distn[k] = 0;
	// Compute the initial j distribution first
	for(r = j - half_height; r <= j + half_height; r++) {
	  //	  printf("r = %d\n",r);
	  for(c = i - half_width; c <= i + half_width; c++) {
	    index = r*hpixels + c;
	    if ((image[index] != overload_tag) &&
		(image[index] != ignore_tag) &&
		(image[index] < MAX_IMAGE_DATA_VALUE)) {
	      this_window[l] = (image[index]-minval)/binsize + 1;
	      this_distn[this_window[l]]++;
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
	    if (this_distn[this_window[k]] == max_count) {
	      mode_value += this_window[k];
	      mode_ct++;
	    } else if (this_distn[this_window[k]] > max_count) {
	      mode_value = this_window[k];
	      max_count = this_distn[this_window[k]];
	      mode_ct = 1;
	    }
	  }
	  image_mode[index_mode] = (size_t)((float)(mode_value/mode_ct) + .5);
	}
      }
    }
    // Now image_mode holds the mode filtered values
    // Convert these values to pixel values and store them in the input image

    clock_t stop = clock();
    double tel = ((double)(stop-start))/CLOCKS_PER_SEC;
    printf("kernel loop took %g seconds\n",tel);
#pragma omp target
    //parallel for				\
//  shared(half_height,half_width,binsize,minval)	\
//  private(i)
    {    
    for (j = half_height; j < vpixels - half_height; j++) {
      for (i = half_width; i < hpixels - half_width; i++) {
	size_t this_index = j * hpixels + i;
	if (image_mode[this_index] != 0) {
	  image[this_index] = (image_mode[this_index]-1)*binsize + minval;
	} else {
	  image[this_index] = ignore_tag;
	}
      }
    }
    }
#pragma omp target update from(image[0:image_length])
#pragma omp target exit data map(delete:image[0:image_length],image_mode[0:image_length])
#pragma omp target exit data map(delete:image[0:image_length],image_mode[0:image_length],window[0:wlen*num_threads[0]*num_teams[0]],distn[0:num_bins*num_teams[0]*num_threads[0]])
    free(image_mode);
    //    free(distn);
    //    free(window);
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
    free(image);/***/
  } // for(pidx)

 CloseShop:
  return(return_value);
}







