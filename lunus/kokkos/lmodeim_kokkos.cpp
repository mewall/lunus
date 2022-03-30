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

#include <KokkosKernels_Sorting.hpp>
#include <cstdlib>

extern "C" {
#include<mwmask.h>
#ifdef USE_OPENMP
#include<omp.h>
#endif
#include<time.h>

void kokkos_start ()
{
  Kokkos::initialize();
}

void kokkos_stop() {
  Kokkos::finalize();
}

}

template<typename ValView, typename OrdView>
struct TestTeamBitonicFunctor
{
  typedef typename ValView::value_type Value;

  TestTeamBitonicFunctor(ValView& values_, OrdView& counts_, OrdView& offsets_)
    : values(values_), counts(counts_), offsets(offsets_)
  {}

  template<typename TeamMem>
  KOKKOS_INLINE_FUNCTION void operator()(const TeamMem t) const
  {
    int i = t.league_rank();
    KokkosKernels::TeamBitonicSort<int, Value, TeamMem>(values.data() + offsets(i), counts(i), t);
  }

  ValView values;
  OrdView counts;
  OrdView offsets;
};


template<typename ValView>
void quickSortListKokkosDeviceArrays(ValView& d_values, size_t num_arrays, size_t array_size)
{
  size_t i, j;

  size_t num_values = num_arrays * array_size;

  typedef Kokkos::View<size_t *> OrdView;

#ifdef DEBUG
  printf("quickSortListKokkosDeviceArrays: Creating views\n");
#endif
  OrdView d_offsets("d_offsets",num_arrays);
  Kokkos::View<size_t *>::HostMirror h_offsets = Kokkos::create_mirror_view(d_offsets);
  OrdView d_counts("d_counts",num_arrays);
  Kokkos::View<size_t *>::HostMirror h_counts = Kokkos::create_mirror_view(d_counts);

#ifdef DEBUG
  printf("quickSortListKokkosDeviceArrays: Done creating views\n");
#endif

#ifdef DEBUG
  printf("quickSortListKokkosDeviceArrays: Initializing offset and counts arrays on host\n");
#endif

  for (i=0; i<num_arrays;i++) {
    h_offsets[i] = (int)(i*array_size);
    h_counts[i] = array_size;
  }
#ifdef DEBUG
  printf("quickSortListKokkosDeviceArrays: Initializing data array on host\n");
#endif

#ifdef DEBUG
  printf("quickSortListKokkosDeviceArrays: Copying arrays to device\n");
#endif

  Kokkos::deep_copy(d_offsets,h_offsets);
  Kokkos::deep_copy(d_counts,h_counts);

#ifdef DEBUG
  printf("quickSortListKokkosDeviceArrays: Performing sort\n");
#endif

  Kokkos::parallel_for(Kokkos::TeamPolicy<>(num_arrays, Kokkos::AUTO()),
      TestTeamBitonicFunctor<ValView, OrdView>(d_values, d_counts, d_offsets));
  Kokkos::fence();

#ifdef DEBUG
  printf("quickSortList: num_arrays, array_size = %ld, %ld\n",num_arrays,array_size);
#endif
}


// Code taken from xorshf96()

KOKKOS_INLINE_FUNCTION size_t rand_local(void) {          //period 2^96-1
static size_t x=123456789, y=362436069, z=521288629;
size_t t;
    x ^= x << 16;
    x ^= x >> 5;
    x ^= x << 1;

   t = x;
   x = y;
   y = z;
   z = t ^ x ^ y;

  return z;
}

KOKKOS_INLINE_FUNCTION double log_local(double x) {
  int n,N=10,onefac=1;
  double y,xfac,f = 0.0;

  xfac = y = x-1.;

  for (n=1;n<=10;n++) {
    f += (double)onefac*xfac/(double)n;
    onefac *= -1;
    xfac *= y;
  }
  return(f);
}

void swap(size_t *a, size_t *b) 
{ 
  size_t t = *a; 
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

void quickSortList(size_t arr[], size_t stack[], size_t num_arrays, size_t array_size) {
  size_t i;
#ifdef DEBUG
  printf("quickSortList: num_arrays, array_size = %ld, %ld\n",num_arrays,array_size);
#endif
#ifdef USE_OPENMP
#pragma omp parallel for default(shared) private(i)
#endif
  for (i = 0; i < num_arrays; i++) {
    size_t *this_window = &arr[i*array_size];
    size_t *this_stack = &stack[i*array_size];
    quickSortIterative(this_window,this_stack,0,array_size-1);
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
int lmodeim_kokkos(DIFFIMAGE *imdiff_in)
{
  RCCOORDS_DATA 
    half_height,
    half_width,
    n,
    m; 

  static IMAGE_DATA_TYPE
    //IMAGE_DATA_TYPE
    *image_all = NULL;

  IMAGE_DATA_TYPE
    *image = NULL,
    maxval,
    minval,
    binsize;
  
  int
    return_value = 0;

  static size_t
    //size_t
    *image_mode_all = NULL,
    *window_all = NULL,
    *nvals_all = NULL,
    *stack_all = NULL;

  size_t
    //size_t
    *image_mode = NULL,
    *window = NULL,
    *nvals = NULL,
    *stack = NULL;

  DIFFIMAGE *imdiff;

  int reentry = imdiff_in->reentry;

  size_t wlen = (imdiff_in->mode_height+2)*(imdiff_in->mode_width+2);
    
  size_t num_panels;

  int  pidx;

  double tic, toc, tmkarr, tsort, tmkimg;

  size_t
    i,
    j;

  static size_t 
    //size_t 
    image_length = 0,
    hpixels = 0,
    vpixels = 0;

  size_t jblock, iblock;

  size_t
    num_jblocks = LUNUS_NUM_JBLOCKS,
    num_iblocks = LUNUS_NUM_IBLOCKS;

  static size_t 
    num_arrays,
    num_values,
    num_per_jblock, 
    num_per_iblock;

  //  printf("omp_get_num_teams() = %d, omp_get_num_threads() = %d\n",omp_get_num_teams(), omp_get_num_threads());

  //  fflush(stdout);

  imdiff = &imdiff_in[0];
  num_panels = imdiff->num_panels;

#ifdef DEBUG
  printf("LMODEIM_KOKKOS: num_iblocks, num_jblocks = %ld, %ld\n");
#endif
  
  if (reentry == 2 || reentry == 3) {
    if (hpixels != imdiff->hpixels || vpixels != imdiff->vpixels) {
      perror("LMODEIM_KOKKOS: Image panel size changed on reentry. Aborting\n");
      exit(1);
    }
  }

  half_height = imdiff->mode_height / 2;
  half_width = imdiff->mode_width / 2;
  image_length = imdiff->image_length;
  hpixels = imdiff->hpixels;
  vpixels = imdiff->vpixels;
  binsize = imdiff->mode_binsize;


  if (reentry == 0 || reentry == 1) {

    //    printf("MODEIM: Allocating arrays\n");

    num_per_jblock = 0;
    for (jblock = 0; jblock < num_jblocks; jblock++) {
      size_t jlo = half_height + jblock;
      size_t jhi = vpixels - half_height;     
      size_t num_this = (jhi - jlo)/num_jblocks+1;
      if (num_per_jblock < num_this) num_per_jblock = num_this;
    }
    num_per_iblock = 0;
    for (iblock = 0; iblock < num_iblocks; iblock++) {
      size_t ilo = half_width + iblock;
      size_t ihi = hpixels - half_width;     
      size_t num_this = (ihi - ilo)/num_iblocks+1;
      if (num_per_iblock < num_this) num_per_iblock = num_this;
    }

    num_arrays = num_per_jblock * num_per_iblock * num_panels;
    num_values = num_arrays * wlen;

    /* 
     * Allocate working arrays: 
     */ 
  
    image_all = (IMAGE_DATA_TYPE *)calloc(image_length*num_panels,sizeof(IMAGE_DATA_TYPE));
    image_mode_all = (size_t *)calloc(image_length*num_panels,sizeof(size_t));
    nvals_all = (size_t *)calloc(num_arrays,sizeof(size_t));
    window_all = (size_t *)calloc(num_values,sizeof(size_t));
    stack_all = (size_t *)calloc(num_values,sizeof(size_t));

    if (!image_all || !image_mode_all || !window_all || !stack_all) {
      sprintf(imdiff->error_msg,"\nLMODEIM:  Couldn't allocate arrays.\n\n");
      return_value = 1;
      return(return_value);
    }

  } 

  size_t num_mode_values=0, num_median_values=0, num_med90_values=0, num_this_values=0, num_ignored_values=0;

  //  printf("LMODEIM: mode_height = %d, mode_width = %d, num_panels = %d\n",imdiff_in->mode_height,imdiff_in->mode_width,imdiff_in->num_panels);

  IMAGE_DATA_TYPE overload_tag = imdiff->overload_tag;
  IMAGE_DATA_TYPE ignore_tag = imdiff->ignore_tag;

  int got_first_val = 0;  
    
  for (pidx = 0; pidx < num_panels; pidx++) {
    imdiff = &imdiff_in[pidx];
    image = &image_all[pidx*image_length];
     
    if (pidx != imdiff->this_panel) {
      perror("LMODEIM: Image panels are not indexed sequentially. Aborting\n");
      exit(1);
    }
    if (hpixels != imdiff->hpixels || vpixels != imdiff->vpixels) {
      perror("LMODEIM: Image panels are not identically formatted. Aborting\n");
      exit(1);
    }

    memcpy(image, imdiff->image, image_length*sizeof(IMAGE_DATA_TYPE));

    // Compute min and max for image

    size_t index; 

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
  }

  { // Start of life of the kokkos arrays
    typedef Kokkos::View<size_t *> OrdView;
    typedef Kokkos::View<size_t *> ValView;
    typedef Kokkos::View<IMAGE_DATA_TYPE *> ImgView;
    typedef Kokkos::View<IMAGE_DATA_TYPE *> ImgTParamView;
    typedef Kokkos::View<size_t *> SizeTParamView;
  
    OrdView d_offsets("d_offsets",num_arrays);
  
    OrdView d_counts("d_counts",num_arrays);
  
    ValView d_window_all("d_window_all",num_values);
    
    OrdView d_nvals_all("d_nvals_all",num_arrays);
  
    ImgView d_image_all("d_image_all",num_panels*image_length);
    Kokkos::View<IMAGE_DATA_TYPE *, Kokkos::HostSpace, Kokkos::MemoryTraits<Kokkos::Unmanaged> > h_image_all(image_all,num_panels*image_length);

  
    ValView d_image_mode_all("d_image_mode_all",num_panels*image_length);
    Kokkos::View<size_t*, Kokkos::HostSpace, Kokkos::MemoryTraits<Kokkos::Unmanaged> > h_image_mode_all(image_mode_all,num_panels*image_length);

    size_t num_params = 100;
    SizeTParamView d_sizet_params("d_sizet_params",num_params); 
    Kokkos::View<size_t *>::HostMirror h_sizet_params = Kokkos::create_mirror_view(d_sizet_params);
    ImgTParamView d_imgt_params("d_imgt_params",num_params); 
    Kokkos::View<IMAGE_DATA_TYPE *>::HostMirror h_imgt_params = Kokkos::create_mirror_view(d_imgt_params);
  
    Kokkos::deep_copy(d_image_all,h_image_all);
    Kokkos::deep_copy(d_image_mode_all,h_image_mode_all);


    // Compute the mode filtered image

    double start = ltime();

    tic = ltime();

    for (jblock = 0; jblock < num_jblocks; jblock++) {
      for (iblock = 0; iblock < num_iblocks; iblock++) {
	size_t jlo = half_height + jblock;
	size_t jhi = vpixels - half_height;     
	size_t ilo = half_width + iblock;
	size_t ihi = hpixels - half_width;     

	// Define params array
	h_imgt_params[0] = minval;
	h_imgt_params[1] = binsize;
	h_imgt_params[2] = overload_tag;
	h_imgt_params[3] = ignore_tag;
	h_sizet_params[0] = wlen;
	h_sizet_params[1] = num_jblocks;
	h_sizet_params[2] = num_iblocks;
	h_sizet_params[3] = num_per_jblock;
	h_sizet_params[4] = num_per_iblock;
	h_sizet_params[5] = jlo;
	h_sizet_params[6] = jhi;
	h_sizet_params[7] = ilo;
	h_sizet_params[8] = ihi;
	h_sizet_params[9] = num_panels;
	h_sizet_params[10] = image_length;
	h_sizet_params[11] = hpixels;
  
	Kokkos::deep_copy(d_imgt_params,h_imgt_params);
	Kokkos::deep_copy(d_sizet_params,h_sizet_params);
	typedef Kokkos::TeamPolicy<>               team_policy;
	typedef typename team_policy::member_type  team_member;
	Kokkos::parallel_for(Kokkos::TeamPolicy<>(num_per_jblock, Kokkos::AUTO()),KOKKOS_LAMBDA(const team_member& thread) {

	    
	    IMAGE_DATA_TYPE  minval = d_imgt_params[0];
	    IMAGE_DATA_TYPE binsize = d_imgt_params[1];
	    IMAGE_DATA_TYPE overload_tag = d_imgt_params[2];
	    IMAGE_DATA_TYPE ignore_tag = d_imgt_params[3];
	    size_t wlen = d_sizet_params[0];
	    size_t num_jblocks = d_sizet_params[1];
	    size_t num_iblocks = d_sizet_params[2];
	    size_t num_per_jblock = d_sizet_params[3];
	    size_t num_per_iblock = d_sizet_params[4];
	    size_t jlo = d_sizet_params[5];
	    size_t jhi = d_sizet_params[6];
	    size_t ilo = d_sizet_params[7];
	    size_t ihi = d_sizet_params[8];
	    size_t num_panels = d_sizet_params[9];
	    size_t image_length = d_sizet_params[10];
	    size_t hpixels = d_sizet_params[11];
	    
	    for (int pidx = 0; pidx < num_panels; pidx++) {
	      size_t this_image_idx = pidx*image_length;
	      size_t first_window_idx = pidx*wlen*num_per_jblock*num_per_iblock;
	      size_t first_nval_idx = pidx*num_per_jblock*num_per_iblock;

	      int j = jlo + thread.league_rank()*num_jblocks;
	      //	  for (j = jlo; j < jhi; j=j+num_jblocks) {
	      if (j < jhi) {
		Kokkos::parallel_for(Kokkos::TeamThreadRange(thread, num_per_iblock), [&](int ii) {
		  int i = ilo + ii*num_iblocks;
		  if (i < ihi) {
		    //		for (int i = ilo; i < ihi; i=i+num_iblocks) {
		    size_t block_idx = (((j-jlo)/num_jblocks)*num_per_iblock+(i-ilo)/num_iblocks);
		    size_t window_idx = first_window_idx + block_idx * wlen;
		    size_t nval_idx = first_nval_idx + block_idx;
		    size_t k;
		    for (k = 0; k < wlen; k++) {
		      d_window_all(window_idx + k) = ULONG_MAX;
		    }
		    size_t l = 0;
		    //		    size_t wind = window_idx;
		    RCCOORDS_DATA rlo = j - half_height;
		    RCCOORDS_DATA rhi = (j + half_height);
		    RCCOORDS_DATA clo = i - half_width;
		    RCCOORDS_DATA chi = (i + half_width);
		    RCCOORDS_DATA nr = 2*half_height + 1;
		    RCCOORDS_DATA nc = 2*half_width + 1;
		    for (size_t ri = 0; ri < nr; ri++) {
		      for (size_t ci = 0; ci < nc; ci++) {
			RCCOORDS_DATA r = j - half_height + ri;
			RCCOORDS_DATA c = i - half_width + ci;
			//		    for (r = rlo; r <= rhi; r++) {
			//		      for (c = clo; c <= chi; c++) {
			size_t index = this_image_idx + r*hpixels + c;
			size_t wind = window_idx + ri*nc + ci;
			if ((d_image_all(index) != overload_tag) &&
			    (d_image_all(index) != ignore_tag) &&
			    (d_image_all(index) < MAX_IMAGE_DATA_VALUE)) {
			  d_window_all(wind) = (d_image_all(index)-minval)/binsize + 1;
			  l++;
			}
			else {
			  d_window_all(wind) = ULONG_MAX;
			}
			//			wind++;
		      }
		    }
		    d_nvals_all(nval_idx) = l;
		  }
		}); // Kokkos::parallel_for()
	      }
	    }
	  }); // Kokkos::parallel_for()
	
	toc = ltime();
	tmkarr = toc - tic;

	tic = ltime();

	quickSortListKokkosDeviceArrays<ValView>(d_window_all,num_per_iblock*num_per_jblock*num_panels,wlen);

	toc = ltime();
	tsort = toc - tic;

	tic = ltime();
	Kokkos::parallel_for(Kokkos::TeamPolicy<>(num_per_jblock, Kokkos::AUTO()),KOKKOS_LAMBDA(const team_member& thread) {

	    IMAGE_DATA_TYPE  minval = d_imgt_params[0];
	    IMAGE_DATA_TYPE binsize = d_imgt_params[1];
	    IMAGE_DATA_TYPE overload_tag = d_imgt_params[2];
	    IMAGE_DATA_TYPE ignore_tag = d_imgt_params[3];
	    size_t wlen = d_sizet_params[0];
	    size_t num_jblocks = d_sizet_params[1];
	    size_t num_iblocks = d_sizet_params[2];
	    size_t num_per_jblock = d_sizet_params[3];
	    size_t num_per_iblock = d_sizet_params[4];
	    size_t jlo = d_sizet_params[5];
	    size_t jhi = d_sizet_params[6];
	    size_t ilo = d_sizet_params[7];
	    size_t ihi = d_sizet_params[8];
	    size_t num_panels = d_sizet_params[9];
	    size_t image_length = d_sizet_params[10];
	    size_t hpixels = d_sizet_params[11];

	    for (int pidx = 0; pidx < num_panels; pidx++) {

	      size_t this_image_idx = pidx*image_length;
	      size_t first_window_idx = pidx*wlen*num_per_jblock*num_per_iblock;
	      size_t first_nval_idx = pidx*num_per_jblock*num_per_iblock;

	      int j = jlo + thread.league_rank()*num_jblocks;
	  //	  for (j = jlo; j < jhi; j=j+num_jblocks) {
	      if (j < jhi) {
		Kokkos::parallel_for(Kokkos::TeamThreadRange(thread, num_per_iblock), [&](int ii) {
		  int i = ilo + ii*num_iblocks;
		  if (i < ihi) {
		    //		for (int i = ilo; i < ihi; i=i+num_iblocks) {
		  int mode_ct = 0;
		  size_t mode_value=0, max_count=0;
		  size_t index_mode = this_image_idx + j*hpixels + i;
		  size_t this_value = (d_image_all(index_mode)-minval)/binsize + 1;
		  size_t block_idx = (((j-jlo)/num_jblocks)*num_per_iblock+(i-ilo)/num_iblocks);
		  size_t window_idx = first_window_idx + block_idx * wlen;
		  size_t nval_idx = first_nval_idx + block_idx;
		  size_t l = d_nvals_all(nval_idx);
		  if (l == 0 || d_image_all(index_mode) == ignore_tag || d_image_all(index_mode) == overload_tag || d_image_all(index_mode) >= MAX_IMAGE_DATA_VALUE) {
		    d_image_mode_all(index_mode) = 0;
		  } else {
		    int kmed = l/2;
		    int k90 = l*9/10;
		    size_t min_value = d_window_all(window_idx);
		    size_t max_value = d_window_all(window_idx + l-1);
		    size_t range_value = max_value - min_value;
		    size_t median_value = d_window_all(window_idx + kmed);
		    size_t med90_value = d_window_all(window_idx + k90);
		    
		    size_t this_count = 1;
		    size_t last_value = d_window_all(window_idx);
		    max_count = 1;
		    size_t k;
		    for (k = 1; k < l; k++) {
		      size_t this_window_value = d_window_all(window_idx + k);
		      if (this_window_value == last_value) {
			this_count++;
		      } else {
			last_value = this_window_value;
			this_count = 1;
		      }
		      if (this_count > max_count) max_count = this_count;
		    }
		    this_count = 1;
		    last_value = d_window_all(window_idx);
		    double p, entropy = 0.0;
		    for (k = 1; k < l; k++) {
		      size_t this_window_value = d_window_all(window_idx + k);
		      if (this_window_value == last_value) {
			this_count++;
		      } else {
			p = (double)this_count/(double)l;
			entropy -=  p * log_local(p);
			last_value = this_window_value;
			this_count = 1;
		      }
		      if (this_count == max_count) {
			mode_value += this_window_value;
			mode_ct++;
		      }
		    }
		    mode_value = (size_t)(((float)mode_value/(float)mode_ct) + .5);
		    p = (double)this_count/(double)l;
		    entropy -=  p * log_local(p);
#ifdef DEBUG
		    if (j == 600 && i == 600) {
		      printf("LMODEIM: entropy = %g, mode_ct = %d, mode_value = %ld, median_value = %ld, range_value = %ld, this_value = %ld, med90_value = %ld, kmed = %d, k90 = %d\n",entropy,mode_ct,mode_value,median_value,range_value,this_value,med90_value,kmed,k90);
		    } 
#endif 
		    if (range_value <= 2) {
		      d_image_mode_all(index_mode)  = this_value;
		    } else {
		      if (this_value <= med90_value) {
			d_image_mode_all(index_mode) = this_value;
		      } else {
			d_image_mode_all(index_mode) = d_window_all(window_idx + (size_t)(((double)k90*(double)rand_local())/(double)ULONG_MAX));
		      }
		    }
		  }
		  }
										      });
	      }
	    }
	  }); // Kokkos::parallel_for()
      
	toc = ltime();
	tmkimg = toc - tic;
#ifdef DEBUG
	printf("iblock = %d, jblock = %d, tmkarr = %g secs, tsort = %g secs, tmkimg = %g secs\n",iblock,jblock,tmkarr,tsort,tmkimg);
#endif

      }
    }
    // Now image_mode holds the mode filtered values
    // Convert these values to pixel values and store them in the input image

    Kokkos::deep_copy(h_image_mode_all,d_image_mode_all);

    double stop = ltime();

    double tel = stop-start;
    fflush(stdout);

#ifdef DEBUG
    printf("LMODEIM: %g seconds, num_mode_values=%ld,num_median_values=%ld,num_this_values=%ld,num_med90_values=%ld,num_ignored_values=%ld\n",tel,num_mode_values,num_median_values,num_this_values,num_med90_values,num_ignored_values);
#endif

    for (pidx = 0; pidx < num_panels; pidx++) {

      size_t this_image_idx = pidx*image_length;
      size_t first_window_idx = pidx*wlen*num_per_jblock*num_per_iblock;
      size_t first_nval_idx = pidx*num_per_jblock*num_per_iblock;
    
#ifdef USE_OPENMP
#pragma omp distribute
#endif

      for (j = 0; j < vpixels; j++) {
	if (j < half_height || j > (vpixels-half_height)) {
	  for (i = 0; i < hpixels; i++) {
	    size_t this_index = this_image_idx + j * hpixels + i;
	    image_all[this_index] = ignore_tag;
	  } 
	} else {

	  for (i = 0; i < half_width; i++) {
	    size_t this_index = this_image_idx + j * hpixels + i;
	    image_all[this_index] = ignore_tag;
	  }
	
	  for (i = hpixels - half_width; i < hpixels; i++) {
	    size_t this_index = this_image_idx + j * hpixels + i;
	    image_all[this_index] = ignore_tag;
	  }

#ifdef USE_OPENMP
#pragma omp parallel for default(shared)
#endif

	  for (i = half_width; i < hpixels - half_width; i++) {
	    size_t this_index = this_image_idx + j * hpixels + i;
	    if (image_mode_all[this_index] != 0) {
	      image_all[this_index] = (image_mode_all[this_index]-1)*binsize + minval;
	    } else {
	      image_all[this_index] = ignore_tag;
	    }
	  }
	}
      }
    }

    for (pidx = 0; pidx < num_panels; pidx++) {

      imdiff = &imdiff_in[pidx];
      image = &image_all[pidx*image_length];

      memcpy(imdiff->image,image,image_length*sizeof(IMAGE_DATA_TYPE));
    }
  } // End of life of the kokkos arrays
  
  if (reentry == 0 || reentry == 3) {

    //      printf("MODEIM: Freeing arrays\n");
    free(image_all);
    free(image_mode_all);
    free(window_all);
    free(nvals_all);
    free(stack_all);
  }

 CloseShop:
  return(return_value);
}

