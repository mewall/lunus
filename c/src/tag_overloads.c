/* TAG_OVERLOADS.C - Change 32767 to -1 (65535) in an input 16-bit greyscale
		image with a 4096 byte header (tv6 TIFF file).

   Author: Mike Wall
   Date: 2/26/94
   Version: 1.0
   Description:

		"tag_overloads <input file> <output file>" 

		Input is a 16-bit 1024x1024 TV6 tiff image.  
		Output is a 16-bit tiff image.
*/

#define LINESIZE 120
#define DEFAULT_HEADER_LENGTH 4096
#define DEFAULT_FILE_SIZE 2101248
#define MAX_PIXEL_VALUE 0xff7f		/* Remember...reversed byte order. */
#define OVERFLOW_TAG 0xffff		/* 65535 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char *argv[])
{

	short ltag_overloads(unsigned short *image, size_t imagelength, 
				short max_pixel_value, short overflow_tag);

	FILE
		*f_in,			/* Input file */
		*f_out;			/* Output file */

	char 
		*filename;		/* Generic filename string */

	char
		*header;		/* TIFF header buffer */

	size_t
		filesize,		/* Size of the file in bytes */
		imagesize,		/* Sz of the img (no hdr) in bytes */
		imagelength,		/* Sz of img in pixels */
		num_read,		/* Number of structs read */
		num_wrote,		/* Number of structs written */
		i,			/* Counting index */
		header_length = DEFAULT_HEADER_LENGTH; /* header length in chars */

	unsigned short
		*image;		/* Buffer for image */

	filename = (char *)malloc(sizeof(char)*(LINESIZE+1));
        if (!filename) {
                printf("\nCan't allocate image arrays.  Exiting.\n\n");
                exit(0);
        }
	if (argc < 3) {
		printf("\n     Usage: tag_overloads <input file> "
			"<output file>\n\n");
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
	if (!header) {
		printf("Can't allocate image arrays.  Exiting.\n");
		exit(0);
	}

/*
 * Read the input image:
 */

	filesize = DEFAULT_FILE_SIZE;
	imagesize = filesize-header_length;
	imagelength = imagesize/2;
        image = (unsigned short *)malloc(sizeof(unsigned short)*
			(imagelength+1));
	if (!image) {
		printf("\nCan't allocate image array.  Exiting.\n\n");
		exit(0);
	}

/*
 * Read header:
 */

	num_read = fread(header,sizeof(char),header_length,f_in);

/*
 * Read image:
 */

	num_read = fread(image,sizeof(unsigned short),imagelength,f_in);
	if (ferror(f_in)) {
		printf("Error reading file %s.",filename);
	}


/*
 * Replace maximum pixel value with standard overflow tag:
 */

	ltag_overloads(image, imagelength, MAX_PIXEL_VALUE, OVERFLOW_TAG);

	i = 0;
	while (i < imagelength) {
		if (image[i] == MAX_PIXEL_VALUE) {
			image[i] = OVERFLOW_TAG;
		}
		i++;
	}


/* 
 * Write TIFF header:
 */

	num_wrote = fwrite(header, sizeof(char), header_length, f_out);

/*
 * Write the output image:
 */

	num_wrote = fwrite(image, sizeof(short), imagelength, f_out);

/*
 * Free allocated memory:
 */

	free((char *)filename);
	free((short *)image);
	free((char *)header);
}

short ltag_overloads(unsigned short *image, size_t imagelength, 
			short max_pixel_value, short overflow_tag)
{
	long 
		i;

	i = 0;
	while (i < imagelength) {
		if (image[i] == max_pixel_value) {
			image[i] = overflow_tag;
		}
		i++;
	}
	return(0);
}
