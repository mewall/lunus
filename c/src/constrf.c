/* CONSTRF.C - Create an rfile with the value of a constant given at the
		input line.
   
   Author: Mike Wall
   Date: 4/19/94
   Version: 1.
   
   "constrf <input rfile> <constant> <output rfile>"

   Input is an rfile and a constant.  Output is an rfile with all values 
   equal to constant, in the format of the input rfile.

   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*infile,
	*outfile;
  
  size_t
    i;

  DIFFIMAGE 
	*imdiff;

  RFILE_DATA_TYPE
	rfile_value;

/*
 * Set input line defaults:
 */
	
	infile = stdin;
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
			rfile_value = (RFILE_DATA_TYPE)atof(argv[2]);
		case 2:
			if ( (infile = fopen(argv[1],"rb")) == NULL ) {
				printf("Can't open %s.",argv[1]);
				exit(0);
			}
			break;
		default:
			printf("\n Usage: constrf <input rfile> "
				"<constant> <output rfile>\n\n");
			exit(0);
	}
 
/*
 * Initialize diffraction images:
 */
    
  if ((imdiff = linitim()) == NULL) {
    perror("\nCouldn't initialize diffraction image.\n\n");
    exit(0);
  }

 /*
  * Read in rfiles:
  */

   imdiff->infile = infile;
   if (lreadrf(imdiff) != 0) {
     perror(imdiff->error_msg);
     goto CloseShop;
   }

/*
 * Create constant rfile:
 */

   imdiff->rfile[0] = rfile_value;
   lconstrf(imdiff);

/*
 * Write the output rfile:
 */

   imdiff->outfile = outfile;
   if(lwriterf(imdiff) != 0) {
     perror(imdiff->error_msg);
     goto CloseShop;
   }
   
 CloseShop:
  
/*
 * Free allocated memory:
 */

  lfreeim(imdiff);

/*
 * Close files:
 */
  
  fclose(infile);
  fclose(outfile);
  
}

