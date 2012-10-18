/* REVBYTELT.C - Reverse the byte order for a lattice
   
   Author: Mike Wall
   Date: 10/10/2012
   Version: 1.
   
   Usage:
   		"revbytelt <input file> <output file>"

   Input is a 3D lattice. Output is a 3D lattice with byte order reversed.


   */

#include<mwmask.h>

union fourbytes {
  uint32_t value;
  char bytes[4];
};

int main(argc, argv)
	short argc;
	char *argv[];
{
  FILE
	*infile,
	*outfile;

  char
    error_msg[LINESIZE];

  size_t
	i,
	num_read,
	num_wrote;

  union fourbytes *buf;


/*
 * Set input line defaults:
 */
	
	infile = stdin;
	outfile = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
		case 3:
			if (strcmp(argv[2],"-") == 0) {
				outfile = stdout;
			}
			else {
			 if ((outfile = fopen(argv[2],"wb")) == NULL) {
				printf("\nCan't open %s.\n\n",argv[2]);
				exit(0);
			 }
			}
		case 2:
			if (strcmp(argv[1],"-") == 0) {
				infile = stdin;
			}
			else {
			 if ( (infile = fopen(argv[1],"rb")) == NULL ) {
				printf("\nCan't open %s.\n\n",argv[1]);
				exit(0);
			 }
			}
		case 1:
			break;
		default:
			printf("\n Usage: revbytelt <input file> "
				"<output file>\n\n");
			exit(0);
	}
  
/*
 * Allocate memory for lattice:
 */

	fseek(infile,0L,SEEK_END);
	size_t sz = ftell(infile);
	fseek(infile,0L,SEEK_SET);


	if (sz%sizeof(union fourbytes)!=0) {
	  printf("\nFile size is not a multiple of 4\n\n");
	  exit(0);
	}

	size_t nelem = sz/sizeof(union fourbytes);

  buf = (union fourbytes *)malloc(sz);
  if (!buf) {
    printf("\n***Unable to allocate all memory.\n");
    goto CloseShop;
  }
  
/*
 * Reverse byte order:
 */

  union fourbytes tmp;
  size_t j;

  for (i=0;i<nelem;i++) {
    num_read = fread(&buf[i], sizeof(union fourbytes), 1, infile);
    for (j=0;j<4;j++) {
      tmp.bytes[j] = buf[i].bytes[3-j];
    }
    buf[i]=tmp;
  }
  

/*
 * Write lattice to output file:
 */

  num_wrote = fwrite(buf, sizeof(union fourbytes), nelem, 
				outfile);
CloseShop:
  
  /*
   * Free allocated memory:
   */

  free(buf);

  /*
   * Close files:
   */
  
  fclose(infile);
  fclose(outfile);
}

