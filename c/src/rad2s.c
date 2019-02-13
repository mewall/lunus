#include<stdlib.h>
#include<math.h>
#include<stdio.h>

int main()
{
	char input_string[121];
	float x,y;
	float res;

	while (fgets(input_string,120,stdin) != NULL) {
		sscanf(input_string,"%g %g",&x,&y);
		res = 2*sinf(.5*atanf(.0508/56.4*x))/.91;
		printf("%e %e\n",res,y);
	}
}	
