#include<stdio.h>
#include<stdlib.h>

int main()
{
	long
		lattice_size = 16;

	float
		lower_bound = 0,
		upper_bound = 1;

	fwrite(&lattice_size,sizeof(long),1,stdout);
	fwrite(&lattice_size,sizeof(long),1,stdout);
	fwrite(&lattice_size,sizeof(long),1,stdout);
	fwrite(&lower_bound,sizeof(float),1,stdout);
	fwrite(&upper_bound,sizeof(float),1,stdout);
	fwrite(&lower_bound,sizeof(float),1,stdout);
	fwrite(&upper_bound,sizeof(float),1,stdout);
	fwrite(&lower_bound,sizeof(float),1,stdout);
	fwrite(&upper_bound,sizeof(float),1,stdout);
}
