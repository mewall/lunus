/*
 * TAILC.C	- Output the tail of a specified file.  Use number of
		characters given on input line.

   Version: 1.0
   Date: 3/17/94

	Input line is:
			"tailc -<number of characters> <input file>"

*/

#include<lunus.h>

int main(int argc, char *argv[])
{
	FILE
		*infile;
	
	char
		*tail,
		*head;

	size_t
		header_length,
		filesize,
		tail_length,
		num_read,
		num_wrote;

	switch(argc) {
		case 4:
			if (strcmp(argv[3],"-") == 0) {
				infile = stdin;
			}
			else {
			 if ((infile = fopen(argv[3],"rb")) == NULL) {
				printf("Can't open %s.",argv[2]);
				exit(0);
			 }
			}
		case 3:
			if (!(filesize = atol(argv[2]))) {
			  printf("Invalid file size.\n");
			  exit(0);
			}
		case 2:
			if (argv[1][0] == '-') {
			  if (!(tail_length = atol(argv[1]+1))) {
			    printf("Incorrect syntax for tail length.\n");
			    exit(0);
			  }
			}
			else {
				printf("Must have '-' in front "
					"of tail length.\n");
				exit(0);
			}
			break;
		default:
			printf("\n   Usage: tailc -<number of chars> "
				"<input file>\n\n");
	}
	header_length = filesize - tail_length;
	tail = (char *)malloc(tail_length+1);
	head = (char *)malloc(header_length+1);
	num_read = fread(head, sizeof(char), header_length, infile);
	num_read = fread(tail, sizeof(char), tail_length, infile);
	num_wrote = fwrite(tail, sizeof(char), tail_length, stdout);

	free((char *)tail);
	free((char *)head);
}
