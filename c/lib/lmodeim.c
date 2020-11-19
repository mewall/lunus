/* LMODEIM.C - Remove the peaks from an image by mode filetering using
               a specified mask size.
   
   Author: Mike Wall 
   Date: 2/2/95
   Version: 1.
   
   "modeim <image in> <image out> <mask size> <bin size>"

   Input is ascii coordinates file.  Output is 16-bit 
   image of specified size (1024 x 1024 default).

   Reentry codes (imdiff->reentry):
     0 = Enter fresh without intent for future reentry (initialize and free)
     1 = Enter fresh with intent for future reentry (initialize without free)
     2 = Reentry with intent for future reentry (no initialize, no free)
     3 = Reentry without intent for future reentry (no initialize, but do free)
   */

#include<mwmask.h>
#ifdef USE_OPENMP
#include<omp.h>
#endif
#include<time.h>

#ifdef USE_OFFLOAD
#pragma omp declare target
#endif

void swap(size_t *a, size_t * b) 
{ 
    int t = *a; 
    *a = *b; 
    *b = t; 
} 
  
int partition(size_t arr[], int l, int h) 
{ 
    size_t x = arr[h]; 
    int i = (l - 1); 
    int j;

    for (j = l; j <= h - 1; j++) { 
        if (arr[j] <= x) { 
            i++; 
            swap(&arr[i], &arr[j]); 
        } 
    } 
    swap(&arr[i + 1], &arr[h]); 
    return (i + 1); 
} 

void quickSortIterative(size_t arr[], size_t stack[], int l, int h) 
{ 
    // initialize top of stack 
    int top = -1; 
  
    // push initial values of l and h to stack 
    stack[++top] = l; 
    stack[++top] = h; 
  
    // Keep popping from stack while is not empty 
    while (top >= 0) { 
        // Pop h and l 
        h = stack[top--]; 
        l = stack[top--]; 
  
        // Set pivot element at its correct position 
        // in sorted array 
        int p;
        p = partition(arr, l, h); 
  
        // If there are elements on left side of pivot, 
        // then push left side to stack 
        if (p - 1 > l) { 
            stack[++top] = l; 
            stack[++top] = p - 1; 
        } 
  
        // If there are elements on right side of pivot, 
        // then push right side to stack 
        if (p + 1 < h) { 
            stack[++top] = p + 1; 
            stack[++top] = h; 
        } 
    } 
}

void insertion_sort(size_t *a,int first,int last) {
    int len;
    int i=1+first;
    len = last - first + 1;
    while (i < len) {
        size_t x = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > x) {
            a[j + 1] = a[j];
            j = j - 1;
        }
        a[j+1] = x;
        i = i + 1;
    }
}

void quicksort(size_t *a,int first,int last){
   int i, j, pivot;
   size_t temp;

   if(first<last){
      pivot=first;
      i=first;
      j=last;

      while(i<j){
         while(a[i]<=a[pivot]&&i<last)
            i++;
         while(a[j]>a[pivot])
            j--;
         if(i<j){
	   temp=a[i];
	   a[i]=a[j];
	   a[j]=temp;
         }
      }

      temp=a[pivot];
      a[pivot]=a[j];
      a[j]=temp;
      quicksort(a,first,j-1);
      quicksort(a,j+1,last);

   }
}
#ifdef USE_OFFLOAD
#pragma omp end declare target
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

  static IMAGE_DATA_TYPE
    *image = NULL;

  IMAGE_DATA_TYPE
    maxval,
    minval,
    binsize;
  
  int
    return_value = 0;

  static size_t
    *image_mode = NULL,
    *window = NULL,
    *stack = NULL;

  DIFFIMAGE *imdiff;

  int reentry = imdiff_in->reentry;

  size_t wlen = (imdiff_in->mode_height+1)*(imdiff_in->mode_width+1);
    
  int pidx;

  size_t
    num_teams,
    num_threads,
    index,
    i,
    j,
    k;

  static size_t 
    image_length = 0,
    hpixels = 0,
    vpixels = 0;

  imdiff = &imdiff_in[0];

  if (reentry == 2 || reentry == 3) {
    if (hpixels != imdiff->hpixels || vpixels != imdiff->vpixels) {
      perror("LMODEIM: Image panel size changed on reentry. Aborting\n");
      exit(1);
    }
  }

  half_height = imdiff->mode_height / 2;
  half_width = imdiff->mode_width / 2;
  image_length = imdiff->image_length;
  hpixels = imdiff->hpixels;
  vpixels = imdiff->vpixels;

#ifdef USE_OPENMP
#ifdef USE_OFFLOAD
#pragma omp target teams distribute map(from:num_teams,num_threads)
#else
#pragma omp distribute
#endif
#endif

  for (j = half_height; j < vpixels - half_height; j++) {
    num_teams = omp_get_num_teams();

#ifdef USE_OPENMP
#ifdef USE_OFFLOAD
#pragma omp parallel num_threads(32)
#else
#pragma omp parallel
#endif
#endif

    {
      num_threads = omp_get_num_threads();
    }
  }
    
  printf(" Number of teams, threads = %ld, %ld\n",num_teams,num_threads);

  if (reentry == 0 || reentry == 1) {

    image = (IMAGE_DATA_TYPE *)calloc(image_length,sizeof(IMAGE_DATA_TYPE));
    image_mode = (size_t *)calloc(image_length,sizeof(size_t));
    window = (size_t *)calloc(wlen*num_teams*num_threads,sizeof(size_t));
    stack = (size_t *)calloc(wlen*num_teams*num_threads,sizeof(size_t));

    /* 
     * Allocate working mode filetered image: 
     */ 
  
    if (!image || !image_mode || !window || !stack) {
      sprintf(imdiff->error_msg,"\nLMODEIM:  Couldn't allocate arrays.\n\n");
      return_value = 1;
      goto CloseShop;
    }

#ifdef USE_OFFLOAD
#pragma omp target enter data map(alloc:image[0:image_length],image_mode[0:image_length])
#pragma omp target enter data map(alloc:window[0:wlen*num_teams*num_threads],stack[0:wlen*num_teams*num_threads])
#endif

  } 

  for (pidx = 0; pidx < imdiff_in->num_panels; pidx++) {
    imdiff = &imdiff_in[pidx];
    if (pidx != imdiff->this_panel) {
      perror("LMODEIM: Image panels are not indexed sequentially. Aborting\n");
      exit(1);
    }
    if (hpixels != imdiff->hpixels || vpixels != imdiff->vpixels) {
      perror("LMODEIM: Image panels are not identically formatted. Aborting\n");
      exit(1);
    }

    memcpy(image, imdiff->image, image_length*sizeof(IMAGE_DATA_TYPE));

    IMAGE_DATA_TYPE overload_tag = imdiff->overload_tag;
    IMAGE_DATA_TYPE ignore_tag = imdiff->ignore_tag;
    

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

    // Compute the mode filtered image


#ifdef USE_OFFLOAD
#pragma omp target update to(image[0:image_length],image_mode[0:image_length])
    //#pragma omp target update to(window[0:wlen*num_teams*num_threads],stack[0:wlen*num_teams*num_threads])
#endif

    //    printf("Entering omp offload region\n");
    //    fflush(stdout);
    
#ifdef USE_OPENMP
    double start = omp_get_wtime();
#else
    double start = ((double)clock())/CLOCKS_PER_SEC;
#endif

#ifdef USE_OPENMP
#ifdef USE_OFFLOAD
#pragma omp target map(to:minval,binsize,wlen,overload_tag,ignore_tag,num_teams,num_threads) 
#pragma omp teams
#pragma omp distribute
#else
#pragma omp distribute
#endif
#endif

    for (j = half_height; j < vpixels-half_height; j++) {
//      printf("Team = %ld, j = %d\n",tm,j);
#ifdef USE_OPENMP
#ifdef USE_OFFLOAD
#pragma omp parallel for private(index,k,r,c) schedule(static,1) num_threads(32)
#else
#pragma omp parallel for private(index,k,r,c)
#endif
#endif

      for (i = half_width; i < hpixels-half_width; i++) {
	int mode_ct = 0;
	size_t mode_value=0, max_count=0;
	size_t index_mode = j*hpixels + i;
	size_t tm = omp_get_team_num();
	size_t th = omp_get_thread_num();
	size_t nt = omp_get_num_threads();
	size_t ntm = omp_get_num_teams();
	size_t this_value = (image[index_mode]-minval)/binsize + 1;
	if (ntm != num_teams || nt != num_threads) {
	  printf("Number of teams, threads %ld, %ld differs from assumed numbers %ld, %ld\n",ntm,nt,num_teams,num_threads);
	  exit(1);
	}
	size_t *this_window = &window[(tm*nt+th)*wlen];
	size_t *this_stack = &stack[(tm*nt+th)*wlen];
	for (k = 0; k < wlen; k++) {
	  this_window[k] = 0;
	}
	int l = 0;
//        printf("Start tm = %ld,th = %ld,i = %d,j = %ld\n",tm,th,i,index_mode/hpixels);
	for (r = j - half_height; r <= j + half_height; r++) {
	  for (c = i - half_width; c <= i + half_width; c++) {
	    index = r*hpixels + c;
	    if ((image[index] != overload_tag) &&
		(image[index] != ignore_tag) &&
		(image[index] < MAX_IMAGE_DATA_VALUE)) {
	      this_window[l] = (image[index]-minval)/binsize + 1;
	      l++;
	    }
	  }
	}
	if (l == 0 || image[index_mode] == ignore_tag || image[index_mode] == overload_tag || image[index_mode] >= MAX_IMAGE_DATA_VALUE) {
	  image_mode[index_mode] = 0;
	}
	else {
//          printf("Starting quicksort for i=%d,j=%ld\n",i,index_mode/hpixels);
//	  insertion_sort(this_window,0,l-1);
//	  quicksort(this_window,0,l-1);
	  quickSortIterative(this_window,this_stack,0,l-1);
//          printf("Done with quicksort for i=%d,j=%ld\n",i,index_mode/hpixels);
	  // Get the median
	  int kmed = l/2;
	  int k90 = l*9/10;
	  size_t median_value = this_window[kmed];
	  size_t med90_value = this_window[k90];

	  // Get the mode
	  size_t this_count = 1;
          size_t last_value = this_window[0];
          max_count = 1;
	  for (k = 1; k < l; k++) {
            if (this_window[k] == last_value) {
              this_count++;
            } else {
              last_value = this_window[k];
              this_count = 1;
            }
            if (this_count > max_count) max_count = this_count;
	  }
          this_count = 1;
          last_value = this_window[0];
	  double entropy = 0.0;
	  for (k = 1; k < l; k++) {
	    if (this_window[k] == last_value) {
              this_count++;
            } else {
	      double p = (double)this_count/(double)l;
	      entropy -=  p * log(p);
              last_value = this_window[k];
              this_count = 1;
            }
            if (this_count == max_count) {
	      mode_value += this_window[k];
	      mode_ct++;
	    }
	  }
	  double p = (double)this_count/(double)l;
	  entropy -=  p * log(p);
	  //	  image_mode[index_mode] = (size_t)(((float)mode_value/(float)mode_ct) + .5);
	  if (entropy > log(10.)) {
	    if (mode_ct == 1) {
	      mode_value = median_value;
	    }
	  } else {
	    if (this_value < med90_value) {
	      mode_value = this_value;
	    } else {
	      mode_value = this_window[(size_t)(((double)k90*(double)rand())/(double)RAND_MAX)];
	      //	      printf("%d %ld %ld\n",kmed,mode_value,median_value);
	      //	      mode_value = median_value;
	    }
	  }
	  image_mode[index_mode] = (size_t)(((float)mode_value/(float)mode_ct) + .5);
	}
//        printf("Stop tm = %ld,th = %ld,i = %d,j = %d\n",tm,th,i,j);
      }
    }

    // Now image_mode holds the mode filtered values
    // Convert these values to pixel values and store them in the input image

#ifdef USE_OPENMP
    double stop = omp_get_wtime();
#else
    double stop = ((double)clock())/CLOCKS_PER_SEC;
#endif
    double tel = stop-start;
    fflush(stdout);

#ifdef DEBUG
    printf("kernel loop took %g seconds\n",tel);
#endif
    
#ifdef USE_OPENMP
#ifdef USE_OFFLOAD
#pragma omp target teams distribute map(to:minval,binsize,ignore_tag)
#else
#pragma omp distribute
#endif
#endif

    for (j = 0; j < vpixels; j++) {
      if (j < half_height || j > (vpixels-half_height)) {
	for (i = 0; i < hpixels; i++) {
	  size_t this_index = j * hpixels + i;
	  image[this_index] = ignore_tag;
	} 
      } else {

	for (i = 0; i < half_width; i++) {
	  size_t this_index = j * hpixels + i;
	  image[this_index] = ignore_tag;
	}
	
	for (i = hpixels - half_width; i < hpixels; i++) {
	  size_t this_index = j * hpixels + i;
	  image[this_index] = ignore_tag;
	}

#ifdef USE_OPENMP
#ifdef USE_OFFLOAD
#pragma omp parallel for schedule(static,1) num_threads(32)
#else
#pragma omp parallel for
#endif
#endif

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
	  
#ifdef USE_OFFLOAD
#pragma omp target update from(image[0:image_length]) 
#endif
    memcpy(imdiff->image,image,image_length*sizeof(IMAGE_DATA_TYPE));

    if (reentry == 0 || reentry == 3) {
#ifdef USE_OFFLOAD
#pragma omp target exit data map(delete:image[0:image_length],image_mode[0:image_length],window[0:wlen*num_threads*num_teams],stack[0:wlen*num_threads*num_teams])
#endif
      free(image);
      free(image_mode);
      free(window);
      free(stack);
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







