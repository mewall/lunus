/* LUNUS.C - Diffuse scattering analysis and modeling.
   
   Author: Mike Wall  
   Date: 6/13/2017
   Version: 1.
   
   "lunus <input_deck>"

   Apply lunus diffuse scattering methods according to instructions in input_deck
   
   */

#include<lunus.h>

int main(int argc, char *argv[])
{
  FILE
        *deckin,
	*imagein,
        *lunusout,
        *scaleout;
  
  char
    inputdeck[10000],
    *imageinpath = NULL,
    *lunusoutpath = NULL,
    *scaleoutpath = NULL,
    *deck = NULL,    
    *amatrix_format = NULL,
    *amatrix_path = NULL,
    *xvectors_path = NULL,
    *writevtk_str = NULL,
    *integration_image_type = NULL,
    *lattice_dir = NULL,
    *diffuse_lattice_prefix = NULL,
    *spacegroup = NULL,
    *imagelist_name = NULL,
    *jsonlist_name = NULL,
    *imagelist[20000],
    *bkglist[20000],
    **matched_pair,
    *jsonlist[20000],
    error_msg[LINESIZE];

  void *buf;

  int
    writevtk,
    do_integrate = 1,
    filterhkl = 1;

  struct xyzcoords *xvectors_cctbx = NULL,*xvectors = NULL, *Hlist, *dHlist;

  IJKCOORDS_DATA *ilist, *jlist, *klist;

  struct xyzmatrix at[20000];

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
    resolution;

  int
    pphkl = 1;

  DIFFIMAGE 
    *imdiff = NULL, *imdiff_bkg = NULL, *imdiff_corrected = NULL, *imdiff_scale = NULL, *imdiff_scale_ref = NULL;

  LAT3D
    *lat;

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

#ifdef DEBUG
	//	printf("LUNUS: reading input deck\n");
#endif
	// Read input deck into buffer

	if (mpiv->my_id == 0) {
	  num_read = lreadbuf((void **)&deck,inputdeck);
	}

	// Broadcast num_read and the input deck

	lbcastBufMPI((void *)&num_read,(int)sizeof(size_t),0,mpiv);

        if (mpiv->my_id != 0) {
	  deck = (char *)calloc(num_read+1,sizeof(char));
	}

	lbcastBufMPI((void *)deck,(int)num_read,0,mpiv);



#ifdef DEBUG
	//	printf("Length of input deck = %ld\n",num_read);
#endif
	// Parse input deck

	if (strstr(deck,"\nwritevtk") == NULL) {
	  writevtk_str = (char *)malloc(strlen("False")+1);
	  strcpy(writevtk_str,"False");
	} else {
	  writevtk_str = lgettag(deck,"\nwritevtk");
	}

	if (strcmp(writevtk_str,"True")==0) {
	  writevtk=1;
	} else {
	  writevtk=0;
	}

	if (strstr(deck,"\nimagelist_name") != NULL) {
	  imagelist_name=lgettag(deck,"\nimagelist_name");
	}

	if (strstr(deck,"\njsonlist_name") != NULL) {
	  jsonlist_name=lgettag(deck,"\njsonlist_name");
	}

	if (strstr(deck,"\nintegration_image_type") == NULL) {
	  integration_image_type = (char *)malloc(strlen("raw")+1);
	  strcpy(integration_image_type,"raw");
	} else {
	  integration_image_type=lgettag(deck,"\nintegration_image_type");
	}

	if (strstr(deck,"\nfilterhkl") == NULL) {
	  filterhkl = 1;
	} else if (strcmp(lgettag(deck,"\nfilterhkl"),"False")==0) {
	  filterhkl=0;
	} 

	if (strstr(deck,"\npphkl") == NULL) {
	  pphkl = 1;
	} else {
	  pphkl = lgettagi(deck,"\npphkl");
	}

	if (strstr(deck,"\npoints_per_hkl") != NULL) {
	  pphkl = lgettagi(deck,"\npoints_per_hkl");
	}

	if (strstr(deck,"\nspacegroup") != NULL) {
	  spacegroup=lgettag(deck,"\nspacegroup");
	}

	if (strstr(deck,"\nresolution") == NULL) {
	  resolution = -1.;
	} else {
	  resolution = lgettagf(deck,"\nresolution");
	}

	if (strstr(deck,"\nxvectors_path") == NULL) {
	  if (do_integrate!=0) {
	    perror("Must provide xvectors_path for integration\n");
	    exit(1);
	  }
	} else {
	  xvectors_path=lgettag(deck,"\nxvectors_path");
	}

	if (strstr(deck,"\namatrix_format") != NULL) {
	  amatrix_format=lgettag(deck,"\namatrix_format");
	}

	if (strstr(deck,"\nlattice_dir") == NULL) {
	  lattice_dir = (char *)malloc(strlen(".")+1);
	  strcpy(lattice_dir,".");
	} else {
	  lattice_dir=lgettag(deck,"\nlattice_dir");
	}

	if (strstr(deck,"\ndiffuse_lattice_dir") != NULL) {
	  lattice_dir = lgettag(deck,"\ndiffuse_lattice_dir");
	}

	if (strstr(deck,"\ndiffuse_lattice_prefix") == NULL) {
	  diffuse_lattice_prefix = (char *)malloc(strlen("diffuse_lunus")+1);
	  strcpy(diffuse_lattice_prefix,"diffuse_lunus");
	} else {
	  diffuse_lattice_prefix=lgettag(deck,"\ndiffuse_lattice_prefix");
	}

	// Get file lists and A matrices

	if (mpiv->my_id == 0) {

	  if (imagelist_name != NULL) printf("imagelist_name=%s\n",imagelist_name);

	  if (jsonlist_name != NULL) printf("jsonlist_name=%s\n",jsonlist_name);
	  
	  printf("integration_image_type=%s\n",integration_image_type);

	  printf("lattice_dir=%s\n",lattice_dir);
	  
	  printf("diffuse_lattice_prefix=%s\n",diffuse_lattice_prefix);

	  printf("resolution=%f\n",resolution);

	  printf("pphkl=%d\n",pphkl);

	  if (filterhkl == 1) {
	    printf("filterhkl=True\n");
	  } else {
	    printf("filterhkl=False\n");
	  }

	}

	if (mpiv->my_id == 0) {

	  if (imagelist_name == NULL && jsonlist_name == NULL) {
	    
	    perror("Can't generate image list due to NULL value of imagelist_name or jsonlist_name.\n");
	    
	  } else if (jsonlist_name != NULL) {

	    // Obtain image filenames and crystal orientations from .json files

	    FILE *f;

	    if ((f = fopen(jsonlist_name,"r")) == NULL) {
	      printf("Can't open %s.\n",jsonlist_name);
	      exit(1);
	    }
	  
	    size_t bufsize = LINESIZE;
	    char *json_name;

	    i = 0;

	    json_name = (char *)calloc(LINESIZE+1,sizeof(char));

	    int chars_read;

	    while ((chars_read = getline(&json_name,&bufsize,f)) != -1) {

	      json_name[chars_read-1]=0;

	      if ((readExptJSON(&at[i],&imagelist[i],&bkglist[i],json_name)) != 0) {
		printf("Skipping %s, unable to read\n",buf);
	      } else {
		//	      printf("%s,%s\n",imagelist[i],bkglist[i]);
		i++;
	      }

	    }

	    num_images = i;

	  } else {

	    // Read image list and A matrix files

	    FILE *f;

	    if ((f = fopen(imagelist_name,"r")) == NULL) {
	      printf("Can't open %s.\n",imagelist_name);
	      exit(1);
	    }
	  
	    size_t bufsize = LINESIZE;
	    char *buf;

	    i = 0;

	    buf = (char *)malloc(LINESIZE+1);

	    int chars_read;

	    while ((chars_read = getline(&buf,&bufsize,f)) != -1) {
	    
	      // Read amatrix
	    
	      if ((readAmatrix(&at[i],amatrix_format,i+1) == -1)) {
		printf("Missing amatrix file %s. Skipping frame %d.\n",amatrix_path,i);
	      } else {
	      
	      
		buf[chars_read-1]=0;
	      
		str_length = snprintf(NULL,0,"%s",buf);
	      
		imagelist[i] = (char *)malloc(str_length+1);
	      
		sprintf(imagelist[i],"%s",buf);
	      
		//	    printf("%s\n",imagelist[i]);
	      
		i++;
	      }
	    
	    }
	    num_images = i;
	  }
	}

	// Broadcast file lists from rank 0 to other MPI ranks

	lbcastBufMPI((void *)&num_images,sizeof(size_t),0,mpiv);
	int il_sz[num_images];
	int bl_sz[num_images];
	if (mpiv->my_id == 0) {
	  for (i=0;i<num_images;i++) {
	    il_sz[i] = strlen(imagelist[i])+1;
	    bl_sz[i] = strlen(bkglist[i])+1;
	  }
	}
	lbcastBufMPI((void *)&il_sz,sizeof(int)*num_images,0,mpiv);
	lbcastBufMPI((void *)&bl_sz,sizeof(int)*num_images,0,mpiv);
	for (i=0;i<num_images;i++) {
	  if (mpiv->my_id != 0) {
	    imagelist[i] = (char *)malloc(il_sz[i]);
	    bkglist[i] = (char *)malloc(bl_sz[i]);
	  }
	  lbcastBufMPI((void *)imagelist[i],il_sz[i],0,mpiv);
	  lbcastBufMPI((void *)bkglist[i],bl_sz[i],0,mpiv);
	}

	// Broadcast the list of A matrices to all MPI ranks

	lbcastBufMPI((void *)&at,sizeof(struct xyzmatrix)*num_images,0,mpiv);	

	// Initialize the 3D dataset for this MPI rank

	lat = linitlt();

	// Get lattice params from input deck

	lat->params = deck;

	lsetparamslt(lat);

	if (strstr(lat->cell_str,"None") != NULL) {
	    float a,b,c,alpha,beta,gamma,adotb,adotc,bdotc;
	    struct xyzmatrix a0;
	    a0 = at[0];
	    a = sqrtf(a0.xx*a0.xx+a0.xy*a0.xy+a0.xz*a0.xz);
	    b = sqrtf(a0.yx*a0.yx+a0.yy*a0.yy+a0.yz*a0.yz);
	    c = sqrtf(a0.zx*a0.zx+a0.zy*a0.zy+a0.zz*a0.zz);
	    adotb = a0.xx*a0.yx + a0.xy*a0.yy + a0.xz*a0.yz;
	    adotc = a0.xx*a0.zx + a0.xy*a0.zy + a0.xz*a0.zz;
	    bdotc = a0.yx*a0.zx + a0.yy*a0.zy + a0.yz*a0.zz;
	    alpha = acosf(bdotc/b/c)*180./PI;
	    beta = acosf(adotc/a/c)*180./PI;
	    gamma = acosf(adotb/a/b)*180./PI;
	    //	    str_length = snprintf(NULL,0,"%f,%f,%f,%f,%f,%f",a,b,c,alpha,beta,gamma);
	    //	    unit_cell = (char *)calloc(str_length+1,sizeof(char));
	    sprintf(lat->cell_str,"%f,%f,%f,%f,%f,%f",a,b,c,alpha,beta,gamma);
	    printf("Calculated unit cell from first amatrix=%s\n",lat->cell_str);
	  }
	  lparsecelllt(lat);
	  lat->cell.a *= lat->pphkl;
	  lat->cell.b *= lat->pphkl;
	  lat->cell.c *= lat->pphkl;
	  lat->xvoxels = ((int)(lat->cell.a/lat->resolution.max)+1)*2;
	  lat->yvoxels = ((int)(lat->cell.b/lat->resolution.max)+1)*2;
	  lat->zvoxels = ((int)(lat->cell.c/lat->resolution.max)+1)*2;
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
	  lat->latct = (LATTICE_DATA_TYPE *)calloc(lat->lattice_length,sizeof(size_t));


	/*
	 * Initialize diffraction image:
	 */


	if ((imdiff = linitim()) == NULL) {
	  perror("Couldn't initialize diffraction image.\n\n");
	  exit(0);
	}

	// Define parameters from input deck

	imdiff->params = deck;

	lsetparamsim(imdiff);

	// Set mpi variables for imdiff

	imdiff->mpiv = mpiv;

	// Read the xvectors on rank 0
	if (imdiff->mpiv->my_id == 0) {
	  num_read = lreadbuf((void **)&xvectors_cctbx,xvectors_path);
	  if (num_read != 3*imdiff->image_length*sizeof(float)) {
	    perror("LUNUS: Number of xvectors differs from number of pixels in image.\n");
	    exit(1);
	  }
#ifdef DEBUG		
	  printf("SAMPLES\n");
	  for (j=50000;j<50010;j++) {
	    printf("(%f, %f, %f): %d\n",xvectors_cctbx[j].x,xvectors_cctbx[j].y,xvectors_cctbx[j].z, imdiff_corrected->image[j]);
	  }
	  printf("\n");
#endif		
	  // Reorder the xvectors (transpose)

	  index = 0;

	  if (xvectors != NULL) free(xvectors);

	  xvectors = (struct xyzcoords *)malloc(num_read);		

	  if (num_read != sizeof(struct xyzcoords)*imdiff->image_length) {
	    perror("Number of xvectors not equal to image length. Exiting.\n");
	    exit(1);;
	  }
		   
	  size_t k;

	  for (j=0; j<imdiff->vpixels; j++) {
	    for (k=0; k<imdiff->hpixels; k++) {
	      // The following conditional is needed to prevent a segfault-inducing Intel 18.X optimization error:
	      //		    if (index == 0) printf("");
	      //		    xvectors[k*imdiff->vpixels + j] = xvectors_cctbx[index];
	      xvectors[index] = xvectors_cctbx[k*imdiff->vpixels + j];
	      index++;
	    }
	  }
	}

	// Broadcast the xvectors to other ranks
	lbcastBufMPI((void *)&num_read,sizeof(size_t),0,imdiff->mpiv);
	if (imdiff->mpiv->my_id != 0) {
	  if (xvectors != NULL) free(xvectors);
	  xvectors = (struct xyzcoords *)malloc(num_read);
	}
	lbcastBufMPI((void *)xvectors,num_read,0,imdiff->mpiv);
	//	      Hlist = (struct xyzcoords *)malloc(num_read);
	//	      dHlist = (struct xyzcoords *)malloc(num_read);
	//	      ilist = (IJKCOORDS_DATA *)malloc(num_read);
	//	      jlist = (IJKCOORDS_DATA *)malloc(num_read);
	//	      klist = (IJKCOORDS_DATA *)malloc(num_read);
	imdiff->xvectors = xvectors;

	// Process all of the images

	int ct=0;

	// Process the images on this rank yielding a partial sum for the 3D dataset

	for (i=mpiv->my_id+1;i<=num_images;i=i+mpiv->num_procs) {

	  /*
	   * Read diffraction image:
	   */
	  
	  if ( (imagein = fopen(imagelist[i-1],"rb")) == NULL ) {
	    printf("Can't open %s.",imagelist[i-1]);
	    exit(0);
	  }

	  imdiff->infile = imagein;
	  if (lreadim(imdiff) != 0) {
	    perror(imdiff->error_msg);
	    exit(0);
	  }

	  fclose(imagein);

	  // Associate an A matrix with this image

	  imdiff->amatrix = at[i-1];

	  // Define image parameters from input deck

	  imdiff->params = deck;

	  lsetparamsim(imdiff);

	  // Subtract background image if available:

	  int needs_bkgsub = 0;

	  if (strlen(bkglist[i-1]) > 0) {

#ifdef DEBUG
	    printf("Matched pair:%s,%s\n",imagelist[i-1],bkglist[i-1]);
#endif
	    needs_bkgsub = 1;
	    if ( (imagein = fopen(bkglist[i-1],"rb")) == NULL ) {
	      printf("Can't open %s.",bkglist[i-1]);
	      exit(0);
	    }
	    imdiff_bkg->infile = imagein;
	    if (lreadim(imdiff_bkg) != 0) {
	      perror(imdiff_bkg->error_msg);
	      exit(0);
	    }
	    fclose(imagein);
	  }

	  if (needs_bkgsub == 1) lbkgsubim(imdiff,imdiff_bkg);

	  // Initialize other images
	  
	  if (imdiff_corrected == NULL) imdiff_corrected = linitim();
	  if (imdiff_scale == NULL) imdiff_scale = linitim();
	  if (imdiff_scale_ref == NULL) imdiff_scale_ref = linitim();
	  if (imdiff_bkg == NULL) imdiff_bkg = linitim();

	  // Apply masks

	  lpunchim(imdiff);
	  lwindim(imdiff);
	  lthrshim(imdiff);

	  // Mode filter to create image to be used for scaling

	  lcloneim(imdiff_scale,imdiff);

	  lmodeim(imdiff_scale);

	  // Calculate correction factor

	  lcfim(imdiff);	  

	  // Calculate corrected image

	  lcloneim(imdiff_corrected,imdiff);
	  if (lmulcfim(imdiff_corrected) != 0) {
	    perror(imdiff_corrected->error_msg);
	    exit(1);
	  }

	    
	  // Set up common variables on the first pass

	  if (ct == 0) {
	    // Reference image for scaling
	    lcloneim(imdiff_scale_ref,imdiff_scale);
#ifdef DEBUG
	    printf("Rank %d, imdiff_scale_ref->image_length = %ld,imdiff_scale_ref->overload_tag=%d,imdiff_scale_ref->ignore_tag=%d,imdiff_scale_ref->value_offset=%d,",imdiff->mpiv->my_id,imdiff_scale_ref->image_length,imdiff_scale_ref->overload_tag,imdiff_scale_ref->ignore_tag,imdiff_scale_ref->value_offset);
#endif
	    //	      lbarrierMPI(mpiv);
	    //	      printf("Barrier 1 passed\n");
	    // Use the rank 0 image
	    lbarrierMPI(imdiff->mpiv);
	    // Broadcast the image data
	    lbcastImageMPI(imdiff_scale_ref->image,imdiff_scale_ref->image_length,0,imdiff->mpiv);
	    // Broadcast the pedestal as well -- this is critical
	    lbcastBufMPI((void *)&imdiff_scale_ref->value_offset,sizeof(IMAGE_DATA_TYPE),0,imdiff->mpiv);
#ifdef DEBUG
	    int num_nz=0;
	    size_t num_ign = 0;
	    float sum_vals = 0.0;
	    for (j=0; j<imdiff_scale_ref->image_length; j++) {	      
	      if (imdiff_scale_ref->image[j] != imdiff_scale_ref->overload_tag && imdiff_scale_ref->image[j] != 0) {
		num_nz++;
		sum_vals += imdiff_scale_ref->image[j];
		//	      printf("image[%d]=%d,",j,imdiff_scale_ref->image[j]);
	      } else num_ign ++;
	      //	      if (num_nz>10) break;
	    }
	    printf("num_ign = %ld,avg = %g,",num_ign,sum_vals/(float)num_nz);
	    lavgrim(imdiff_scale_ref);
	    for (j=100; j<110;j++) {
	      if (j>100 && j<=110)  printf("rf[%d]=%f,",j,imdiff_scale_ref->rfile[j]);
	    }
	    printf("\n");
#endif
	    //	      lbarrierMPI(mpiv);
	    //	      printf("Barrier 2 passed\n");
	  }
	  
	  // Calculate the image scale factor

	  lscaleim(imdiff_scale_ref,imdiff_scale);
	  float this_scale_factor = imdiff_scale_ref->rfile[0];
	  printf("Image %d scale factor, error = %f, %f\n",i,imdiff_scale_ref->rfile[0],imdiff_scale_ref->rfile[1]);


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
	    H = lmatvecmul(imdiff->amatrix, imdiff->xvectors[j]);
#ifdef DEBUG
	    if (j<10) {
	      printf("Image %d, H[%d] = (%f, %f, %f)\n",i,j,H.x,H.y,H.z);
	    }
#endif
	    dH.x = fabs(H.x - roundf(H.x));
	    dH.y = fabs(H.y - roundf(H.y));
	    dH.z = fabs(H.z - roundf(H.z));
	    if (lat->filterhkl==0 || dH.x>=0.25 || dH.y>=0.25 || dH.z>=0.25) {
	      ii = (IJKCOORDS_DATA)roundf(H.x*(float)lat->pphkl) + i0;
	      jj = (IJKCOORDS_DATA)roundf(H.y*(float)lat->pphkl) + j0;
	      kk = (IJKCOORDS_DATA)roundf(H.z*(float)lat->pphkl) + k0;
	      if (ii>=0 && ii<lat->xvoxels && jj>=0 && jj<lat->yvoxels &&
		  kk>=0 && kk<lat->zvoxels && imdiff_scale->image[index]>0 &&
		  imdiff_scale->image[index]!=imdiff->ignore_tag) {
		size_t latidx = kk*lat->xyvoxels + jj*lat->xvoxels + ii;
		if (strcmp(lat->integration_image_type,"raw")==0) {
		  lat->lattice[latidx] += 
		    (LATTICE_DATA_TYPE)(imdiff->image[index]-imdiff->value_offset)
		    * imdiff->correction[index]
		    * this_scale_factor;
		}
		if (strcmp(lat->integration_image_type,"corrected")==0) {
		  lat->lattice[latidx] += 
		    (LATTICE_DATA_TYPE)(imdiff_corrected->image[index]-imdiff_corrected->value_offset)
		    * this_scale_factor;
		}
		if (strcmp(lat->integration_image_type,"scale")==0) {
		  lat->lattice[latidx] += 
		    (LATTICE_DATA_TYPE)(imdiff_scale->image[index]-imdiff_scale->value_offset)
		    * imdiff->correction[index]
		    * this_scale_factor;
		}
		lat->latct[latidx] += 1;
		data_added += 1;
	      }
	    }
	    index++;
	  }
#ifdef DEBUG
	  printf("data_added = %ld\n",data_added);
#endif
	  ct++;
	}

#ifdef DEBUG
	  // Count number of data points in the lattice
	  int num_data_points=0;
	  float sum_data_points=0.0;
	  
	  for (j=0;j<lat->lattice_length;j++) {
	    if (lat->latct[j] != 0) {
	      if (lat->lattice[j]/(float)lat->latct[j] < 32767.) {
		num_data_points++;
		sum_data_points += lat->lattice[j]/(float)lat->latct[j];
	      }
	    }
	  }
	  printf("num_data_points=%d, mean_data_points=%f\n",num_data_points,sum_data_points/(float)num_data_points);
#endif

	  // Merge the data and counts

	  LATTICE_DATA_TYPE *latsum;
	  size_t *latctsum;
	  
	  latsum = (LATTICE_DATA_TYPE *)calloc(lat->lattice_length,sizeof(LATTICE_DATA_TYPE));
	  latctsum = (size_t *)calloc(lat->lattice_length,sizeof(size_t));
	  
	  lreduceSumLatticeMPI(lat->lattice,latsum,lat->lattice_length,0,mpiv);
	  lreduceSumLatctMPI(lat->latct,latctsum,lat->lattice_length,0,mpiv);
	  
	  // Calculate the mean on the root rank and output the result
	  
	  if (mpiv->my_id == 0) {
	    for (j=0; j<lat->lattice_length; j++) {
	      if (latctsum[j] != 0) {
		lat->lattice[j] = latsum[j]/(float)latctsum[j];
		if isnan(lat->lattice[j]) lat->lattice[j] = lat->mask_tag;
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
	      printf("Can't open %s.\n",latticeoutpath);
	      exit(1);
	    }
	    lat->outfile=latticeout;
	    lwritelt(lat);
	    fclose(latticeout);
	    // Write .vtk if requested

	    if (writevtk != 0) {

	      str_length = snprintf(NULL,0,"%s/%s.vtk",lattice_dir,diffuse_lattice_prefix);
	    
	      char *vtkoutpath;
	      
	      vtkoutpath = (char *)malloc(str_length+1);
	      
	      sprintf(vtkoutpath,"%s/%s.vtk",lattice_dir,diffuse_lattice_prefix);
	      
	      //	  printf("imageinpath = %s\n",imageinpath);
	      
	      FILE *vtkout;
	      
	      if ( (vtkout = fopen(vtkoutpath,"w")) == NULL ) {
		printf("Can't open %s.\n",vtkoutpath);
		exit(1);
	      }
	      
	      lat->outfile=vtkout;
	      if (spacegroup != NULL) {
		strcpy(lat->space_group_str,spacegroup);
	      } else {
		strcpy(lat->space_group_str,"Unknown");
	      }
	      lwritevtk(lat);
	    }
	  }
	lfinalMPI(mpiv);

}

