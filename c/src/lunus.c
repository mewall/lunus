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
    *amatrix_format,
    *amatrix_path,
    *xvectors_path,
    *do_integrate_str,
    *filterhkl_str,
    *lattice_dir,
    *diffuse_lattice_prefix,
    *unit_cell,
    error_msg[LINESIZE];

  void *buf;

  int
    do_integrate,
    filterhkl;

  struct xyzcoords *xvectors_cctbx = NULL,*xvectors = NULL, *Hlist, *dHlist;

  IJKCOORDS_DATA *ilist, *jlist, *klist;

  struct xyzmatrix *amatrix;

  size_t
    index,
    i,
    j,
    num_images;

  IJKCOORDS_DATA
    i0, j0, k0;

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
    *imdiff, *imdiff_corrected = NULL, *imdiff_scale = NULL, *imdiff_scale_ref = NULL;

  LAT3D
    *lat;

  LATTICE_DATA_TYPE
    *latct = NULL;

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

	num_read = lreadbuf((void **)&deck,inputdeck);

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

	if ((do_integrate_str=lgettag(deck,"\ndo_integrate")) == NULL) {
	  do_integrate_str = (char *)malloc(strlen("False"+1));
	  strcpy(do_integrate_str,"False");
	}

	if (strcmp(do_integrate_str,"True")==0) {
	  do_integrate=1;
	} else {
	  do_integrate=0;
	}

	if ((filterhkl_str=lgettag(deck,"\nfilterhkl")) == NULL) {
	  filterhkl_str = (char *)malloc(strlen("False"+1));
	  strcpy(filterhkl_str,"False");
	}

	if (strcmp(filterhkl_str,"True")==0) {
	  filterhkl=1;
	} else {
	  filterhkl=0;
	}

	if (lgettag(deck,"\nscale_inner_radius") == NULL) {
	  if (do_integrate!=0) {
	    perror("Must provide scale_inner_radius for integration\n");
	    exit(1);
	  }
	} else {
	  scale_inner_radius = atoi(lgettag(deck,"\nscale_inner_radius"));
	}

	if (lgettag(deck,"\nscale_outer_radius") == NULL) {
	  if (do_integrate!=0) {
	    perror("Must provide scale_outer_radius for integration\n");
	    exit(1);
	  }
	} else {
	  scale_outer_radius = atoi(lgettag(deck,"\nscale_outer_radius"));
	}

	if (lgettag(deck,"\npphkl") != NULL) {
	  pphkl = atoi(lgettag(deck,"\npphkl"));
	}

	if ((unit_cell=lgettag(deck,"\nunit_cell")) == NULL) {
	  if (do_integrate!=0) {
	    perror("LUNUS: Must provide unit_cell for integration\n");
	    exit(1);
	  }
	}

	if (lgettag(deck,"\ncella") == NULL) {
	  if (do_integrate!=0) {
	    perror("Must provide cella for integration\n");
	    exit(1);
	  }
	} else {
	  cella = atof(lgettag(deck,"\ncella"));
	}

	if (lgettag(deck,"\ncellb") == NULL) {
	  if (do_integrate!=0) {
	    perror("Must provide cellb for integration\n");
	    exit(1);
	  }
	} else {
	  cellb = atof(lgettag(deck,"\ncellb"));
	}

	if (lgettag(deck,"\ncellc") == NULL) {
	  if (do_integrate!=0) {
	    perror("Must provide cellc for integration\n");
	    exit(1);
	  }
	} else {
	  cellc = atof(lgettag(deck,"\ncellc"));
	}

	if (lgettag(deck,"\nalpha") == NULL) {
	  if (do_integrate!=0) {
	    perror("Must provide alpha for integration\n");
	    exit(1);
	  }
	} else {
	  alpha = atof(lgettag(deck,"\nalpha"));
	}

	if (lgettag(deck,"\nbeta") == NULL) {
	  if (do_integrate!=0) {
	    perror("Must provide beta for integration\n");
	    exit(1);
	  }
	} else {
	  beta = atof(lgettag(deck,"\nbeta"));
	}

	if (lgettag(deck,"\ngamma") == NULL) {
	  if (do_integrate!=0) {
	    perror("Must provide gamma for integration\n");
	    exit(1);
	  }
	} else {
	  gamma = atof(lgettag(deck,"\ngamma"));
	}

	if (lgettag(deck,"\nresolution") == NULL) {
	  if (do_integrate!=0) {
	    perror("Must provide resolution for integration\n");
	    exit(1);
	  }
	} else {
	  resolution = atof(lgettag(deck,"\nresolution"));
	}

	if ((xvectors_path=lgettag(deck,"\nxvectors_path")) == NULL) {
	  if (do_integrate!=0) {
	    perror("Must provide xvectors_path for integration\n");
	    exit(1);
	  }
	}

	if ((amatrix_format=lgettag(deck,"\namatrix_format")) == NULL) {
	  if (do_integrate!=0) {
	    perror("Must provide amatrix_format for integration\n");
	    exit(1);
	  }
	}

	if ((lattice_dir=lgettag(deck,"\nlattice_dir")) == NULL) {
	  if (do_integrate!=0) {
	    perror("LUNUS: Must provide lattice_dir for integration\n");
	    exit(1);
	  }
	}

	if ((diffuse_lattice_prefix=lgettag(deck,"\ndiffuse_lattice_prefix")) == NULL) {
	  if (do_integrate!=0) {
	    perror("LUNUS: Must provide diffuse_lattice_prefix for integration\n");
	    exit(1);
	  }
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

	// Initialize other images

	imdiff_corrected = linitim();
	imdiff_scale = linitim();
	imdiff_scale_ref = linitim();

	// Define the integration lattices and associated variables
        //      if performing integration

	if (do_integrate!=0) {
	  lat = linitlt();
	  strcpy(lat->cell_str,unit_cell);
	  lparsecelllt(lat);
	  lat->cell.a *= pphkl;
	  lat->cell.b *= pphkl;
	  lat->cell.c *= pphkl;
	  lat->xvoxels = ((int)(lat->cell.a/resolution)+1)*2;
	  lat->yvoxels = ((int)(lat->cell.b/resolution)+1)*2;
	  lat->zvoxels = ((int)(lat->cell.c/resolution)+1)*2;
	  i0 = (IJKCOORDS_DATA)(lat->xvoxels/2. - 1.);
	  j0 = (IJKCOORDS_DATA)(lat->yvoxels/2. - 1.);
	  k0 = (IJKCOORDS_DATA)(lat->zvoxels/2. - 1.);
	  lat->xscale = 1./lat->cell.a;
	  lat->yscale = 1./lat->cell.b;
	  lat->zscale = 1./lat->cell.c;
	  lat->xbound.min = -i0*lat->xscale;
	  lat->ybound.min = -j0*lat->yscale;
	  lat->zbound.min = -k0*lat->zscale;
	  lat->xbound.max = lat->xbound.min + ((float)lat->xvoxels-1)*lat->xscale;
	  lat->ybound.max = lat->ybound.min + ((float)lat->yvoxels-1)*lat->yscale;
	  lat->zbound.max = lat->zbound.min + ((float)lat->zvoxels-1)*lat->zscale;
	  lat->origin.i = (IJKCOORDS_DATA)(-lat->xbound.min/lat->xscale + .5);
	  lat->origin.j = (IJKCOORDS_DATA)(-lat->ybound.min/lat->yscale + .5);
	  lat->origin.k = (IJKCOORDS_DATA)(-lat->zbound.min/lat->zscale + .5);
	  lat->xyvoxels = lat->xvoxels * lat->yvoxels;
	  lat->lattice_length = lat->xyvoxels*lat->zvoxels;
	  //	  if (lat->lattice != NULL) free(lat->lattice);
	  lat->lattice = (LATTICE_DATA_TYPE *)calloc(lat->lattice_length,sizeof(LATTICE_DATA_TYPE));
	  //	  if (latct != NULL) free(latct);
	  latct = (LATTICE_DATA_TYPE *)calloc(lat->lattice_length,sizeof(LATTICE_DATA_TYPE));
	}

	// Process all of the images

	int n;

	n = num_images/mpiv->num_procs;
	if (num_images % mpiv->num_procs != 0) {
	  n++;
	} 

	int ib = mpiv->my_id*n+1;
	int ie = (mpiv->my_id+1)*n;

	int ct=0;

	for (i=ib;i<=ie&&i<=num_images;i++) {

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

	  imdiff->correction[0]=correction_factor_scale;
	  lcfim(imdiff);	  

	  // Write masked and corrected image

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

	  if (do_integrate!=0) {
	    
	    // Set up common variables on the first pass

	    if (ct == 0) {
	      // Reference image for scaling
	      lcloneim(imdiff_scale_ref,imdiff_scale);
	      //	      lbarrierMPI(mpiv);
	      //	      printf("Barrier 1 passed\n");
	      // Use the rank 0 image
	      lbcastImageMPI(imdiff_scale_ref->image,imdiff_scale_ref->image_length,0,mpiv);
	      //	      lbarrierMPI(mpiv);
	      //	      printf("Barrier 2 passed\n");
	      // Read the xvectors on rank 0
	      if (mpiv->my_id == 0) {
		num_read = lreadbuf((void **)&xvectors_cctbx,xvectors_path);
		if (num_read != 3*imdiff->image_length*sizeof(float)) {
		  perror("LUNUS: Number of xvectors differs from number of pixels in image.\n");
		  exit(1);
		}
#ifdef DEBUG		
		for (j=0;j<3;j++) {
		  printf("(%f, %f, %f) ",xvectors_cctbx[j].x,xvectors_cctbx[j].y,xvectors_cctbx[j].z);
		}
		printf("\n");
#endif		
	      // Reorder the xvectors (transpose)

		index = 0;

		if (xvectors != NULL) free(xvectors);

		xvectors = (struct xyzcoords *)malloc(num_read);		

		size_t k;

		for (j=0; j<imdiff->vpixels; j++) {
		  for (k=0; k<imdiff->hpixels; k++) {
		    xvectors[index] = xvectors_cctbx[k*imdiff->vpixels + j];
		    index++;
		  }
		}
	      }

	      // Broadcast the xvectors to other ranks
	      lbcastBufMPI((void *)&num_read,sizeof(size_t),0,mpiv);
	      if (mpiv->my_id != 0) {
		if (xvectors != NULL) free(xvectors);
		xvectors = (struct xyzcoords *)malloc(num_read);
	      }
	      lbcastBufMPI((void *)xvectors,num_read,0,mpiv);
	      //	      Hlist = (struct xyzcoords *)malloc(num_read);
	      //	      dHlist = (struct xyzcoords *)malloc(num_read);
	      //	      ilist = (IJKCOORDS_DATA *)malloc(num_read);
	      //	      jlist = (IJKCOORDS_DATA *)malloc(num_read);
	      //	      klist = (IJKCOORDS_DATA *)malloc(num_read);
	    }
	  
	    // Calculate the image scale factor

	    lscaleim(imdiff_scale_ref,imdiff_scale);
	    float this_scale_factor = imdiff_scale_ref->rfile[0];
	    printf("Image %d scale factor, error = %f, %f\n",i,imdiff_scale_ref->rfile[0],imdiff_scale_ref->rfile[1]);

	    // Read amatrix

	    str_length = snprintf(NULL,0,amatrix_format,i);
	    
	    amatrix_path = (char *)malloc(str_length+1);
	    
	    sprintf(amatrix_path,amatrix_format,i);

	    num_read = lreadbuf((void **)&amatrix,amatrix_path);
	    
#ifdef DEBUG		
	    printf("(%f, %f, %f) ",amatrix->xx,amatrix->xy,amatrix->xz);
	    printf("(%f, %f, %f) ",amatrix->yx,amatrix->yy,amatrix->yz);
	    printf("(%f, %f, %f) ",amatrix->zx,amatrix->zy,amatrix->zz);
	    printf("\n");
#endif		

	    struct xyzmatrix *a,at;

	    // Calculate the transpose of a, to use matrix multiplication method

	    a = (struct xyzmatrix *)amatrix;

	    at = lmatt(*a);

	    // Calculate the rotated and scaled xvectors, yielding Miller indices

#ifdef DEBUG
	    /*	    if (i == 1) {
	    for (j = 50000;j<50010;j++) {
	      printf("(%f %f %f) ",xvectors[j].x,xvectors[j].y,xvectors[j].z);
	    }
	    printf("\n");
	    for (j=50000;j<50010;j++) {	      
	      printf("%d ",imdiff_scale->image[j]);
	    }
	    printf("\n");
	    }
	    */
#endif

	    // Collect the image data into the lattice

	    size_t data_added=0;
	    struct xyzcoords H, dH;
	    IJKCOORDS_DATA ii,jj,kk;
	    index = 0;
	    for (j=0; j<imdiff->image_length; j++) {
	      H = lmatvecmul(at, xvectors[j]);
#ifdef DEBUG
	      if (j<10) {
		printf("Image %d, H[%d] = (%f, %f, %f)\n",i,j,H.x,H.y,H.z);
	      }
#endif
	      dH.x = fabs(H.x - roundf(H.x));
	      dH.y = fabs(H.y - roundf(H.y));
	      dH.z = fabs(H.z - roundf(H.z));
	      if (filterhkl==0 || dH.x>=0.25 || dH.y>=0.25 || dH.z>=0.25) {
		ii = (IJKCOORDS_DATA)roundf(H.x*(float)pphkl) + i0;
		jj = (IJKCOORDS_DATA)roundf(H.y*(float)pphkl) + j0;
		kk = (IJKCOORDS_DATA)roundf(H.z*(float)pphkl) + k0;
		if (ii>=0 && ii<lat->xvoxels && jj>=0 && jj<lat->yvoxels &&
		    kk>=0 && kk<lat->zvoxels && imdiff_scale->image[index]>0 &&
		    imdiff_scale->image[index]!=imdiff->ignore_tag) {
		  size_t latidx = kk*lat->xyvoxels + jj*lat->xvoxels + ii;
		  if (filterhkl!=0) {
		    lat->lattice[latidx] += 
		      (LATTICE_DATA_TYPE)imdiff->image[index]
		      * imdiff->correction[index]
		      * this_scale_factor;
		  } else {
		    lat->lattice[latidx] += 
		      (LATTICE_DATA_TYPE)imdiff_scale->image[index]
		      * this_scale_factor;
		  }
		  latct[latidx] += 1.;
		  data_added += 1;
		}
	      }
	      index++;
	    }
#ifdef DEBUG
	    printf("data_added = %ld\n",data_added);
#endif
	  }
	  ct++;
	}

#ifdef DEBUG
	// Count number of data points in the lattice
	int num_data_points=0;
	float sum_data_points=0.0;

	for (j=0;j<lat->lattice_length;j++) {
	  if (latct[j] != 0) {
	    if (lat->lattice[j]/latct[j] < 32767.) {
	      num_data_points++;
	      sum_data_points += lat->lattice[j]/latct[j];
	    }
	  }
	}
	printf("num_data_points=%d, mean_data_points=%f\n",num_data_points,sum_data_points/(float)num_data_points);
#endif
	if (do_integrate != 0) {

	  // Merge the data and counts

	  LATTICE_DATA_TYPE *latsum, *latctsum;
	  
	  latsum = (LATTICE_DATA_TYPE *)calloc(lat->lattice_length,sizeof(LATTICE_DATA_TYPE));
	  latctsum = (LATTICE_DATA_TYPE *)calloc(lat->lattice_length,sizeof(LATTICE_DATA_TYPE));
	  
	  lreduceSumLatticeMPI(lat->lattice,latsum,lat->lattice_length,0,mpiv);
	  lreduceSumLatticeMPI(latct,latctsum,lat->lattice_length,0,mpiv);
	  
	  // Calculate the mean on the root rank and output the result
	  
	  if (mpiv->my_id == 0) {
	    for (j=0; j<lat->lattice_length; j++) {
	      if (latctsum[j] != 0) {
		lat->lattice[j] = latsum[j]/latctsum[j];
	      } else {
		lat->lattice[j] = lat->mask_tag;
	      }
	    }
	    
	    // output the result
	    
	    str_length = snprintf(NULL,0,"%s/%s.lat",lattice_dir,diffuse_lattice_prefix);
	    
	    char *latticeoutpath;
	    
	    latticeoutpath = (char *)malloc(str_length+1);
	    
	    sprintf(latticeoutpath,"%s/%s.lat",lattice_dir,diffuse_lattice_prefix);
	    
	    //	  printf("imageinpath = %s\n",imageinpath);
	    
	    FILE *latticeout;
	    
	    if ( (latticeout = fopen(latticeoutpath,"wb")) == NULL ) {
	      printf("Can't open %s.",latticeoutpath);
	      exit(1);
	    }
	    lat->outfile=latticeout;
	    lwritelt(lat);
	    fclose(latticeout);
	  }
	}
	lfinalMPI(mpiv);

}

