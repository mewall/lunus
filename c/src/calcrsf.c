/* CALCRSF.C - Calculate an R factor and other figures of merit from structure factors and hkl reflections.
   
   Author: Mike Wall
   Date: 5/4/2015
   Version: 1.
   
   Usage:
   		"calcrsf <input reflections .hkl> <input real sf> <input imag sf>"

		Input is a .hkl reflections file and real,imag structure factors in lattice format. 
			Output is R factor and other figures of merit to stdout.  
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *lat1in,
    *lat2in,
    *hklin;
  
  char
    error_msg[LINESIZE],filename[256];
  
  size_t
    lat_index,
    num_read,
    num_wrote;

  LAT3D 
    *lat1,*lat2;

  int
    i,
    hh,
    kk,
    ll,
    nn[4],
    c,r,s,cc,rr,ss,
    nfit;
  
  float
    I,sigI,Imodel,
    F,sigF,
    freal,fimag;
/*
 * Set input line defaults:
 */
  hklin = stdin;
  nfit=1;

/*
 * Read information from input line:
 */
	switch(argc) {
	  case 5:
	    nfit=atoi(argv[4]);
	  case 4:
	    if ((lat2in = fopen(argv[3],"rb")) == NULL) {
	      printf("\nCan't open %s.\n\n",argv[3]);
	      exit(0);
	    }
	  case 3:
	    if ( (lat1in = fopen(argv[2],"rb")) == NULL ) {
	      printf("\nCan't open %s.\n\n",argv[2]);
	      exit(0);
	    }
	  case 2:
	  if (strcmp(argv[1],"-") == 0) {
	    hklin = stdin;
	  }
	  else {
	    if ( (hklin = fopen(argv[1],"r")) == NULL ) {
	      printf("\nCan't open %s.\n\n",argv[1]);
	      exit(0);
	    }
	  }
	  break;
	  default:
	  printf("\n Usage: calcrsf <input reflections .hkl> <input real sf lat> <input imag sf lat>\n\n");
	  exit(0);
	}
  
  /*
   * Initialize lattices:
   */

  if ((lat1 = linitlt()) == NULL) {
    perror("Couldn't initialize lattice.\n\n");
    exit(0);
  }

  if ((lat2 = linitlt()) == NULL) {
    perror("Couldn't initialize lattice.\n\n");
    exit(0);
  }
  
  /*
   * Read in lattices:
   */

  lat1->filename = filename;
  lat1->infile = lat1in;
  if (lreadlt(lat1) != 0) {
    perror("Couldn't read map.\n\n");
    exit(0);
  }

  lat2->filename = filename;
  lat2->infile = lat2in;
  if (lreadlt(lat2) != 0) {
    perror("Couldn't read map.\n\n");
    exit(0);
  }

  float ignore_tag=-1.;

  /*
   * Read the .hkl reflections file one line at a time, and replace fft amplitudes with reflections. 
   * Assume intensities in hkl file.
   */

  float *Iobs,*sigmaIobs,*Fobs,*sigmaFobs,*Fcalc,*Icalc;
  Iobs = (float *)malloc((lat1->lattice_length)*sizeof(float));
  sigmaIobs = (float *)malloc((lat1->lattice_length)*sizeof(float));
  Fobs = (float *)malloc((lat1->lattice_length)*sizeof(float));
  sigmaFobs = (float *)malloc((lat1->lattice_length)*sizeof(float));
  Fcalc = (float *)malloc((lat1->lattice_length)*sizeof(float));
  Icalc = (float *)malloc((lat1->lattice_length)*sizeof(float));
  for (i=0;i<lat1->lattice_length;i++) {
    Iobs[i] = ignore_tag;
    Fobs[i] = ignore_tag;
    Icalc[i] = ignore_tag;
    Fcalc[i] = ignore_tag;
  }
  //  printf("Iobs[0]=%f\n",Iobs[0]);
  //  printf("Scaling the map\n");
  float xx_R=0.0, xy_R=0.0;
  float xx_goof=0.0,xy_goof=0.0;
  float xx_wR2_shelx=0.0,xy_wR2_shelx=0.0;
  float xx_wR2_ccp4=0.0,xy_wR2_ccp4=0.0;
  float w_ccp4,w_shelx;
  while (fscanf(hklin,"%d %d %d %f %f",&hh,&kk,&ll,&I,&sigI)!=EOF) {
    lat_index = ((ll+lat1->origin.k)%lat1->zvoxels)*lat1->xyvoxels+((kk+lat1->origin.j)%lat1->yvoxels)*lat1->xvoxels+((hh+lat1->origin.i)%lat1->xvoxels);
    if (lat_index > lat1->lattice_length) {
      printf("lat_index greater than lat1->lattice_length\n");
      exit(0);
    }
    freal=lat1->lattice[lat_index]; fimag=lat2->lattice[lat_index];
    Icalc[lat_index]=freal*freal+fimag*fimag;
    if (Icalc[lat_index]>0) {
      Fcalc[lat_index]=sqrtf(Icalc[lat_index]);
    }
    Iobs[lat_index] = I;
    sigmaIobs[lat_index] = sigI;
    if (I>0) {
      Fobs[lat_index] = sqrtf(I);
      sigmaFobs[lat_index] = sigI/2./Fobs[lat_index];
    }
    // accumulate statistics for calculation of overall scale factor
    if (Fobs[lat_index] != ignore_tag) {
      float xx = Fcalc[lat_index]*Fcalc[lat_index];
      float xy = Fobs[lat_index]*Fcalc[lat_index];
      xx_R += xx;
      xy_R += xy;
      w_ccp4 = 1./sigmaFobs[lat_index]/sigmaFobs[lat_index];
      xx_wR2_ccp4 += xx*w_ccp4; xy_wR2_ccp4 += xy*w_ccp4;
      w_shelx = 1./sigI/sigI;
      xx_wR2_shelx += Icalc[lat_index]*Icalc[lat_index]*w_shelx; 
      xy_wR2_shelx += Iobs[lat_index]*Icalc[lat_index]*w_shelx;
    }
    //      float p=(2.*Imodel+I)/3.;
    //w_shelx = 1./(sigI*sigI+(0.1*p)*(0.1*p));      
    //      xx_goof += I*I*w_shelx; xy_goof += I*Imodel*w_shelx;
    //      friedel_index = (s*map->section_length+r*map->nc+c)*2+1;
    //      scaled_fft_data[friedel_index] = F/Fmodel*fft_data[friedel_index];
    //      scaled_fft_data[friedel_index+1] = F/Fmodel*fft_data[friedel_index+1];
  }

  // Calculate scale factors

  float scale_R = xx_R/xy_R; // overall scale factor
  float scale_wR2_ccp4 = xx_wR2_ccp4/xy_wR2_ccp4; // overall scale factor
  float scale_wR2_shelx = xx_wR2_shelx/xy_wR2_shelx; // overall scale factor
  //  float scale_goof = xy_goof/xx_goof; // overall scale factor

  // calculate figures of merit

  //  printf(" Scale factors: %f %f %f\n",scale_R,scale_wR2_ccp4,scale_wR2_shelx);

  float R_num=0,R_denom=0,wR2_ccp4_num=0,wR2_ccp4_denom=0,wR2_shelx_num=0,wR2_shelx_denom=0;
  long ndat = 0;
  //  printf("Iobs[0],sigmaIobs[0]=%f,%f",Iobs[0],sigmaIobs[0]);
  for (i=0;i<lat1->lattice_length;i++) {
    if (Iobs[i] != ignore_tag) {
      ndat++;
      if (sigmaIobs[i]<=0) {
	printf("sigmaIobs <= 0 for index %d\n",i);
	exit(0);
      }
      w_shelx = 1./sigmaIobs[i]/sigmaIobs[i];
      wR2_shelx_num += w_shelx*powf((Iobs[i]-Icalc[i]/scale_wR2_shelx),2.);
      wR2_shelx_denom += w_shelx*powf(Iobs[i],2.);
      //      float p = (2.*Imodel/scale_wR2_shelx+I)/3.;
      //      w_shelx = 1./(sigsqI+(0.1*p)*(0.1*p));
      if (Iobs[i] > 0) {
	w_ccp4 = 1./sigmaFobs[i]/sigmaFobs[i];
	R_num += fabs(Fobs[i]-Fcalc[i]/scale_R);
	R_denom += Fobs[i];
	wR2_ccp4_num += w_ccp4*powf((Fobs[i]-Fcalc[i]/scale_wR2_ccp4),2.);
	wR2_ccp4_denom += w_ccp4*powf(Fobs[i],2.);
      }
    }
  }
float R = R_num/R_denom;
float wR2_ccp4 = sqrtf(wR2_ccp4_num/wR2_ccp4_denom);
float wR2_shelx = sqrtf(wR2_shelx_num/wR2_shelx_denom);
float goof = sqrtf(wR2_shelx_num/((float)ndat-(float)nfit));

printf("%11.9f %11.9f %11.9f %11.9f\n",R,wR2_ccp4,wR2_shelx,goof);


CloseShop:
  
  /*
   * Close files:
   */
  
  fclose(hklin);
  fclose(lat1in);
  fclose(lat2in);
}

