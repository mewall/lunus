/* MINRIM.C - Calculate the minimum intensity as a function of radius for an
		input image.
   
   Author: Mike Wall   
   Date: 4/4/93
   Version: 1.
   
   "minrim <input image> <output rfile> <x origin> <y origin>"

   Input is TIFF TV6 image.  Output is a list of values of I(r).

   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*imagein,
	*outfile;
  
  char
    error_msg[LINESIZE];

  size_t
    num_wrote,
    num_read;

  DIFFIMAGE 
	*imdiff;

  struct rccoords origin;

/*
 * Set input line defaults:
 */
	
	imagein = stdin;
	outfile = stdout;
	origin.r = DEFAULT_IMAGE_ORIGIN;
	origin.c = DEFAULT_IMAGE_ORIGIN;

/*
 * Read information from input line:
 */
	switch(argc) {
		case 5: 
			origin.r = (RCCOORDS_DATA)atoi(argv[4]);
		case 4:
			origin.c = (RCCOORDS_DATA)atoi(argv[3]);
		case 3:
			if ( (outfile = fopen(argv[2],"wb")) == NULL ) {
				printf("Can't open %s.",argv[2]);
				exit(0);
			}
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
			printf("\n Usage: minrim <input image> "
				"<output rfile> \n\n");
			exit(0);
	}
  
 /*
  * Initialize diffraction image:
  */

  if ((imdiff = linitim(1)) == NULL) {
    perror("Couldn't initialize diffraction image.\n\n");
    exit(0);
  }

/*
 * Set main defaults:
 */

	imdiff->origin = origin;
  

/*
 * Read diffraction image:
 */
	imdiff->infile = imagein;
	if (lreadim(imdiff) != 0) {
		perror(imdiff->error_msg);
		goto CloseShop;
	}


  /*
   * Generate rfile:
   */
  
	lminrim(imdiff);  

  /*
   * Write the output rfile:
   */

   imdiff->outfile = outfile;
   if(lwriterf(imdiff) != 0) {
     perror(imdiff->error_msg);
     goto CloseShop;
   }

  num_wrote = fwrite(imdiff->rfile, sizeof(RFILE_DATA_TYPE), 
		imdiff->rfile_length, outfile);
  if (num_wrote != imdiff->rfile_length){
    printf("Only wrote %ld words to rfile.", num_wrote);
  }
  
CloseShop:
  
/*
 * Free allocated memory:
 */

  lfreeim(imdiff);

  /*
   * Close files:
   */
  
  fclose(imagein);
  fclose(outfile);
  
}

