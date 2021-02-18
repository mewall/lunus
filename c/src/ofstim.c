/* OFSTIM.C - Attempt to detect uniform offset in a diffraction image.
   
   Author: Mike Wall
   Date: 2/16/2021
   Version: 1.
   
   "ofstim <input correction factor> <input image> <inner radius> <outer radius>" 

   Input correction factors and image. Output the detected offset.

   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*cfin,
	*imagein,
	*imageout;
  
  size_t
    i,num_read;

  DIFFIMAGE 
    *imdiff;

  int
    inner_radius = 0,
    outer_radius = 0;

/*
 * Set input line defaults:
 */
	
	cfin = stdin;
	imagein = stdin;
/*
 * Read information from input line:
 */
	switch(argc) {
	case 5:
	  outer_radius = atoi(argv[4]);
		case 4:
		  inner_radius = atoi(argv[3]);
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
			if (strcmp(argv[1], "-") == 0) {
				cfin = stdin;
			}
			else {
			 if ( (cfin = fopen(argv[1],"rb")) == NULL ) {
				printf("Can't open %s.",argv[1]);
				exit(0);
			 }
			}
			break;
		default:
			printf("\n Usage: mulcfim <input correction factor>"
			       "<input image> <output image>\n\n");
			exit(0);
	}
/*
 * Initialize diffraction images:
 */

  if ((imdiff = linitim(1)) == NULL) {
    perror("Couldn't initialize diffraction images.\n\n");
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

// Allocate correction factor

  imdiff->correction = (float *)malloc(imdiff->image_length*sizeof(float));

  // Read correction factor

  num_read = fread(imdiff->correction, sizeof(float), imdiff->image_length,
		 cfin);

  //printf("num_read = %ld\n",num_read);
  if (num_read != imdiff->image_length) {
	perror(imdiff->error_msg);
        goto CloseShop;
  }

  // Calculate the offset:

  imdiff->correct_offset = 1;
  imdiff->correct_offset_inner_radius = inner_radius;
  imdiff->correct_offset_outer_radius = outer_radius;

  if (lofstim(imdiff) != 0) {
    perror(imdiff->error_msg);
    goto CloseShop;
  }


/*
 * Write the offset:
 */

  printf("Correction offset = %g\n",imdiff->correction_offset);

CloseShop:
  
/*
 * Free allocated memory:
 */

  lfreeim(imdiff);

/*
 * Close files:
 */
  
  fclose(imagein);
  fclose(cfin);
}

