/* SPLINTRF.C - Spline interpolate an rfile.
   
   Author: Mike Wall
   Date: 6/28/2017
   Version: 1.
   
   Usage:
   		"splintrf <input rfile> <output rfile> <sampling factor>"

		Input is a radial intensity distribution. Output is an interpolated distribution with Nx sampling.
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*infile,
    *outfile;

  char
    error_msg[LINESIZE];

  size_t
	k,
	num_read,
	num_wrote;

  float
    mask_tag = DEFAULT_LATTICE_MASK_TAG;

  int 
    nx,
    rfile_length,
    rfile_int_length;

  RFILE_DATA_TYPE *rfile,*rfile_int;
  void *buf;


  // Input line defaults:

  nx = 10;

/*
 * Read information from input line:
 */
	switch(argc) {
		case 4:
		  nx = atoi(argv[3]);
		case 3:
			if (strcmp(argv[2],"-") == 0) {
				outfile = stdout;
			}
			else {
			 if ( (outfile = fopen(argv[2],"wb")) == NULL ) {
				printf("\nCan't open %s.\n\n",argv[2]);
				exit(0);
			 }
			}
		case 2:
			if (strcmp(argv[1],"-") == 0) {
				infile = stdin;
			} else {
			  if ((infile = fopen(argv[1],"rb")) == NULL) {
			    printf("\nCan't open %s.\n\n",argv[1]);
			    exit(0);
			  }
			}
			break;
		default:
		  printf("\n Usage: splintrf <input rfile> <output rfile> <sampling factor>\n\n");
		  exit(0);
	}
  
/*
 * Read in rfile:
 */

	int end_pos;

	fseek(infile,0,SEEK_END);
	end_pos = ftell(infile);
	fseek(infile,0,SEEK_SET);
	buf = (void *)malloc(end_pos);
	num_read = fread(buf,sizeof(char),end_pos,infile);
	rfile = (RFILE_DATA_TYPE *)buf;
  
	rfile_length = end_pos/(int)sizeof(RFILE_DATA_TYPE);

	
	// First, compute the spline from the rfile

	float *tau, *c;
	int n,ibcbeg,ibcend;
	int l,jd,kk;
	kk = 4;
	jd = 0;
	int i,j;
	
	tau = (float *)malloc(sizeof(float)*rfile_length);
	c = (float *)malloc(sizeof(float)*4*rfile_length);
	ibcbeg = 0;
	ibcend = 0;
	
	// 
	
	
	// populate variables for spline
	
	n=0;
	
	for (i=0;i<rfile_length;i++) {
	  if (rfile[i] != mask_tag) {
	    tau[n]=(float)i;
	    c[4*n] = rfile[i];
	    n++;
	  }
	}
	
	lspline(tau,c,&n,&ibcbeg,&ibcend);

	// Calculate interpolated rfile


	float x;

	rfile_int_length=nx*rfile_length;

	rfile_int = (RFILE_DATA_TYPE *)malloc(sizeof(RFILE_DATA_TYPE)*rfile_int_length);

	for (i=0;i<rfile_int_length;i++) {
	  x = (float)i/(float)(rfile_length);
	  l = (int)x+1;
	  if (x>= tau[0] && x <= tau[n-1]) {
	    rfile_int[i] = lspleval(tau,c,&l,&kk,&x,&jd);
	  } else {
	    rfile_int[i] = mask_tag;
	  }
	}    

/*
 * Write rfile:
 */

	num_wrote = fwrite(rfile_int,sizeof(RFILE_DATA_TYPE),rfile_int_length,outfile);
	if (num_wrote != rfile_int_length) {
	  printf("\nCouldn't write rfile.\n\n");
	  goto CloseShop;
	}

CloseShop:
  

  /*
   * Close files:
   */
  
  fclose(infile);
  fclose(outfile);
}

