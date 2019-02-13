/* SCALEIM.C - Scale one image to another pixel-by-pixel within a radial region.
   
   Author: Mike Wall
   Date: 10/25/2016
   Version: 1.
   
   "scaleim <input image 1> <input image 2> <inner radius> <outer radius>"

   Input two diffraction images.  Output is scale factor between the two, computed using pixels in between inner and outer radius. 

   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *imagein1,
    *imagein2;
  
  size_t
    i,
    num_wrote,
    num_read;

  DIFFIMAGE 
	*imdiff1,
	*imdiff2;

  short 
	inner_radius = 0,
	outer_radius = 0;	

/*
 * Read information from input line:
 */
	switch(argc) {
		case 5:
			outer_radius = (short)atoi(argv[4]);
		case 4:
			inner_radius = (short)atoi(argv[3]);
		case 3:
			if (strcmp(argv[2], "-") == 0) {
				imagein2 = stdin;
			}
			else {
			 if ( (imagein2 = fopen(argv[2],"rb")) == NULL ) {
				printf("Can't open %s.",argv[2]);
				exit(0);
			 }
			}
		case 2:
			if (strcmp(argv[1], "-") == 0) {
				imagein1 = stdin;
			}
			else {
			 if ( (imagein1 = fopen(argv[1],"rb")) == NULL ) {
				printf("Can't open %s.",argv[1]);
				exit(0);
			 }
			}
			break;
		default:
			printf("\n Usage: scaleim <input image 1> "
				"<input image 2> "
				"<inner radius> <outer radius>\n\n");
			exit(0);
	}
/*
 * Initialize diffraction images:
 */

  if (((imdiff1 = linitim()) == NULL) || ((imdiff2 = linitim()) == NULL)) {
    perror("Couldn't initialize diffraction images.\n\n");
    exit(0);
  }

  //printf("scaleim:Made it past diffraction image initialization.\n\n");
/*
 * Set main defaults:
 */

	imdiff1->scale_inner_radius = inner_radius;
	imdiff1->scale_outer_radius = outer_radius;

 
 /*
  * Read diffraction image:
  */

  imdiff1->infile = imagein1;
  if (lreadim(imdiff1) != 0) {
    perror(imdiff1->error_msg);
    goto CloseShop;
  }

  imdiff2->infile = imagein2;
  if (lreadim(imdiff2) != 0) {
    perror(imdiff2->error_msg);
    goto CloseShop;
  }

  if (lscaleim(imdiff1,imdiff2) != 0) {
    perror(imdiff2->error_msg);
    goto CloseShop;
  }

  printf("%f %f\n",imdiff1->rfile[0],imdiff1->rfile[1]);

 CloseShop:
  
/*
 * Free allocated memory:
 */

  lfreeim(imdiff1);
  lfreeim(imdiff2);

/*
 * Close files:
 */
  
  fclose(imagein1);
  fclose(imagein2);
}

