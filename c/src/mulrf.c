/* MULRF.C - Multiply two rfiles.
   
   Author: Mike Wall
   Date: 1/12/95
   Version: 1.
   
   "mulrf <input rfile 1> <input rfile 2> <output rfile>"

   Input is two rfiles.  Output is product of rfiles, point-by-point.
   second.

   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*infile1,
	*infile2,
	*outfile;
  
  size_t
    i;

  DIFFIMAGE 
	*imdiff1,
	*imdiff2;

/*
 * Set input line defaults:
 */
	
	infile1 = stdin;
	infile2 = stdin;
	outfile = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
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
				infile2 = stdin;
			}
			else {
			 if ( (infile2 = fopen(argv[2],"rb")) == NULL ) {
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
			printf("\n Usage: mulrf <input rfile 1> "
				"<input rfile 2> <output rfile>\n\n");
			exit(0);
	}
 
/*
 * Initialize diffraction images:
 */
    
  if ((imdiff1 = linitim(1)) == NULL) {
    perror("\nCouldn't initialize diffraction image.\n\n");
    exit(0);
  }

  if ((imdiff2 = linitim(1)) == NULL) {
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

   imdiff2->infile = infile2;
   if (lreadrf(imdiff2) != 0) {
     perror(imdiff2->error_msg);
     goto CloseShop;
   }

/*
 * Multiply rfiles:
 */

   lmulrf(imdiff1,imdiff2);

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
  lfreeim(imdiff2);

/*
 * Close files:
 */
  
  fclose(infile1);
  fclose(infile2);
  fclose(outfile);
  
}
