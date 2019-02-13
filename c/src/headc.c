/*
 * HEADC.C	- Output the header of a specified file.  Use number of
		characters given on input line.

   Version: 1.0
   Date: 3/17/94

	Input line is:
			"headc -<number of characters> <input file>"

*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char *argv[])
{
	FILE
		*infile;
	
	char
		*header;

	size_t
		header_length,
		num_read,
		num_wrote;

	switch(argc) {
		case 3:
			if (strcmp(argv[2],"-") == 0) {
				infile = stdin;
			}
			else {
			 if ((infile = fopen(argv[2],"rb")) == NULL) {
				printf("Can't open %s.",argv[2]);
				exit(0);
			 }
			}
		case 2:
			if (argv[1][0] == '-') {
			  if (!(header_length = atol(argv[1]+1))) {
			    printf("Incorrect syntax for header length.\n");
			    exit(0);
			  }
			}
			else {
				printf("Must have '-' in front "
					"of header length.\n");
				exit(0);
			}
			break;
		default:
			printf("\n   Usage: headc -<number of chars> "
				"<input file>\n\n");
	}
	header = (char *)malloc(header_length+1);
	num_read = fread(header, sizeof(char), header_length, infile);
	num_wrote = fwrite(header, sizeof(char), header_length, stdout);

	free((char *)header);
}
