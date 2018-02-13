/* LUNUS.C - Diffuse scattering analysis and modeling.
   
   Author: Mike Wall  
   Date: 6/13/2017
   Version: 1.
   
   "lunus <input_deck>"

   Apply lunus diffuse scattering methods according to instructions in input_deck
   
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
        *deckin,
	*imagein,
        *lunusout,
        *scaleout;
  
  char
    *imageinpath,
    *lunusoutpath,
    *scaleoutpath,
    *deck,
    *image_prefix,
    *image_suffix,
    *lunus_image_prefix,
    *scale_image_prefix,
    *lunus_image_dir,
    *raw_image_dir,
    error_msg[LINESIZE];

  size_t
    i,
    num_images;

  float
    normim_tilt_x=0.0,
    normim_tilt_y=0.0,
    polarim_dist=-1.,
    polarim_offset=0.0,
    polarim_polarization=1.0,
    correction_factor_scale=1.0;

  int
    modeim_bin_size=1,
    modeim_kernel_width,
    thrshim_min,
    thrshim_max,
    punchim_xmax,
    punchim_xmin,
    punchim_ymax,
    punchim_ymin,
    windim_xmax,
    windim_xmin,
    windim_ymax,
    windim_ymin;

  DIFFIMAGE 
	*imdiff;

  size_t 
    str_length,
    num_read;

  MPIVARS
    *mpiv;


  // Initialize MPI

  mpiv = (MPIVARS *)malloc(sizeof(MPIVARS));
  mpiv->argc = argc;
  mpiv->argv = argv;

  linitMPI(mpiv);

/*
 * Set input line defaults:
 */

/*
 * Read information from input line:
 */
	switch(argc) {
		case 2:
			if (strcmp(argv[1], "-") == 0) {
				deckin = stdin;
			}
			else {
			 if ( (deckin = fopen(argv[1],"r")) == NULL ) {
				printf("Can't open %s.",argv[1]);
				exit(0);
			 }
			}
			break;
		default:
			printf("\n Usage: lunus <input deck>\n\n");
			lfinalMPI(mpiv);
			exit(0);
	}

	// Read input deck into a buffer

	fseek(deckin, 0, SEEK_END); // seek to end of file
	str_length = ftell(deckin); // get current file pointer
	fseek(deckin, 0, SEEK_SET); // seek back to beginning of file
	// proceed with allocating memory and reading the file	

	deck = (char *)malloc(str_length);
	
	num_read = fread(deck,sizeof(char),str_length,deckin);

	//	printf("Length of input deck = %ld\n",num_read);

	if (lgettag(deck,"\nraw_image_dir") != NULL) {
	  str_length = strlen(lgettag(deck,"\nraw_image_dir"));
	  raw_image_dir = (char *)malloc(str_length+1);
	  strcpy(raw_image_dir,lgettag(deck,"\nraw_image_dir"));
	} else {
	  perror("LUNUS: Must provide raw_image_dir\n");
	  exit(1);
	}

	if (lgettag(deck,"\nlunus_image_dir") != NULL) {
	  str_length = strlen(lgettag(deck,"\nlunus_image_dir"));
	  lunus_image_dir = (char *)malloc(str_length+1);
	  strcpy(lunus_image_dir,lgettag(deck,"\nlunus_image_dir"));
	} else {
	  perror("LUNUS: Must provide lunus_image_dir\n");
	  exit(1);
	}

	if (lgettag(deck,"\nimage_prefix") != NULL) {
	  str_length = strlen(lgettag(deck,"\nimage_prefix"));
	  image_prefix = (char *)malloc(str_length+1);
	  strcpy(image_prefix,lgettag(deck,"\nimage_prefix"));
	} else {
	  perror("LUNUS: Must provide image_prefix\n");
	  exit(1);
	}

	if (lgettag(deck,"\nimage_suffix") != NULL) {
	  str_length = strlen(lgettag(deck,"\nimage_suffix"));
	  image_suffix = (char *)malloc(str_length+1);
	  strcpy(image_suffix,lgettag(deck,"\nimage_suffix"));
	} else {
	  image_suffix = (char *)malloc(strlen("img"+1));
	  strcpy(image_suffix,"img");
	}
	  

	if (lgettag(deck,"\nlunus_image_prefix") != NULL) {
	  str_length = strlen(lgettag(deck,"\nlunus_image_prefix"));
	  lunus_image_prefix = (char *)malloc(str_length+1);
	  strcpy(lunus_image_prefix,lgettag(deck,"\nlunus_image_prefix"));
	} else {
	  perror("Must provide lunus_image_prefix\n");
	  exit(1);
	}

	if (lgettag(deck,"\nscale_image_prefix") != NULL) {
	  str_length = strlen(lgettag(deck,"\nscale_image_prefix"));
	  scale_image_prefix = (char *)malloc(str_length+1);
	  strcpy(scale_image_prefix,lgettag(deck,"\nscale_image_prefix"));
	} else {
	  perror("Must provide scale_image_prefix\n");
	  exit(1);
	}

	str_length = strlen(lgettag(deck,"\npunchim_xmax"));

	if (str_length != 0) {
	  punchim_xmax = atoi(lgettag(deck,"\npunchim_xmax"));
	}

	str_length = strlen(lgettag(deck,"\npunchim_xmin"));

	if (str_length != 0) {
	  punchim_xmin = atoi(lgettag(deck,"\npunchim_xmin"));
	}

	str_length = strlen(lgettag(deck,"\npunchim_ymax"));

	if (str_length != 0) {
	  punchim_ymax = atoi(lgettag(deck,"\npunchim_ymax"));
	}

	str_length = strlen(lgettag(deck,"\npunchim_ymin"));

	if (str_length != 0) {
	  punchim_ymin = atoi(lgettag(deck,"\npunchim_ymin"));
	}

	str_length = strlen(lgettag(deck,"\nwindim_xmax"));

	if (str_length != 0) {
	  windim_xmax = atoi(lgettag(deck,"\nwindim_xmax"));
	}

	str_length = strlen(lgettag(deck,"\nwindim_xmin"));

	if (str_length != 0) {
	  windim_xmin = atoi(lgettag(deck,"\nwindim_xmin"));
	}

	str_length = strlen(lgettag(deck,"\nwindim_ymax"));

	if (str_length != 0) {
	  windim_ymax = atoi(lgettag(deck,"\nwindim_ymax"));
	}

	str_length = strlen(lgettag(deck,"\nwindim_ymin"));

	if (str_length != 0) {
	  windim_ymin = atoi(lgettag(deck,"\nwindim_ymin"));
	}

	str_length = strlen(lgettag(deck,"\nthrshim_max"));

	if (str_length != 0) {
	  thrshim_max = atoi(lgettag(deck,"\nthrshim_max"));
	}

	str_length = strlen(lgettag(deck,"\nthrshim_min"));

	if (str_length != 0) {
	  thrshim_min = atoi(lgettag(deck,"\nthrshim_min"));
	}

	str_length = strlen(lgettag(deck,"\nmodeim_bin_size"));

	if (str_length != 0) {
	  modeim_bin_size = atoi(lgettag(deck,"\nmodeim_bin_size"));
	}

	str_length = strlen(lgettag(deck,"\nmodeim_kernel_width"));

	if (str_length != 0) {
	  modeim_kernel_width = atoi(lgettag(deck,"\nmodeim_kernel_width"));
	}

	str_length = strlen(lgettag(deck,"\nnum_images"));

	if (str_length != 0) {
	  num_images = (size_t)atol(lgettag(deck,"\nnum_images"));
	}

	/*	str_length = strlen(lgettag(deck,"\npolarim_dist"));

	if (str_length != 0) {
	  polarim_dist = atof(lgettag(deck,"\npolarim_dist"));
	}

	printf("polarim_dist=%f\n",polarim_dist);
	*/

	str_length = strlen(lgettag(deck,"\npolarim_offset"));

	if (str_length != 0) {
	  polarim_offset = atof(lgettag(deck,"\npolarim_offset"));
	}

	str_length = strlen(lgettag(deck,"\npolarim_polarization"));

	if (str_length != 0) {
	  polarim_polarization = atof(lgettag(deck,"\npolarim_polarization"));
	}

	str_length = strlen(lgettag(deck,"\ncorrection_factor_scale"));
	
	if (str_length != 0) {
	  correction_factor_scale = atof(lgettag(deck,"\ncorrection_factor_scale"));
	}

	if (mpiv->my_id == 0) {
	  printf("raw_image_dir=%s\n",raw_image_dir);
	  
	  printf("lunus_image_dir=%s\n",lunus_image_dir);
	  
	  printf("image_prefix=%s\n",image_prefix);
	  
	  printf("image_suffix=%s\n",image_suffix);
	  
	  printf("lunus_image_prefix=%s\n",lunus_image_prefix);
	  
	  printf("scale_image_prefix=%s\n",scale_image_prefix);
	  
	  printf("punchim_xmax=%d\n",punchim_xmax);
	  
	  printf("punchim_xmin=%d\n",punchim_xmin);
	  
	  printf("punchim_ymax=%d\n",punchim_ymax);
	  
	  printf("punchim_ymin=%d\n",punchim_ymin);
	  
	  printf("windim_xmax=%d\n",windim_xmax);
	  
	  printf("windim_xmin=%d\n",windim_xmin);
	  
	  printf("windim_ymax=%d\n",windim_ymax);
	  
	  printf("windim_ymin=%d\n",windim_ymin);
	  
	  printf("thrshim_max=%d\n",thrshim_max);
	  
	  printf("thrshim_min=%d\n",thrshim_min);
	  
	  printf("modeim_kernel_width=%d\n",modeim_kernel_width);
	  
	  printf("modeim_bin_size=%d\n",modeim_bin_size);
	  
	  printf("num_images=%ld\n",num_images);
	  
	  printf("polarim_offset=%f\n",polarim_offset);
	
	  printf("polarim_polarization=%f\n",polarim_polarization);

	  printf("correction_factor_scale=%f\n",correction_factor_scale);

	}
	/*
	 * Initialize diffraction image:
	 */

	if ((imdiff = linitim()) == NULL) {
	  perror("Couldn't initialize diffraction image.\n\n");
	  exit(0);
	}

	// Define parameters from input deck

	imdiff->punchim_upper.c = punchim_xmax;
	imdiff->punchim_lower.c = punchim_xmin;
	imdiff->punchim_upper.r = punchim_ymax;
	imdiff->punchim_lower.r = punchim_ymin;

	imdiff->window_upper.c = windim_xmax;
	imdiff->window_lower.c = windim_xmin;
	imdiff->window_upper.r = windim_ymax;
	imdiff->window_lower.r = windim_ymin;

	imdiff->upper_threshold = thrshim_max;
	imdiff->lower_threshold = thrshim_min;

	imdiff->polarization = polarim_polarization;
	imdiff->polarization_offset = polarim_offset;

	imdiff->cassette.x = normim_tilt_x;
	imdiff->cassette.y = normim_tilt_y;
	imdiff->cassette.z = 0.0;

	imdiff->mode_height = modeim_kernel_width - 1;
	imdiff->mode_width = modeim_kernel_width - 1;
	imdiff->mode_binsize = modeim_bin_size;

	// Process all of the images

	int n;

	n = num_images/mpiv->num_procs;
	if (num_images % mpiv->num_procs != 0) {
	  n++;
	} 

	int i0 = mpiv->my_id*n+1;
	int i1 = (mpiv->my_id+1)*n;


	for (i=i0;i<=i1&&i<=num_images;i++) {

	  str_length = snprintf(NULL,0,"%s/%s_%05d.%s",raw_image_dir,image_prefix,i,image_suffix);

	  imageinpath = (char *)malloc(str_length+1);

	  sprintf(imageinpath,"%s/%s_%05d.%s",raw_image_dir,image_prefix,i,image_suffix);

	  //	  printf("imageinpath = %s\n",imageinpath);

	  /*
	   * Read diffraction image:
	   */
	  
	  if ( (imagein = fopen(imageinpath,"rb")) == NULL ) {
	    printf("Can't open %s.",imageinpath);
	    exit(0);
	  }

	  imdiff->infile = imagein;
	  if (lreadim(imdiff) != 0) {
	    perror(imdiff->error_msg);
	    exit(0);
	  }

	  fclose(imagein);

	  // Apply masks

	  lpunchim(imdiff);
	  lwindim(imdiff);
	  lthrshim(imdiff);

	  // Apply correction factor

	  imdiff->correction = (float *)malloc(imdiff->image_length*sizeof(float));
	  imdiff->correction[0]=correction_factor_scale;
	  lcfim(imdiff);
	  if (lmulcfim(imdiff) != 0) {
	    perror(imdiff->error_msg);
	    exit(1);
	  }
	  

	  // Write masked and corrected image

	  str_length = snprintf(NULL,0,"%s/%s_%05d.%s",lunus_image_dir,lunus_image_prefix,i,image_suffix);

	  lunusoutpath = (char *)malloc(str_length+1);

	  sprintf(lunusoutpath,"%s/%s_%05d.%s",lunus_image_dir,lunus_image_prefix,i,image_suffix);

	  if ( (lunusout = fopen(lunusoutpath,"wb")) == NULL ) {
	    printf("Can't open %s.",lunusoutpath);
	    exit(1);
	  }

	  imdiff->outfile = lunusout;
	  if(lwriteim(imdiff) != 0) {
	    perror(imdiff->error_msg);
	    exit(1);
	  }

	  fclose(lunusout);

	  // Mode filter to create image to be used for scaling

	  lmodeim(imdiff);

	  // Write mode filtered image

	  str_length = snprintf(NULL,0,"%s/%s_%05d.%s",lunus_image_dir,scale_image_prefix,i,image_suffix);

	  scaleoutpath = (char *)malloc(str_length+1);

	  sprintf(scaleoutpath,"%s/%s_%05d.%s",lunus_image_dir,scale_image_prefix,i,image_suffix);

	  if ( (scaleout = fopen(scaleoutpath,"wb")) == NULL ) {
	    printf("Can't open %s.",scaleoutpath);
	    exit(1);
	  }

	  imdiff->outfile = scaleout;
	  if(lwriteim(imdiff) != 0) {
	    perror(imdiff->error_msg);
	    exit(1);
	  }

	  fclose(scaleout);

	  //	  imdiff->correction[0]=1.;
	  //	  lcfim(imdiff);

	}

	lfinalMPI(mpiv);

	fclose(deckin);
}

