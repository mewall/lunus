/* SQRTRF.C - Take the square root of each element of an rfile.
   
   Author: Veronica Pillar (based off Mike Wall's mulrf.c)
   Date: 9/25/15
   Version: 1.
   
   "sqrtrf <input rfile> <output rfile>"

   Input is one rfile.  Output is the point-by-point square root of that rfile.

   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*infile1,
	*outfile;
  
  size_t
    i;

  DIFFIMAGE 
	*imdiff1;

/*
 * Set input line defaults:
 */
	
	infile1 = stdin;
	outfile = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
		case 3:
			if (strcmp(argv[2], "-") == 0) {
				outfile = stdout;
			}
			else {
			 if ( (outfile = fopen(argv[2],"wb")) == NULL ) {
				printf("Can't open %s.",argv[2]);
				exit(0);
			 }
			}
		case 2:
			if ( (infile1 = fopen(argv[1],"rb")) == NULL ) {
				printf("Can't open %s.",argv[1]);
				exit(0);
			}
			break;
		default:
			printf("\n Usage: sqrtrf <input rfile> "
				"<output rfile>\n\n");
			exit(0);
	}
 
/*
 * Initialize diffraction images:
 */
    
  if ((imdiff1 = linitim()) == NULL) {
    perror("\nCouldn't initialize diffraction image.\n\n");
    exit(0);
  }

 /*
  * Read in rfiles:
  */

   imdiff1->infile = infile1;
   if (lreadrf(imdiff1) != 0) {
     perror(imdiff1->error_msg);
     goto CloseShop;
   }

/*
 * Multiply rfiles:
 */
   lsqrtrf(imdiff1);

/*
 * Write the output rfile:
 */

   imdiff1->outfile = outfile;
   if(lwriterf(imdiff1) != 0) {
     perror(imdiff1->error_msg);
     goto CloseShop;
   }
   
 CloseShop:

/*
 * Free allocated memory:
 */

  lfreeim(imdiff1);

/*
 * Close files:
 */
  
  fclose(infile1);
  fclose(outfile);
  
}
