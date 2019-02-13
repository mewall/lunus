/* AVGRF.C - Average the values of an rfile.
   
   Author: Mike Wall
   Date: 1/12/95
   Version: 1.
   
   "avgrf <input rfile> <output value file>"
   
   Input is one rfile.  Output is average value to output file.
   
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
      if ( (outfile = fopen(argv[2],"w")) == NULL ) {
	printf("Can't open %s.",argv[3]);
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
    printf("\n Usage: avgrf <input rfile> "
	   "<output file>\n\n");
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
   * Read in rfile:
   */
  
  imdiff1->infile = infile1;
  if (lreadrf(imdiff1) != 0) {
    perror(imdiff1->error_msg);
    goto CloseShop;
  }
  
  /*
   * Average rfile:
   */
  
  lavgrf(imdiff1);
  
  /*
   * Write the output rfile:
   */
  
  fprintf(outfile,"%e\n",imdiff1->rfile[0]);
  
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
