/* LPROCIMLT.C - Process an image and accumulate the data on a target lattice.

   Author: Mike Wall
   Date: 4/23/2019
   Version: 1.

*/

#include<mwmask.h>
#include <string.h>

int lprocimlt(LAT3D *lat) 
{
  static int ct = 0;
  static DIFFIMAGE 
    *imdiff_corrected = NULL, *imdiff_scale = NULL, *imdiff_scale_ref = NULL;
  IJKCOORDS_DATA
    i0, j0, k0;
  DIFFIMAGE *imdiff = lat->imdiff;

  // Initialize other images
  
  if (imdiff_corrected == NULL) imdiff_corrected = linitim();
  if (imdiff_scale == NULL) imdiff_scale = linitim();
  if (imdiff_scale_ref == NULL) imdiff_scale_ref = linitim();

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

  if (lat->procmode == 0) {
    
    // Reference image for scaling
    lcloneim(imdiff_scale_ref,imdiff_scale);

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


    if (strstr(lat->cell_str,"None") != NULL) {
      float a,b,c,alpha,beta,gamma,adotb,adotc,bdotc;
      struct xyzmatrix a0;
      a0 = imdiff->amatrix;
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
    ct = 0;
    if (lat->lattice != NULL) free(lat->lattice);
    lat->lattice = (LATTICE_DATA_TYPE *)calloc(lat->lattice_length,sizeof(LATTICE_DATA_TYPE));
    if (lat->latct != NULL) free(lat->latct);
    lat->latct = (size_t *)calloc(lat->lattice_length,sizeof(size_t));

    return(0);
  }
	  
  // Calculate the image scale factor

  lscaleim(imdiff_scale_ref,imdiff_scale);
  float this_scale_factor = imdiff_scale_ref->rfile[0];

  // Collect the image data into the lattice

  size_t data_added=0;
  struct xyzcoords H, dH;
  IJKCOORDS_DATA ii,jj,kk;
  size_t index = 0;
  size_t j;
  i0 = (IJKCOORDS_DATA)(lat->xvoxels/2. - 1.);
  j0 = (IJKCOORDS_DATA)(lat->yvoxels/2. - 1.);
  k0 = (IJKCOORDS_DATA)(lat->zvoxels/2. - 1.);
  
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

  ct++;
}


