/* GENMAP.C - Create a 3D map of the scattering intensity of a single 
		diffraction image.
   
   Author: Mike Wall
   Date: 3/20/93
   Version: 1.
   Date: 2/6/95
   Version: 2.
      Use orientation matrix to calculate absolute reciprocal lattice vector.
   
   "genmap <image in> <output file > <spindle_deg> <scale> <hsize> <vsize> 
		<header_length>"

   Input is diffraction image, spindle position and scale.  Output is a
   list of voxels (float x, float y, float z, float value) which
   represents a slice of reciprocal space. 

   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
        *infile,
	*outfile,
	*imagein;
  
  short
	samples_per_row,
	samples_per_column,
	sample_pitch = DEFAULT_SAMPLE_PITCH;

  char
    error_msg[LINESIZE];

  size_t
    num_values,
    num_wrote,
    num_read;

  long   
	i,
	j,
	k;

  DIFFIMAGE 
	*imdiff;

  struct voxel
	*map3D;

  float
	scale,
	spindle_deg;

/*
 * Set input line defaults:
 */
	
	imagein = stdin;
	spindle_deg = DEFAULT_SPINDLE_DEG;
	scale = 1.;

/*
 * Read information from input line:
 */
	switch(argc) {
	  case 6:
	  scale = atof(argv[5]);
	  case 5:
	  spindle_deg = atof(argv[4]);
	  case 4:
	  if (strcmp(argv[3], "-") == 0) {
	    outfile = stdout;
	  }
	  else {
	    if ( (outfile = fopen(argv[3],"wb")) == NULL ) {
	      printf("Can't open %s.",argv[3]);
	      exit(0);
	    }
	  }
	  case 3:
	  if (strcmp(argv[2], "-") == 0) {
	    imagein = stdin;
	  }
	  else {
	    if ( (imagein = fopen(argv[2],"rb")) == NULL ) {
	      printf("Can't open %s.",argv[2]);
	      exit(0);
	    }
	  }
	  case 2:
	  if ( (infile = fopen(argv[1],"r")) == NULL ) {
	    printf("Can't open %s.",argv[1]);
	    exit(0);
	  }
	  break;
	  default:
	  printf("\n Usage: genmap <input file> "
		 "<image in> <output file> <spindle_deg> "
		 "<scale> \n\n");
	  exit(0);
	}
  
/*
 * Initialize diffraction image:
 */

  if ((imdiff = linitim()) == NULL) {
    perror("Couldn't initialize diffraction image.\n\n");
    exit(0);
  }

/*
 * Set main defaults:
 */


	imdiff->spindle_deg = spindle_deg;

  samples_per_row = imdiff->hpixels / sample_pitch;
  samples_per_column = imdiff->vpixels / sample_pitch;  
/***/printf("per row, column = %d, %d\n",samples_per_row, samples_per_column);
  map3D = (struct voxel *)malloc(sizeof(struct voxel) * samples_per_row *
					samples_per_column);
  if (!map3D) {
    perror("\nCan't allocate 3D lattice.\n\n");
    exit(0);
  }

/*
 * Read input file which contains the orientation matrix:
 */

  imdiff->infile = infile;
  lgetmat(imdiff);

/*
 * Read diffraction image:
 */

  imdiff->infile = imagein;
  if (lreadim(imdiff) != 0) {
    perror(imdiff->error_msg);
    goto CloseShop;
  }

/*
 * Step through the image and generate the corresponsing portion of the 3D map:
 */

  k = 0;
  for (i=0; i<samples_per_column; i++) {
    imdiff->pos.r = i*sample_pitch; 
    for (j=0; j<samples_per_row; j++) {
      imdiff->pos.c = j*sample_pitch;
      imdiff->map3D = &map3D[k];
      lgensv(imdiff);
      k++;
    }
  }
  num_values = k;

/*
 * Scale the values to the input line scale:
 */

  for(k=0; k<num_values; k++) {
	map3D[k].value *= scale;
  }
  
  /*
   * Write the output file:
   */

  num_wrote = fwrite(map3D, sizeof(struct voxel), samples_per_row * 
				samples_per_column, outfile); 
  if (num_wrote != samples_per_row * samples_per_column){
    printf("Only wrote %ld voxels.", num_wrote);
  }
  
CloseShop:
  
/*
 * Free allocated memory:
 */

  lfreeim(imdiff);
  free((struct voxel *)map3D);

  /*
   * Close files:
   */
  
  fclose(infile);
  fclose(imagein);
  fclose(outfile);
  
}

