/* RF2CHAR.C - Convert an ascii rfile from tv6 to a list of 8-bit integers, 
		stripping the x-axis.
   Author: Mike Wall
   Date: 3/25/94
   Version: 1.0
   Description:

		"rf2char <lower threshold> <upper threshold>"

		Input is an ascii rfile.  
		Output is an 8-bit list of densitometry values.


   Somewhat butchered by Veronica Pillar 4/14/14; I recommend not using this.
*/

#define DEFAULT_LOWER_THRESHOLD 0
#define DEFAULT_UPPER_THRESHOLD 32767
#define MAX_VALUE 32767

#include<mwmask.h>

int main(int argc, char *argv[])
{
	short
	    upper_threshold = DEFAULT_UPPER_THRESHOLD,
	    lower_threshold = DEFAULT_LOWER_THRESHOLD;

	float
		value,
		scale;

	int
		index;

	char 
		output_value,
		inl[sizeof(RFILE_DATA_TYPE)];

	switch(argc) {
		case 3:
		upper_threshold = atoi(argv[2]);
		case 2:
		lower_threshold = atoi(argv[1]);
		break;
		default:
			printf("\nUsage: rf2char <lower threshold> "
					"<upper threshold>\n\n");
			exit(0);
	}
	scale = 255./(float)(upper_threshold - lower_threshold);
	while (fgets(inl,sizeof(RFILE_DATA_TYPE),stdin) != NULL) {
		//sscanf(inl,"%d %g", &index, &value);
		sscanf(inl,"%g", &value);
		printf(" value = %g\n", value);
		fflush(stdout);
		if (value < (float)lower_threshold) {
			output_value = 0;
		}
		else if (value > (float)upper_threshold) {
			output_value = 255;
		}
		else {
			output_value = (char)((scale * (value - 
				(float)lower_threshold)));
		}
		fwrite(&output_value, sizeof(RFILE_DATA_TYPE), 1, stdout);
	}
}
