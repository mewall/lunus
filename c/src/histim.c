/* HISTIM.C - Histogram a specified patch in an image.
   
   Author: Mike Wall  
   Date: 3/12/95
   Version: 1.
   
   "histim <image in> <x_lower_bound> <x_upper_bound> <y_lower_bound> 
		<y_upper_bound> <histogram output file>"

   Input is diffraction image and boundaries of patch.  Output is
   histogram of pixel values in the patch, in ascii format.
   
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*imagein,
	*histout;
  
  char
    error_msg[LINESIZE];

  size_t   
    i,
    j,
    index = 0;

  DIFFIMAGE 
	*imdiff;

  struct rccoords
	upper_bound,
	lower_bound;

  size_t
    *histogram;

  int
    binsize=1;

/*
 * Set input line defaults:
 */
	upper_bound.r = DEFAULT_WINDOW_UPPER;
	upper_bound.c = DEFAULT_WINDOW_UPPER;
	lower_bound.r = DEFAULT_WINDOW_LOWER;
	lower_bound.c = DEFAULT_WINDOW_LOWER;
	imagein = stdin;
	histout = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
	  case 8:
	  binsize = atoi(argv[7]);
		case 7:
			if (strcmp(argv[6], "-") == 0) {
			       histout = stdout;
			}
			else {
			 if ( (histout = fopen(argv[6],"w")) == NULL ) {
				printf("Can't open %s.",argv[6]);
				exit(0);
			 }
			}
		case 6:
			upper_bound.r = (RCCOORDS_DATA)atol(argv[5]);
		case 5:
			lower_bound.r = (RCCOORDS_DATA)atol(argv[4]);
		case 4:
			upper_bound.c = (RCCOORDS_DATA)atol(argv[3]);
		case 3:
			lower_bound.c = (RCCOORDS_DATA)atol(argv[2]);
		case 2:
			if (strcmp(argv[1], "-") == 0) {
				imagein = stdin;
			}
			else {
			 if ( (imagein = fopen(argv[1],"rb")) == NULL ) {
				printf("Can't open %s.",argv[1]);
				exit(0);
			 }
			}
			break;
		default:
			printf("\n Usage: histim <image in> <x_lower_bound> "
			       "<x_upper_bound> <y_lower_bound> "
			       "<y_upper_bound> <histogram file> "
			       "<bin size>\n\n");
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
 * Read diffraction image:
 */

  imdiff->infile = imagein;
  if (lreadim(imdiff) != 0) {
    perror(imdiff->error_msg);
    goto CloseShop;
  }

/*
 * Allocate memory for histogram:
 */

  histogram = (size_t *)calloc(65536,sizeof(size_t));
  if (!histogram) {
    perror("Couldn't allocate histogram.\n\n");
    exit(0);
  }

/*
 * Set window parameters:
 */

  imdiff->window_upper = upper_bound;
  imdiff->window_lower = lower_bound;

/*
 * Select pixels in the patch and histogram them:
 */

  for(j=0;j<imdiff->vpixels;j++) {
    for(i=0;i<imdiff->hpixels;i++) {
      if ((i >= imdiff->window_lower.c) && (i <= imdiff->window_upper.c)
	  && (j >= imdiff->window_lower.r) && (j <=
					       imdiff->window_upper.r)) {
	histogram[(imdiff->image[index] -
		  imdiff->image[index] % binsize) + 32768]++;
      }
      index++;
    }
  }

/*
 * Write the output file:
 */

  for(i=0;i<=65535;i=i+binsize) {
    if (histogram[i]>0) {
      fprintf(histout,"%d %d\n",(int)i-32768,histogram[i]);
    }
  }

CloseShop:
  
  lfreeim(imdiff);
  free((size_t *)histogram);

/*
 * Close files:
 */
  
  fclose(imagein);
  fclose(histout);
  
}


