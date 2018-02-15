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
    inputdeck[1000],
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
    *do_integrate,
    error_msg[LINESIZE];

  void *buf;

  size_t
    i,
    num_images;

  float
    normim_tilt_x=0.0,
    normim_tilt_y=0.0,
    polarim_dist=-1.,
    polarim_offset=0.0,
    polarim_polarization=1.0,
    correction_factor_scale=1.0,
    cella,
    cellb,
    cellc,
    alpha,
    beta,
    gamma,
    resolution;

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
    windim_ymin,
    scale_inner_radius=0,
    scale_outer_radius=0,
    pphkl = 1;

  DIFFIMAGE 
    *imdiff, *imdiff_corrected, *imdiff_scale, *imdiff_scale_ref;

  LAT3D
    *lat;

  unsigned long
    *latct;

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
		  strcpy(inputdeck,argv[1]);
			break;
		default:
			printf("\n Usage: lunus <input deck>\n\n");
			lfinalMPI(mpiv);
			exit(0);
	}

	//	printf("LUNUS: reading input deck\n");

	// Read input deck into buffer

	num_read = lreadbuf(&deck,inputdeck);

	//	printf("Length of input deck = %ld\n",num_read);

	// Parse input deck

	if ((raw_image_dir=lgettag(deck,"\nraw_image_dir")) == NULL) {
	  perror("LUNUS: Must provide raw_image_dir\n");
	  exit(1);
	}

	if ((lunus_image_dir=lgettag(deck,"\nlunus_image_dir")) == NULL) {
	  perror("LUNUS: Must provide lunus_image_dir\n");
	  exit(1);
	}

	if ((image_prefix=lgettag(deck,"\nimage_prefix")) == NULL) {
	  perror("LUNUS: Must provide image_prefix\n");
	  exit(1);
	}

	if ((image_suffix=lgettag(deck,"\nimage_suffix")) == NULL) {
	  image_suffix = (char *)malloc(strlen("img"+1));
	  strcpy(image_suffix,"img");
	}
	  

	if ((lunus_image_prefix=lgettag(deck,"\nlunus_image_prefix")) == NULL) {
	  perror("Must provide lunus_image_prefix\n");
	  exit(1);
	}

	if ((scale_image_prefix=lgettag(deck,"\nscale_image_prefix")) == NULL) {
	  perror("Must provide scale_image_prefix\n");
	  exit(1);
	}

	if (lgettag(deck,"\npunchim_xmax") == NULL) {
	  perror("Must provide punchim_xmax\n");
	  exit(1);
	} else {
	  punchim_xmax = atoi(lgettag(deck,"\npunchim_xmax"));
	}

	if (lgettag(deck,"\npunchim_xmin") == NULL) {
	  perror("Must provide punchim_xmin\n");
	  exit(1);
	} else {
	  punchim_xmin = atoi(lgettag(deck,"\npunchim_xmin"));
	}

	if (lgettag(deck,"\npunchim_ymax") == NULL) {
	  perror("Must provide punchim_ymax\n");
	  exit(1);
	} else {
	  punchim_ymax = atoi(lgettag(deck,"\npunchim_ymax"));
	}

	if (lgettag(deck,"\npunchim_ymin") == NULL) {
	  perror("Must provide punchim_ymin\n");
	  exit(1);
	} else {
	  punchim_ymin = atoi(lgettag(deck,"\npunchim_ymin"));
	}

	if (lgettag(deck,"\nwindim_xmax") == NULL) {
	  perror("Must provide windim_xmax\n");
	  exit(1);
	} else {
	  windim_xmax = atoi(lgettag(deck,"\nwindim_xmax"));
	}

	if (lgettag(deck,"\nwindim_xmin") == NULL) {
	  perror("Must provide windim_xmin\n");
	  exit(1);
	} else {
	  windim_xmin = atoi(lgettag(deck,"\nwindim_xmin"));
	}

	if (lgettag(deck,"\nwindim_ymax") == NULL) {
	  perror("Must provide windim_ymax\n");
	  exit(1);
	} else {
	  windim_ymax = atoi(lgettag(deck,"\nwindim_ymax"));
	}

	if (lgettag(deck,"\nwindim_ymin") == NULL) {
	  perror("Must provide windim_ymin\n");
	  exit(1);
	} else {
	  windim_ymin = atoi(lgettag(deck,"\nwindim_ymin"));
	}

	if ((do_integrate=lgettag(deck,"\ndo_integrate")) == NULL) {
	  do_integrate = (char *)malloc(strlen("false"+1));
	  strcpy(do_integrate,"false");
	}

	if (lgettag(deck,"\nscale_inner_radius") == NULL) {
	  if (strcmp(do_integrate,"true")==0) {
	    perror("Must provide scale_inner_radius for integration\n");
	    exit(1);
	  }
	} else {
	  scale_inner_radius = atoi(lgettag(deck,"\nscale_inner_radius"));
	}

	if (lgettag(deck,"\nscale_outer_radius") == NULL) {
	  if (strcmp(do_integrate,"true")==0) {
	    perror("Must provide scale_outer_radius for integration\n");
	    exit(1);
	  }
	} else {
	  scale_outer_radius = atoi(lgettag(deck,"\nscale_outer_radius"));
	}

	if (lgettag(deck,"\npphkl") != NULL) {
	  pphkl = atoi(lgettag(deck,"\npphkl"));
	}

	if (lgettag(deck,"\ncella") == NULL) {
	  if (strcmp(do_integrate,"true")==0) {
	    perror("Must provide cella for integration\n");
	    exit(1);
	  }
	} else {
	  cella = atof(lgettag(deck,"\ncella"));
	}

	if (lgettag(deck,"\ncellb") == NULL) {
	  if (strcmp(do_integrate,"true")==0) {
	    perror("Must provide cellb for integration\n");
	    exit(1);
	  }
	} else {
	  cellb = atof(lgettag(deck,"\ncellb"));
	}

	if (lgettag(deck,"\ncellc") == NULL) {
	  if (strcmp(do_integrate,"true")==0) {
	    perror("Must provide cellc for integration\n");
	    exit(1);
	  }
	} else {
	  cellc = atof(lgettag(deck,"\ncellc"));
	}

	if (lgettag(deck,"\nalpha") == NULL) {
	  if (strcmp(do_integrate,"true")==0) {
	    perror("Must provide alpha for integration\n");
	    exit(1);
	  }
	} else {
	  alpha = atof(lgettag(deck,"\nalpha"));
	}

	if (lgettag(deck,"\nbeta") == NULL) {
	  if (strcmp(do_integrate,"true")==0) {
	    perror("Must provide beta for integration\n");
	    exit(1);
	  }
	} else {
	  beta = atof(lgettag(deck,"\nbeta"));
	}

	if (lgettag(deck,"\ngamma") == NULL) {
	  if (strcmp(do_integrate,"true")==0) {
	    perror("Must provide gamma for integration\n");
	    exit(1);
	  }
	} else {
	  gamma = atof(lgettag(deck,"\ngamma"));
	}

	if (lgettag(deck,"\nresolution") == NULL) {
	  if (strcmp(do_integrate,"true")==0) {
	    perror("Must provide resolution for integration\n");
	    exit(1);
	  }
	} else {
	  resolution = atof(lgettag(deck,"\nresolution"));
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

	/*
	str_length = strlen(lgettag(deck,"\ndo_integrate"));
	
	if (str_length != 0) {
	  lunus_integrate = atof(lgettag(deck,"\ndo_integrate"));
	}
	*/

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

	imdiff->mask_inner_radius = scale_inner_radius;
	imdiff->mask_outer_radius = scale_outer_radius;

	// Define the integration lattices, if performing integration

	if (strcmp(do_integrate,"true")==0) {
	  lat = linitlt();
	  lat->xvoxels = ((int)((float)(pphkl)*cella/resolution)+1)*2;
	  lat->yvoxels = ((int)((float)(pphkl)*cellb/resolution)+1)*2;
	  lat->zvoxels = ((int)((float)(pphkl)*cellc/resolution)+1)*2;
	  lat->xyvoxels = lat->xvoxels * lat->yvoxels;
	  lat->lattice_length = lat->xyvoxels*lat->zvoxels;
	  lat->lattice = (LATTICE_DATA_TYPE *)calloc(lat->lattice_length,sizeof(LATTICE_DATA_TYPE));
	  latct = (unsigned long *)calloc(lat->lattice_length,sizeof(unsigned long));
	}

	// Process all of the images

	int n;

	n = num_images/mpiv->num_procs;
	if (num_images % mpiv->num_procs != 0) {
	  n++;
	} 

	int i0 = mpiv->my_id*n+1;
	int i1 = (mpiv->my_id+1)*n;

	int ct=0;

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

	  // Calculate correction factor

	  imdiff->correction = (float *)malloc(imdiff->image_length*sizeof(float));
	  imdiff->correction[0]=correction_factor_scale;
	  lcfim(imdiff);
	  

	  // Write masked and corrected image

	  imdiff_corrected = linitim();
	  lcloneim(imdiff_corrected,imdiff);
	  if (lmulcfim(imdiff_corrected) != 0) {
	    perror(imdiff_corrected->error_msg);
	    exit(1);
	  }
	  str_length = snprintf(NULL,0,"%s/%s_%05d.%s",lunus_image_dir,lunus_image_prefix,i,image_suffix);

	  lunusoutpath = (char *)malloc(str_length+1);

	  sprintf(lunusoutpath,"%s/%s_%05d.%s",lunus_image_dir,lunus_image_prefix,i,image_suffix);

	  if ( (lunusout = fopen(lunusoutpath,"wb")) == NULL ) {
	    printf("Can't open %s.",lunusoutpath);
	    exit(1);
	  }

	  imdiff_corrected->outfile = lunusout;
	  if(lwriteim(imdiff_corrected) != 0) {
	    perror(imdiff_corrected->error_msg);
	    exit(1);
	  }

	  fclose(lunusout);

	  // Mode filter to create image to be used for scaling

	  imdiff_scale = linitim();
	  lcloneim(imdiff_scale,imdiff_corrected);

	  lmodeim(imdiff_scale);

	  // Write mode filtered image

	  str_length = snprintf(NULL,0,"%s/%s_%05d.%s",lunus_image_dir,scale_image_prefix,i,image_suffix);

	  scaleoutpath = (char *)malloc(str_length+1);

	  sprintf(scaleoutpath,"%s/%s_%05d.%s",lunus_image_dir,scale_image_prefix,i,image_suffix);

	  if ( (scaleout = fopen(scaleoutpath,"wb")) == NULL ) {
	    printf("Can't open %s.",scaleoutpath);
	    exit(1);
	  }

	  imdiff_scale->outfile = scaleout;
	  if(lwriteim(imdiff_scale) != 0) {
	    perror(imdiff_scale->error_msg);
	    exit(1);
	  }

	  fclose(scaleout);

	  //	  imdiff->correction[0]=1.;
	  //	  lcfim(imdiff);

	  if (strcmp(do_integrate,"true")==0) {
	    
	    // Copy the reference image from rank 0 to all ranks

	    if (ct == 0) {
	      imdiff_scale_ref = linitim();
	      lcloneim(imdiff_scale_ref,imdiff_scale);
	      lbarrierMPI(mpiv);
	      lbcastImageMPI(imdiff_scale_ref->image,imdiff_scale_ref->image_length,0,mpiv);
	      lbarrierMPI(mpiv);
	    }
	  
	    // Calculate the image scale factor

	    lscaleim(imdiff_scale_ref,imdiff_scale);
	    printf("Image %d scale factor, error = %f, %f\n",i,imdiff_scale_ref->rfile[0],imdiff_scale_ref->rfile[1]);
	  }
	  ct++;
	}

	lfinalMPI(mpiv);

}

