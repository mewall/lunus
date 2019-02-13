/* FILTER.C - Select data points from a list which match another in 
   the x-coordinate.
   
   Author: Mike Wall
   Date: 12/31/94
   Version: 1.0
   
   "filter <input list 1> <input list 2> <output list>"
   
   Input is two (x,y) lists.  Output is (x,y) of the second which have x2=x1.
   
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
  
  char
    input_line[LINESIZE],
    *end_of_file;

  float
    x1,
    y1,
    x2,
    y2;

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
    printf("\n Usage: filter <input list 1> "
	   "<input list 2> <output list>\n\n");
    exit(0);
  }
  
  /*
   * Read in lists; output selectively:
   */
  
  while(fgets(input_line,LINESIZE,infile1) != NULL) {
    sscanf(input_line,"%g %g",&x1,&y1);
    do {
      end_of_file = fgets(input_line,LINESIZE,infile2);
      sscanf(input_line,"%g %g",&x2,&y2);
    } while ((x1 != x2) && (end_of_file != NULL));
    fprintf(outfile,"%e %e\n",x2,y2);
  }
  
  /*
   * Close files:
   */
  
  fclose(infile1);
  fclose(infile2);
  fclose(outfile);
  
}



