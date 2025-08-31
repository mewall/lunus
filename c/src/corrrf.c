/* CORRRF.C - Calculate the correlation coefficient between two rfiles.
   
   Author: Mike Wall
   Date: 6/27/2025
   Version: 1.
   
   Usage:
   		"corrrf <input rfile 1> <input rfile 2>"

		Input is two rfiles. 

		Output is the correlation coefficient written to stdout.  
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *rfilein1,
    *rfilein2;
  
  char
    error_msg[LINESIZE];
  
  size_t
    imin,
    imax,
    i,
    j,
    k,
    rfile_length,
    ct,
    num_read;
  
  RFILE_DATA_TYPE *rfile1,*rfile2,corr,avg1,avg2;

  rfile1 = (RFILE_DATA_TYPE *)malloc(MAX_RFILE_LENGTH*sizeof(RFILE_DATA_TYPE));
  rfile2 = (RFILE_DATA_TYPE *)malloc(MAX_RFILE_LENGTH*sizeof(RFILE_DATA_TYPE));

  /*
   * Read information from input line:
   */

  imin = 0;
  imax = -1;

  switch(argc) {
    case 5:
      imax = atoi(argv[4]);
    case 4:
      imin = atoi(argv[3]);
    case 3:
    if (strcmp(argv[2],"-") == 0) {
      rfilein2 = stdin;
    }
    else {
      if ( (rfilein2 = fopen(argv[2],"rb")) == NULL ) {
	printf("\nCan't open %s.\n\n",argv[2]);
	exit(0);
      }
    }
    case 2:
    if (strcmp(argv[1],"-") == 0) {
      rfilein1 = stdin;
    }
    else {
      if ( (rfilein1 = fopen(argv[1],"rb")) == NULL ) {
	printf("\nCan't open %s.\n\n",argv[1]);
	exit(0);
      }
    }
    break;
    default:
    printf("\n Usage: corrrf <input rfile 1> <input rfile 2> "
	   "\n\n");
    exit(0);
  }
  
/*
 * Read in rfile 1:
 */

  i=0;

  while ((num_read = fread(&rfile1[i], sizeof(RFILE_DATA_TYPE), 1,
                           rfilein1)) == 1) {
    i++;
  }

 rfile_length = i;

/*
 * Read in rfile 2:
 */

  i=0;

  while ((num_read = fread(&rfile2[i], sizeof(RFILE_DATA_TYPE), 1,
                           rfilein2)) == 1) {
    i++;
  }
 
 if (rfile_length > i) {
   rfile_length = i;
 }

 if (imax == -1) imax = rfile_length-1;

 printf("imin = %ld, imax = %ld\n",imin,imax);

  // Calculate means  

  avg1 = 0.0;
  avg2 = 0.0;
  ct = 0;
      for (i = 0; i < rfile_length; i++) {
        if (i >= imin && i <= imax) {
	  avg1 += rfile1[i];
	  avg2 += rfile2[i];
	  ct++;
	}
      }

  avg1 /= (float)ct;
  avg2 /= (float)ct;

/*
 * Calculate the Pearson Correlation:
 */

  float 
    s11=0.0f,
    s22=0.0f,
    s12=0.0f;

  i=0;
  for (i = 0; i < rfile_length; i++) {
     if (i >= imin && i <= imax) {
	  float d1 = (rfile1[i]-avg1);
	  float d2 = (rfile2[i]-avg2);
	  s11 += d1*d1;
	  s22 += d2*d2;
	  s12 += d1*d2;
     }
  }

  corr = s12/sqrtf(s11)/sqrtf(s22);

  printf("%g\n",corr);

  /*
   * Free allocated memory:
   */

  free(rfile1);
  free(rfile2);

  /*
   * Close files:
   */
  
  fclose(rfilein1);
  fclose(rfilein2);
}

