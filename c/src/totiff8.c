/* TOTIFF8.C - Convert a 16-bit 1024x1024 TV6 tiff image to an 8-bit tiff image.
   Author: Mike Wall
   Date: 8/17/93
   Version: 1.0
   Description:

		"totiff8 <input file> <output file>"

		Input is a 16-bit 1024x1024 TV6 tiff image.  
		Output is an 8-bit tiff image.
*/

#define LINESIZE 80
#define DEFAULT_HEADER_LENGTH 4096
#define DEFAULT_FILE_SIZE 2101248

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<errno.h>

union data_union {
	short two_byte;
	struct {
		char low_byte;
		char high_byte;
	} one_byte;
};

int main(int argc, char *argv[])
{
	FILE
		*f_in,
		*f_out;

	char 
		*filename;

	char
		*outstream,
		*header;

	size_t
		filesize,
		imagesize,
		imagelength,
		num_read,
		num_wrote,
		i,
		header_length = DEFAULT_HEADER_LENGTH;

	unsigned short
		handle,
		max,
		min,
		scaled_num;

	short
		*instream;

	union data_union
		pix;

	filename = (char *)malloc(sizeof(char)*(LINESIZE+1));
        if (!filename) {
                printf("Can't allocate image arrays.  Exiting.\n");
                exit(0);
        }
	if (argc < 3) {
		printf("\n     Usage: totiff8 <input file> <output file>\n");
		exit(0);
	}

/*
 * Open input and output files:
 */

	strcpy(filename,argv[1]);
	if ( (f_in = fopen(filename,"rb")) == NULL ) {
		printf("Can't open %s.",filename);
		exit(0);
	}
	strcpy(filename,argv[2]);
	if ( (f_out = fopen(filename,"wb")) == NULL ) {
		printf("Can't open %s.",filename);
		exit(0);
	}


/*
 * Allocate memory for the header:
 */

	header = (char *)malloc(sizeof(char)*(header_length+1));

/*
 * Read the input image:
 */

/*	handle = fileno(f_in);
	if ((filesize = filelength(handle)) == 0) {
		printf("Can't get length of file.\n");
		exit(0);
	}
*/
	filesize = DEFAULT_FILE_SIZE;
	imagesize = filesize-header_length;
	imagelength = imagesize/2;
	outstream = (char *)malloc(sizeof(char)*(imagelength+1));
        instream = (short *)malloc(sizeof(short)*(imagelength+1));
	if (!(outstream && instream)) {
		printf("Can't allocate image arrays.  Exiting.\n");
		exit(0);
	}
	num_read = fread(header,sizeof(char),header_length,f_in);
/*
 * Modify header to indicate 8-bit tiff:
 */

        header[0x88] = 0x08;		/* Bits per pixel */
	header[0xDE] = 0x10;		/* Strip size change */
	header[0x178] = 0x08;		/* TV6 Significant bpp */

	num_read = fread(instream,sizeof(short),imagelength,f_in);
	if (ferror(f_in)) {
		printf("Error reading file %s.",filename);
	}


/*
 * Generate output image:
 */

	i = 0;
	while (i < imagelength) {
		pix.two_byte = instream[i];
		outstream[i] = pix.one_byte.high_byte;
		i++;
	}


/* 
 * Write TIFF header:
 */

	num_wrote = fwrite(header, sizeof(char), header_length, f_out);

/*
 * Write the output image:
 */

	num_wrote = fwrite(outstream, sizeof(char), imagelength, f_out);

/*
 * Free allocated memory:
 */

	free((char *)filename);
	free((short *)instream);
	free((char *)outstream);
	free((char *)header);
}
