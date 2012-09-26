/* LGETPKS.C - Extract the peak values from a DENZO output file.

   Author: Mike Wall
   Date: 9/27/95
   Version: 1.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mwmask.h>

#define XPOS 49
#define XLEN 7
#define YPOS 57
#define YLEN 7
#define DEFAULT_STOPSTRING " 999"

int lgetpks(DIFFIMAGE *imdiff)
{
	char *inl;
	char *stopstring;
	long i;

/*
 * Allocate memory:
 */

	inl = (char *)malloc(sizeof(char)*(LINESIZE+1));
	stopstring = (char *)malloc(sizeof(char)*
			(strlen(DEFAULT_STOPSTRING)+1));

/*
 * Assign stopstring:
 */

	strcpy(stopstring, DEFAULT_STOPSTRING);

/*
 * Skip the first five lines in the file:
 */

	for(i=1;i<=5;i++) {
		fgets(inl, LINESIZE, imdiff->infile);
	}
	i=0;
	while ( (fgets(inl, LINESIZE, imdiff->infile) != NULL) &&
		!((inl[1]==stopstring[1]) && (inl[2]==stopstring[2])&& 
		(inl[3]==stopstring[3]))) {

/*
 * Extract peak position:
 */
		inl[XPOS+XLEN]=0;
		imdiff->peak[i].x = atof(inl+XPOS);
		inl[YPOS+YLEN]=0;
		imdiff->peak[i].y = atof(inl+YPOS);
		i++;
	}
	imdiff->peak_count = i;

/*
 * Free memory:
 */

	free((char *)inl);
	free((char *)stopstring);

	return(0);
}
